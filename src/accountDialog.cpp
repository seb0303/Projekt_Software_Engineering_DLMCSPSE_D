#include "../include/accountDialog.h"
#include "../ui/ui_accountDialog.h"

#include "../include/statemgr.h"
#include "../include/account.h"
#include "../include/iconPickerDialog.h"

AccountDialog::AccountDialog(QWidget *parent, core::StateMgr *ptrStateMgr)
    : QWidget(parent)
    , parent_(parent)
    , ui(new Ui::AccountDialog)
    , ptrStateMgr_(ptrStateMgr)
{
    ui->setupUi(this);

    // Account Objekt initialisieren
    // Verwendungszweck: Verbindung zwischen Account Dialog und Datenbank (CRUD)
    account_ = new Account(this, ptrStateMgr_);
    // Properties definieren
    account_id_ = 0;
    QString currency_str = ptrStateMgr_->getCurrency();
    currency_id_ = ptrStateMgr_->getRootId();
    operation_done_ = false;

    // Komponenten initialisieren
    listWidget_accounts_ = this->ui->listWidget_accounts;
    groupBox_accountData_ = this->ui->groupBox_accountData;
    lineEdit_name_ = this->ui->lineEdit_name;
    lineEdit_accountHolder_ = this->ui->lineEdit_accountHolder;
    label_currency_ = this->ui->label_currency;
    label_currency2_ = this->ui->label_currency2;
    lineEdit_accountNumber_ = this->ui->lineEdit_accountNumber;
    lineEdit_bankName_ = this->ui->lineEdit_bankName;
    lineEdit_bankCode_ = this->ui->lineEdit_bankCode;
    lineEdit_iban_ = this->ui->lineEdit_iban;
    lineEdit_bic_ = this->ui->lineEdit_bic;
    lineEdit_openingBalance_ = this->ui->lineEdit_openingBalance;
    label_icon_ = this->ui->label_icon;
    label_id_ = this->ui->label_accountId;
    label_id_->setStyleSheet("font-weight:700;");
    // Buttons initialisieren
    pushButton_account_add_ = this->ui->pushButton_account_add;
    pushButton_account_delete_ = this->ui->pushButton_account_delete;
    pushButton_changeIcon_ = this->ui->pushButton_change_icon;
    pushButton_quit_ = this->ui->pushButton_quit;
    pushButton_save_ = this->ui->pushButton_save;

    // Alle LineEdits in einem Container sammeln für schnellen Zugriff via Loop
    allLineEdits_ = {
        lineEdit_name_,
        lineEdit_accountHolder_,
        lineEdit_accountNumber_,
        lineEdit_bankName_,
        lineEdit_bankCode_,
        lineEdit_iban_,
        lineEdit_bic_,
        lineEdit_openingBalance_
    };
    for (QLineEdit* el: allLineEdits_){
        if (el){
            el->setMaxLength(30);
        }
    }
    // Für opening balance wird noch auf 12 stellen beschränkt, das sollte reichen...
    lineEdit_openingBalance_->setMaxLength(12);

    // Currency setzen (bleibt unverändert da Datenbankwährung fix)
    label_currency_->setText(currency_str);
    label_currency_->setStyleSheet("font-weight:700; margin-left: 3px;");
    label_currency2_->setText(currency_str);
    label_currency2_->setStyleSheet("font-weight:700;");

    // Abruf der Konten und schreiben in das ListWidget
    this->getAllAccountsMasterdata_();

    // Connect für ListWidget change Event
    connect(listWidget_accounts_, &QListWidget::currentItemChanged, this, [this](){
        QListWidgetItem* item = listWidget_accounts_->currentItem();
        if (!item) return;
        int id = item->data(Qt::UserRole).toInt();
        this->getSingleAccountMasterdata_(id);
    });
    // Aktuellen Operation Type setzen (Edit oder New)
    // Wird benötigt, um zu prüfen ob ein bestehendes Item bearbeitet oder ein neues Item angelegt wird
    if (listWidget_accounts_->count() > 0){
        listWidget_accounts_->setCurrentRow(0); // connect aufrufen
    }

    // Connects für alle Buttons
    // Button Add new Account
    connect(pushButton_account_add_, &QPushButton::clicked, this, [this](){
        this->setNewAccountDialog_();
    });
    // Button quit
    connect(pushButton_quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });
    // Button Icon Dialog öffnen
    connect(pushButton_changeIcon_, &QPushButton::clicked, this, [this](){
        IconPickerDialog* w = this->generateIconWidget_();
        w->show();
    });
    // Button save
    connect(pushButton_save_, &QPushButton::clicked, this, [this](){
        this->saveData();
    });
    // Button delete
    connect(pushButton_account_delete_, &QPushButton::clicked, this, [this](){
        this->deleteAccount();
    });
}

AccountDialog::~AccountDialog()
{
    if (this->operation_done_){
        QApplication::setOverrideCursor(Qt::WaitCursor);
        ptrStateMgr_->setAll();
        QApplication::restoreOverrideCursor();
    }
    delete ui;
    qDebug() << "AccountDialog deconstructed";
}

void AccountDialog::setNewAccountDialog_()
{
    operation_type_ = "new";
    account_id_ = 0;
    this->label_id_->setText("Neuer Datensatz");
    groupBox_accountData_->setTitle("Neues Konto anlegen");
    listWidget_accounts_->setCurrentRow(-1);
    QIcon icon(":/icons/static/img/img_not_found.svg");
    label_icon_->setPixmap(icon.pixmap(40,40));
    label_icon_->setProperty("id", 0);
    for (QLineEdit* item: allLineEdits_){
        item->setText("");
    }

}

void AccountDialog::getAllAccountsMasterdata_()
{
    // Abruf der Konten
    allAccountsMasterdata_ = account_->getAccounts();
    listWidget_accounts_->clear();
    // Schreiben der Konten in das listWidget Item
    for (int i: allAccountsMasterdata_.keys()){
        QListWidgetItem* item = new QListWidgetItem(listWidget_accounts_);
        item->setIcon(allAccountsMasterdata_[i].first); // Icon setzen
        item->setText(allAccountsMasterdata_[i].second); // Namen setzen
        item->setData(Qt::UserRole, i); // Key setzen
        listWidget_accounts_->addItem(item);
    }
    // Prüfung ob nichts geladen wurde
    if (listWidget_accounts_->count() == 0){
        this->setNewAccountDialog_();
    }
}

void AccountDialog::getSingleAccountMasterdata_(int id)
{

    // out["id"] / int
    // out["icon"] / int
    // out["name"] / str
    // out["account_holder"] / str
    // out["currency_id"] / str
    // out["account_number"] / str
    // out["iban"] / str
    // out["bank_code"] / str
    // out["bic"] / str
    // out["bank_name"] / str
    // out["opening_balance"] / double

    // Property setzen
    operation_type_ = "edit";
    groupBox_accountData_->setTitle("Stammdaten");
    singleAccountMasterdata_ = account_->getAccountDetails(id);
    account_id_ = singleAccountMasterdata_.value("id").toInt();

    // Labels und LineEdits füllen
    label_id_->setText(singleAccountMasterdata_.value("id").toString());
    QIcon icon = ptrStateMgr_->convertSVGImageFromDb(singleAccountMasterdata_.value("icon").toInt());
    label_icon_->setPixmap(icon.pixmap(40,40));
    label_icon_->setProperty("id", singleAccountMasterdata_.value("icon").toInt());
    lineEdit_name_->setText(singleAccountMasterdata_.value("name").toString());
    lineEdit_accountHolder_->setText(singleAccountMasterdata_.value("account_holder").toString());
    lineEdit_accountNumber_->setText(singleAccountMasterdata_.value("account_number").toString());
    lineEdit_iban_->setText(singleAccountMasterdata_.value("iban").toString());
    lineEdit_bankCode_->setText(singleAccountMasterdata_.value("bank_code").toString());
    lineEdit_bic_->setText(singleAccountMasterdata_.value("bic").toString());
    lineEdit_bankName_->setText(singleAccountMasterdata_.value("bank_name").toString());
    lineEdit_openingBalance_->setText(QString::number(singleAccountMasterdata_.value("opening_balance").toDouble()));

}

void AccountDialog::saveData()
{

    QMap<QString, QVariant> form_data;

    form_data["id"] = account_id_; // Prüfung
    form_data["icon"] = label_icon_->property("id").toInt(); // Prüfung
    form_data["name"] = lineEdit_name_->text(); // Prüfung
    form_data["account_holder"] = lineEdit_accountHolder_->text();
    form_data["currency_id"] = currency_id_;
    form_data["account_number"] = lineEdit_accountNumber_->text();
    form_data["iban"] = lineEdit_iban_->text();
    form_data["bank_code"] = lineEdit_bankCode_->text();
    form_data["bic"] = lineEdit_bic_->text();
    form_data["bank_name"] = lineEdit_bankName_->text();
    // Als String übernehmen und dann mit Regex auswerten ob ok
    form_data["opening_balance"]  = lineEdit_openingBalance_->text(); // Prüfung
    form_data["status_message"] = ""; // Antwort der nachfolgenden save Funktion

    bool resp = account_->saveAccount(operation_type_, form_data);
    if (!resp){
        QMessageBox::information(nullptr, "Hinweis", form_data["status_message"].toString());
        return;
    }
    operation_done_ = true;
    QMessageBox::information(nullptr, "Hinweis", "Datensatz wurde erfolgreich hinzugefügt");

    // hier muss jetzt noch das UI Update rein
    QIcon icon = ptrStateMgr_->convertSVGImageFromDb(form_data["icon"].toInt());
    QString name = form_data["name"].toString();
    if (operation_type_ == "new"){
        QListWidgetItem* item = new QListWidgetItem(listWidget_accounts_);
        item->setIcon(icon); // Icon setzen
        item->setText(name); // Namen setzen
        item->setData(Qt::UserRole, form_data["id"].toInt()); // Key setzen
        listWidget_accounts_->addItem(item);
        int index = listWidget_accounts_->count() - 1;
        listWidget_accounts_->setCurrentRow(-1);
        listWidget_accounts_->setCurrentRow(index); // connect auslösen
        return;
    }
    else if (operation_type_ == "edit"){
        int current_row = listWidget_accounts_->currentRow();
        this->getAllAccountsMasterdata_(); // Refresh all
        listWidget_accounts_->setCurrentRow(-1);
        listWidget_accounts_->setCurrentRow(current_row); // connect auslösen
    }
    operation_type_ = "edit"; // Auf edit setzen

}

void AccountDialog::deleteAccount()
{
    QListWidgetItem* item = this->listWidget_accounts_->currentItem();
    bool resp = false;
    if (!item){
        QMessageBox::information(nullptr, "Hinweis", "Bitte wähle zuerst den Eintrag aus, den du löschen möchtest");
        return;
    }
    int id = item->data(Qt::UserRole).toInt();
    if (!id) return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Löschen bestätigen");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Möchtest du dieses Konto wirklich löschen? Bitte beachte, dass alle Transaktionen, die diesem Konto zugeordnet sind ebenfalls gelöscht werden!");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int result = msgBox.exec();
    if (result == QMessageBox::Yes){
        QString filter = "id = " + QString::number(id);
        resp = ptrStateMgr_->db->del(constants::TABLE_ACCOUNTS, filter);
    }
    if (!resp){
        QMessageBox::information(nullptr, "Hinweis", "Konto konnte nicht gelöscht werden");
        return;
    }
    listWidget_accounts_->removeItemWidget(item);
    delete listWidget_accounts_->takeItem(listWidget_accounts_->row(item));
    operation_done_ = true;
    QMessageBox::information(nullptr, "Hinweis", "Das Konto wurde erfolgreich gelöscht");
    // Sind noch Einträge vorhanden? Dann den ersten Index im ListView setzen
    if (listWidget_accounts_->count() > 0){
        listWidget_accounts_->setCurrentRow(0);
    }
    // Sonst den Neuanlagedialog öffnen
    else {
        this->setNewAccountDialog_();
    }
}
IconPickerDialog *AccountDialog::generateIconWidget_()
{
    icon_picker_dialog_ = new IconPickerDialog(parent_, ptrStateMgr_, label_icon_);
    icon_picker_dialog_->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    icon_picker_dialog_->setWindowModality(Qt::WindowModal);
    icon_picker_dialog_->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    icon_picker_dialog_->setWindowTitle("Icons");
    return icon_picker_dialog_;
}
