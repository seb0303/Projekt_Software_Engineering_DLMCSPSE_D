#include "../include/transactionDialog.h"
#include "../ui/ui_transactionDialog.h"
#include "../include/statemgr.h"

TransactionDialog::TransactionDialog(QWidget *parent, const int id, const int row, core::StateMgr *ptrStateMgr)
    : QWidget(parent)
    , ui(new Ui::TransactionDialog)
    , ptrStateMgr_(ptrStateMgr)
{
    ui->setupUi(this);

    // Prüfen ob StateMgr gesetzt wurde
    try {
        if (ptrStateMgr_) {
            ptrTransaction = new Transaction(this, ptrStateMgr_);
        }
        else {
            throw std::runtime_error("Laufzeitfehler. Konnte Transaktionsdialog nicht initialisieren");
        }

        // Header setzen
        lbl_header = this->ui->lbl_header;
        lbl_header->setText("Transaktion hinzufügen");
        lbl_id = this->ui->lbl_id;
        lbl_id->setText("neu");
        lbl_type = this->ui->lbl_type;
        lbl_type->setText("NaN");

        lineEdit_amount = this->ui->lineEdit_amount;
        lineEdit_amount->setMaxLength(12);

        // Connect der Amount LineEdit um Typ zu prüfen (Einnahmen, Ausgaben, None)
        connect(lineEdit_amount, &QLineEdit::textChanged, this, [this](){
            QString sign = lineEdit_amount->text().left(1);
            if (sign == "-"){
                lbl_type->setText("Ausgaben");
                return;
            }
            bool isNumber;
            sign.toDouble(&isNumber);
            if (isNumber){
                lbl_type->setText("Einnahmen");
                return;
            }
            lbl_type->setText("NaN");

        });
        // Datenbankwährung setzen
        lbl_currency = this->ui->lbl_currency;
        currency = ptrTransaction->getCurrency();
        lbl_currency->setText(currency.second);

        // Button für Submit
        btn_submit = this->ui->pushButton_submit;
        connect(btn_submit, &QPushButton::clicked, this, [this](){
            bool resp = this->handleSubmitEvent_();
            if (resp){
                this->close();
                this->deleteLater();
            }
        });

        //Button für Cancel
        btn_cancel = this->ui->pushButton_cancel;
        connect(btn_cancel, &QPushButton::clicked, this, [this](){
            this->close();
            this->deleteLater();
        });
        // ComboBoxen definieren
        comboBox_accounts = this->ui->comboBox_account;
        comboBox_categories = this->ui->comboBox_category;
        comboBox_sub_categories = this->ui->comboBox_sub_category;

        // Datensätze für Kategorien laden
        categories = ptrTransaction->getCategories();
        sub_categories = ptrTransaction->getSubCategories();

        // Datensätze für Accounts laden
        accounts = ptrTransaction->getAccounts();

        // ComboBox befüllen
        for (int i: accounts.keys()){
            comboBox_accounts->addItem(accounts[i]);
            comboBox_accounts->setItemData(comboBox_accounts->count() - 1, i, Qt::UserRole);
        }
        for (int i: categories.keys()){
            QPair<QString, QString> el = categories[i];
            comboBox_categories->addItem(el.second, i);
            comboBox_categories->setItemData(comboBox_categories->count() - 1, i, Qt::UserRole);
        }
        connect(comboBox_categories, &QComboBox::currentIndexChanged, this, [this](){
            this->handleCategoryCbxChangeEvent_();
        });
        // Ersten Index der Kategorie-ComboBox setzen um Event Handler auszulösen
        comboBox_categories->setCurrentIndex(0);
        this->handleCategoryCbxChangeEvent_();

        // Formulareingabefelder initialisieren
        textEdit_purpose = this->ui->textEdit_purpose;
        lineEdit_recipient = this->ui->lineEdit_recipient;
        dateEdit_date = this->ui->dateEdit_date;
        QDate current_date = QDate::currentDate();
        dateEdit_date->setDate(current_date);

        // Zeile der Tabelle die bearbeitet wird (falls gesetzt, default 0)
        table_row = row;

        // Falls id gesetzt wurde: Bearbeitungsdialog (nicht Neuanlagedialog) anzeigen und zu
        // bearbeitenden Datensatz aus der Datenbank laden
        if (id) {
            QSqlQuery q = ptrTransaction->getTransaction(id);
            if (q.first()){
                lbl_header->setText("Transaktion bearbeiten");
                lbl_id->setText(q.value("id").toString());

                double db_amount = q.value("amount").toDouble();
                QString balance = QLocale(QLocale::German).toString(db_amount, 'f', 2);

                lineEdit_amount->setText(balance);
                textEdit_purpose->setText(q.value("purpose").toString());
                lineEdit_recipient->setText(q.value("recipient").toString());
                dateEdit_date->setDate(q.value("transaction_date").toDate());

                const int category_index = q.value("category_group_id").toInt();
                const int sub_category_index = q.value("category_id").toInt();
                const int account_index = q.value("account_id").toInt();

                for (int i = 0; i < comboBox_categories->count(); i++){
                    const int el_id = comboBox_categories->itemData(i, Qt::UserRole).toInt();
                    if (el_id == category_index){
                        comboBox_categories->setCurrentIndex(i);
                        break;
                    }
                }

                for (int i = 0; i < comboBox_sub_categories->count(); i++){
                    const int el_id = comboBox_sub_categories->itemData(i, Qt::UserRole).toInt();
                    if (el_id == sub_category_index){
                        comboBox_sub_categories->setCurrentIndex(i);
                        break;
                    }
                }

                for (int i = 0; i < comboBox_accounts->count(); i++){
                    const int el_id = comboBox_accounts->itemData(i, Qt::UserRole).toInt();
                    if (el_id == account_index){
                        comboBox_accounts->setCurrentIndex(i);
                        break;
                    }
                }
            }
            else {
                throw std::runtime_error("Datensatz konnte nicht aus Datenbank geladen werden.");
            }
        }
    }
    catch(std::runtime_error &e){
        QMessageBox::critical(nullptr, "Error", e.what());
        qDebug() << e.what();
        this->close();
        this->deleteLater();
    }

}

TransactionDialog::~TransactionDialog()
{
    delete ui;
}

void TransactionDialog::handleCategoryCbxChangeEvent_()
{
    const int cbx_index = this->comboBox_categories->currentIndex();
    const int category_index = this->comboBox_categories->itemData(cbx_index, Qt::UserRole).toInt();
    // Sub Categories leeren
    comboBox_sub_categories->clear();
    // Typ Label setzen (Einnahmen / Ausgaben / none)
    // QString type = categories[category_index].first;
    // Übersetzen, hier eventuell noch Qt translate tool (tr) einbauen...
    // if (type != "none") {
    //     (type == "expense") ? type = "Ausgaben" : type = "Einnahmen";
    // }
    // lbl_type->setText(type);
    // Iteration über Sub-Categories:
    for (int i: sub_categories.keys()){
        // Erster Index Parent Element (Hauptkategorie)
        // Zweiter Index Bezeichnung der Unterkategorie
        QPair<int, QString> el = sub_categories[i];
        // Prüfen, ob Index des Parent Elements mit Index in Sub-Kategorie übereinstimmt (Referenz)
        // Falls ja, den Datensatz in die Subkategorie laden
        if (el.first == category_index){
            comboBox_sub_categories->addItem(el.second);
            comboBox_sub_categories->setItemData(comboBox_sub_categories->count() - 1, i, Qt::UserRole);
        }
    }
}

bool TransactionDialog::handleSubmitEvent_()
{
    // Aktuell gesetzte Indexe zwischenspeichern
    const int account_index = comboBox_accounts->currentIndex();
    const int category_index = comboBox_categories->currentIndex();
    const int sub_category_index = comboBox_sub_categories->currentIndex();

    // Alle Daten aus dem Formular in form_data übertragen
    // qDebug() << lbl_id->text();
    form_data["id"] = lbl_id->text();
    // qDebug() << comboBox_accounts->itemData(account_index, Qt::UserRole).toString();
    form_data["account_id"] = comboBox_accounts->itemData(account_index, Qt::UserRole).toString();
    // qDebug() << dateEdit_date->text();
    form_data["transaction_date"] = dateEdit_date->text();
    // qDebug() << lineEdit_amount->text();
    form_data["amount"] = lineEdit_amount->text();
    // qDebug() << lineEdit_recipient->text();
    form_data["recipient"] = lineEdit_recipient->text();
    // qDebug() << textEdit_purpose->toPlainText().left(100);
    form_data["purpose"] = textEdit_purpose->toPlainText().left(100);
    // qDebug() << comboBox_categories->itemData(category_index, Qt::UserRole).toString();
    form_data["category_group_id"] = comboBox_categories->itemData(category_index, Qt::UserRole).toString();
    // qDebug() << comboBox_sub_categories->itemData(sub_category_index, Qt::UserRole).toString();
    form_data["category_id"] = comboBox_sub_categories->itemData(sub_category_index, Qt::UserRole).toString();
    // qDebug() << QString::number(constants::ACCOUNT_ID_DEFAULT);
    form_data["currency_id"] = QString::number(constants::ACCOUNT_ID_DEFAULT + 1);
    // Row noch einfügen, falls eine gesetzt wurde
    form_data["row"] = QString::number(table_row);

    if (constants::DEBUG){
        qDebug () << "Übermittelte Datensätze: ";
        for (const QString &key : form_data.keys()) {
            QString z = form_data[key];
            qDebug() << key << ":" << z;
        }
    }

    QPair<bool, QString> resp = ptrTransaction->save_data(form_data);
    if (!resp.first){
        QMessageBox::information(nullptr, "Hinweis", resp.second);
        return false;
    }

    QMessageBox::information(nullptr, "Hinweis", "Datensatz wurde gespeichert.");

    return true;
}
