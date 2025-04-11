#include "../include/addDatabaseDialog.h"
#include "../ui/ui_addDatabaseDialog.h"

AddDatabaseDialog::AddDatabaseDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddDatabaseDialog)
{
    ui->setupUi(this);
    currency = {
        "EUR", "USD", "JPY", "GBP", "AUD", "CAD", "CHF", "RMB", "SEK", "NZD"
    };
    // ComboBox für die Währungsauswahl (default EUR)
    cbx_currency = this->ui->comboBox_currency;
    cbx_currency->addItems(currency);
    cbx_currency->setCurrentIndex(0);

    lineEdit_balance = this->ui->lineEdit_balance;
    lineEdit_balance->setMaxLength(12);

    lineEdit_password = this->ui->lineEdit_password;
    lineEdit_repeatPassword = this->ui->lineEdit_repeatPassword;
    current_password_length = 0;

    // Maximale Passwortlänge auf 30 setzen
    lineEdit_password->setMaxLength(30);
    lineEdit_repeatPassword->setMaxLength(30);

    //RegEx für Passwort um sicherzustellen, dass keine Whitespaces eingefügt werden
    lineEdit_password->setValidator(new QRegularExpressionValidator(QRegularExpression(R"(\S*)"), lineEdit_password));
    lineEdit_repeatPassword->setValidator(new QRegularExpressionValidator(QRegularExpression(R"(\S*)"), lineEdit_repeatPassword));

    // Signal, Slot für Passwordfeld Change Event
    connect(lineEdit_password, &QLineEdit::textChanged, this, [this](){
        this->checkPasswordsAreEqual_();
        current_password_length = lineEdit_password->text().size();
        this->checkPasswordsLength_();
    });
    connect(lineEdit_repeatPassword, &QLineEdit::textChanged, this, [this](){
        this->checkPasswordsAreEqual_();
    });

    // RegEx um sicherzustellen, dass nur Zahlen beim Eröffnungssaldo angegeben werden dürfen
    // Erlaubt nur Zahlen von 0-9, ein Komma, keine führende Null (z.B. 09,23) und keine Buchstaben
    lineEdit_balance->setValidator(new QRegularExpressionValidator(QRegularExpression("^(0|[1-9][0-9]*)(,[0-9]{1,2})?$"), lineEdit_balance));

    lineEdit_dbname = this->ui->lineEdit_dbName;
    lineEdit_dbname->setMaxLength(20);

    // RegEx für sicheren Dateinamen, der vom Nutzer in DbName vorgegeben wird
    lineEdit_dbname->setValidator(new QRegularExpressionValidator(QRegularExpression("^[A-Za-z0-9]+$"), lineEdit_dbname));

    //lbl_currency wird automatisch aktualisiert wenn in der ComboBox eine andere Währung ausgewählt wird
    lbl_currency = this->ui->lbl_curr_edit;
    lbl_currency->setText(cbx_currency->currentText());

    connect(cbx_currency, &QComboBox::currentIndexChanged, this, [this](){
        this->lbl_currency->setText(this->cbx_currency->currentText());
    });

    btn_back = this->ui->pushButton_back;

    // Signal, Slot für Button Zurück
    connect(btn_back, &QPushButton::clicked, this, [this](){
        this->btnClickEventBack();
    });

    btn_submit = this->ui->pushButton_submit;

    // Signal, Slot für Button Weiter (Submit)
    connect(btn_submit, &QPushButton::clicked, this, [this](){
        this->btnClickEventSubmit();
    });

    // Label für Passwort Check als hidden markieren
    lbl_passwordsDoNotMatch = this->ui->lbl_passwordsDoNotMatch;
    lbl_passwordsDoNotMatch->setHidden(true);

}
AddDatabaseDialog::~AddDatabaseDialog()
{
    delete ui;
}

// Button-Handler um zurück zum Welcome Screen zu gelangen ("Zurück")
void AddDatabaseDialog::btnClickEventBack(){
    QPointer<WelcomeScreen> w = new WelcomeScreen();
    bool init = initApp::checkInit();
    if (!init){
        w->showOpenDbDialog();
    }
    w->show();

    // Sauberes löschen aus dem Heap
    this->close();
    this->deleteLater();
}

// Button-Handler um Form Data zu übermitteln.
// Zuvor wird Dateneingabe validiert durch validateData_()
bool AddDatabaseDialog::btnClickEventSubmit()
{
    const bool data_valid = this->validateData_();
    if (!data_valid){
        return false;
    }
    this->setCursor(Qt::WaitCursor);
    if (db::Database::createDb(form_data)){
        QMessageBox::information(nullptr, "Hinweis", "Die neue Datenbank wurde erfolgreich hinzugefügt");
        QPointer<MainWindow> w = new MainWindow(
            nullptr,
            constants::DB_PATH,
            form_data.value("db_name") + constants::DB_FILE_EXT,
            false,
            "",
            "default"
            );
        w->show();
        this->close();
        this->deleteLater();
        this->unsetCursor();
        return true;
    }
    else {
        throw std::runtime_error("Something went wrong while creating the database");
        return false;
    }
}

// Private Methode, Validierung der angegebenen Daten
bool AddDatabaseDialog::validateData_()
{
    // Form Data initialisieren von allen QWidgets
    form_data["db_name"] =  this->ui->lineEdit_dbName->text().trimmed(); // Name der Datenbank
    form_data["currency"] =  cbx_currency->currentText().trimmed(); // Datenbankwährung für alle Konten
    form_data["account_name"] =  this->ui->lineEdit_accountName->text().trimmed(); // Name des Kontos
    form_data["account_holder"] =  this->ui->lineEdit_accountHolder->text().trimmed(); // Name des Kontoinhabers (optional)
    form_data["bank_name"] =  this->ui->lineEdit_bankName->text().trimmed(); // Name der Bank (optional)
    form_data["balance"] =  this->ui->lineEdit_balance->text(); // Eröffnungssaldo (optional, default 0)
    form_data["password"] = lineEdit_password->text();
    form_data["repeat_password"] = lineEdit_repeatPassword->text();

    if (form_data["balance"] == ""){
        form_data["balance"] = "0";
    }

    // Validierung, ob Pflichtfelder ausgefüllt wurden
    if (
        form_data["db_name"] == "" ||
        form_data["currency"] == "" ||
        form_data["account_name"] == "" ||
        form_data["password"] == "" ||
        form_data["repeat_password"] == ""
        ) {
        QMessageBox::information(nullptr, "Hinweis", "Bitte fülle alle Pflichtfelder aus (Name der Datenbank, Währung, Name des Kontos, Passwort, Passwort wiederholen");
        return false;
    }

    // Prüfung, ob bereits eine Datenbank mit selben Namen angelegt wurde
    // Falls ja --> Abbruch
    QDir path(constants::ROOT + "/" + constants::DB_PATH);
    if (path.exists()){
        QStringList filter;
        filter.append("*" + constants::DB_FILE_EXT);
        QStringList entries = path.entryList(filter, QDir::Files);
        QString db_name = form_data.value("db_name") + constants::DB_FILE_EXT;

        for (auto& item: entries){
            if (item == db_name){
                QMessageBox::information(nullptr, "Hinweis", "Der angegebene Datenbankname exisitiert bereits. Bitte wähle eine andere Bezeichnung.");
                return false;
            }
        }
    }
    // Prüfung, ob Passwörter übereinstimmen
    if (!(form_data["password"] == form_data["repeat_password"])){
        QMessageBox::information(nullptr, "Hinweis", "Passwörter stimmen nicht überein!");
        return false;
    }
    // Mindestanzahl an Zeichen für neues Passwort
    if (form_data["password"].length() < 8){
        QMessageBox::information(nullptr, "Hinweis", "Passwort ist zu kurz, bitte vergebe ein Passwort mit mindestens 8 Zeichen");
        return false;
    }

    return true;
}

// ** Passwortvalidierung bei neuem Datenbankdialog **
// Prüfung, ob Passwortwiederholung passt
void AddDatabaseDialog::checkPasswordsAreEqual_()
{
    const QString password1 = this->lineEdit_password->text();
    const QString password2 = this->lineEdit_repeatPassword->text();
    this->lbl_passwordsDoNotMatch->setText("❌ Passwörter stimmen nicht überein!");

    const bool check = (password1 == password2);
    (check) ? this->lbl_passwordsDoNotMatch->setHidden(true)
            : this->lbl_passwordsDoNotMatch->setHidden(false);
}
// Prüfung, ob mindestens 8 Zeichen vergeben wurden
void AddDatabaseDialog::checkPasswordsLength_()
{
    // 8 Zeichen sollte das Datenbankpasswort mindestens haben
    const int min_required_length = 8;
    if (min_required_length > this->current_password_length){
        this->lbl_passwordsDoNotMatch->setText("❌ Passwort zu kurz (min. 8 Zeichen)!");
        this->lbl_passwordsDoNotMatch->setHidden(false);
    }
    else {
        this->lbl_passwordsDoNotMatch->setHidden(true);
    }
}
