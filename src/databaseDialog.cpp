#include "../include/databaseDialog.h"
#include "../ui/ui_databaseDialog.h"
#include "../include/statemgr.h"

DatabaseDialog::DatabaseDialog(QWidget *parent, core::StateMgr *ptrStateMgr)
    : QWidget(parent)
    , ui(new Ui::DatabaseDialog)
    ,ptrStateMgr_(ptrStateMgr)
{
    ui->setupUi(this);
    QSqlQuery q = ptrStateMgr_->getRoot();

    pushButton_database_delete_ = this->ui->pushButton_database_delete;
    pushButton_quit_ = this->ui->pushButton_quit;
    pushButton_save_ = this->ui->pushButton_save;
    pushButton_submitPassword_ = this->ui->pushButton_submitPassword;

    label_id_ = this->ui->label_id;
    label_createdAt_ = this->ui->label_createdAt;
    label_passwordsDoNotMatch_ = this->ui->label_passwordsDoNotMatch;

    lineEdit_name_ = this->ui->lineEdit_name;
    lineEdit_passwordOld_ = this->ui->lineEdit_passwordOld;
    lineEdit_passwordNew_ = this->ui->lineEdit_passwordNew;
    lineEdit_passwordNewRepeat_ = this->ui->lineEdit_passwordNewRepeat;

    comboBox_currency_ = this->ui->comboBox_currency;

    label_passwordsDoNotMatch_->setHidden(true);

    QStringList currency = {
        "EUR", "USD", "JPY", "GBP", "AUD", "CAD", "CHF", "RMB", "SEK", "NZD"
    };

    // Query aktivieren
    q.first();
    label_id_->setText(q.value("id").toString());
    label_createdAt_->setText(q.value("created_at").toString().left(10));
    lineEdit_name_->setText(q.value("database_name").toString());

    currency.removeAll(q.value("currency").toString());

    comboBox_currency_->addItem(q.value("currency").toString());
    comboBox_currency_->addItems(currency);
    comboBox_currency_->setCurrentIndex(0);

    connect(pushButton_save_, &QPushButton::clicked, this, &DatabaseDialog::save_data_);
    connect(pushButton_quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });
    connect(pushButton_database_delete_, &QPushButton::clicked, this, &DatabaseDialog::delete_database_); // nicht implementiert
    connect(pushButton_submitPassword_, &QPushButton::clicked, this, &DatabaseDialog::submit_passwort_change_);
    connect(lineEdit_passwordNew_, &QLineEdit::textChanged, this, [this](){
        bool ok;
        ok = this->checkPasswordsAreEqual_();
        if (!ok) return;
        this->checkPasswordLength_();
    });
    connect(lineEdit_passwordNewRepeat_, &QLineEdit::textChanged, this, [this](){
        bool ok;
        ok = this->checkPasswordsAreEqual_();
        if (!ok) return;
        this->checkPasswordLength_();
    });
}

DatabaseDialog::~DatabaseDialog()
{
    delete ui;
}

// Speichert die Änderungen in der Datenbank
// Passwortänderungen werden hier nicht bearbeitet
void DatabaseDialog::save_data_()
{
    QMap<QString, QVariant> form_data;
    // Pflichtfelder prüfen
    if (comboBox_currency_->currentText() == "" || lineEdit_name_->text() == ""){
        QMessageBox::information(nullptr, "Hinweis", "Bitte fülle alle mit * markierten Pflichtfelder aus");
        return;
    }
    bool ok;
    form_data["id"] = label_id_->text().toInt(&ok);
    if (!ok){
        qDebug() << "Could not convert toInt";
        form_data["id"] = constants::ROOT_ID; // Fallback zu id = 1, es darf keine weitere ID vorkommen...
    }
    form_data["database_name"] = lineEdit_name_->text();
    form_data["currency"] = comboBox_currency_->currentText();

    ok = this->ptrStateMgr_->db->update_root(form_data);
    if (!ok){
        qDebug() << "Could not update database";
        QMessageBox::information(nullptr, "Hinweis", "Ein Fehler beim Aktualisieren der Datenbank ist aufgetreten");
        return;
    }
    QMessageBox::information(nullptr, "Hinweis", "Daten wurden erfolgreich gespeichert");

}

void DatabaseDialog::submit_passwort_change_()
{
    QString old_password = lineEdit_passwordOld_->text();
    if (old_password == ""){
        QMessageBox::warning(nullptr, "Hinweis", "Bitte gebe dein altes Passwort an");
        return;
    }

    QString password1 = lineEdit_passwordNew_->text();
    QString password2 = lineEdit_passwordNewRepeat_->text();
    if (password1 != password2){
        QMessageBox::warning(nullptr, "Hinweis", "Neue Passwörter stimmen nicht überein");
        return;
    }
    if (password1.length() < 8){
        QMessageBox::warning(nullptr, "Hinweis", "Passwort ist zu kurz. Bitte vergebe ein Passwort mit mindestens 8 Zeichen");
        return;
    }
    QMap<QString, QString> passwords;
    passwords["old"] = old_password;
    passwords["new"] = password1;


    // Hier muss jetzt der Submit erfolgen...
    QMessageBox::information(nullptr, "Hinweis", "Passwort wurde erfolgreich geändert");

}

void DatabaseDialog::delete_database_()
{
    // Kann später eventuell noch implementiert werden
    // Datenbanken können solange einfach über den Windows Explorer entfernt werden
    QMessageBox::warning(nullptr, "Hinweis", "Die Datenbank kann aktuell nicht gelöscht werden.");
}

// ** Passwortvalidierung bei neuem Datenbankdialog **
// Prüfung, ob Passwortwiederholung passt
bool DatabaseDialog::checkPasswordsAreEqual_()
{
    const QString password1 = this->lineEdit_passwordNew_->text();
    const QString password2 = this->lineEdit_passwordNewRepeat_->text();
    this->label_passwordsDoNotMatch_->setText("❌ keine Übereinstimmung!");

    const bool check = (password1 == password2);
    (check) ? this->label_passwordsDoNotMatch_->setHidden(true)
            : this->label_passwordsDoNotMatch_->setHidden(false);
    return check;
}
// Prüfung, ob mindestens 8 Zeichen vergeben wurden
bool DatabaseDialog::checkPasswordLength_()
{
    // 8 Zeichen sollte das Datenbankpasswort mindestens haben
    const int min_required_length = 8;
    if (min_required_length > lineEdit_passwordNew_->text().length()){
        this->label_passwordsDoNotMatch_->setText("❌ zu kurz (min. 8 Zeichen)!");
        this->label_passwordsDoNotMatch_->setHidden(false);
        return false;
    }
    else {
        this->label_passwordsDoNotMatch_->setHidden(true);
        return true;
    }
}
