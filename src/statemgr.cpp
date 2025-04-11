#include "../include/statemgr.h"
#include "../include/mainWindow.h"

core::StateMgr::StateMgr(QWidget* parent, db::Database* db):
    parent_(parent), db(db)
{
    // Basiseinstellung für State Parameter
    QDate date_to = QDate::currentDate(); // Aktuelles Datum
    QDate date_from = date_to.addDays(-constants::TIME_PERIOD_DAYS_DEFAULT); // - 90 Tage
    QString category_type = constants::CATEGORY_TYPE_DEFAULT; // "all"
    int category_id = constants::CATEGORY_ID_DEFAULT; // 0
    QString account_type = constants::ACCOUNT_TYPE_DEFAULT; // "all"
    int account_id = constants::ACCOUNT_ID_DEFAULT; // 0

    this->setTimePeriodParam(date_from, date_to);
    this->setCategoryTypeParam(category_type);
    this->setCategoryIdParam(category_id);
    this->setAccountTypeParam(account_type);
    this->setAccountIdParam(account_id);

    this->main_window = qobject_cast<MainWindow*>(parent);

    delete_status = false;

    connect(this, &core::StateMgr::refreshChart, this->main_window, &MainWindow::handleChartRefresh);
}

// Get Categories (Kategorien)
QSqlQuery core::StateMgr::getCategories() const
{
    QSqlQuery q;
    q = this->db->read(constants::TABLE_CATEGORIES);
    return q;
}

// Get SubCategories (Unterkategorien)
QSqlQuery core::StateMgr::getSubCategories() const
{
    QSqlQuery q;
    q = this->db->read(constants::TABLE_SUBCATEGORIES);
    return q;
}

// Get Accounts (Konten)
QSqlQuery core::StateMgr::getAccounts() const
{
    QSqlQuery q;
    q = this->db->read(constants::TABLE_ACCOUNTS);
    return q;
}

// Get Transactions
QSqlQuery core::StateMgr::getTransactions(bool item, int id) const
{
    // Falls nur ein spezieller Datensatz abgerufen werden soll (item, id)
    // Query wird nur für gesuchten Datensatz ausgeführt und gibt QSqlQuery zurück...
    if (item){
        QString param = "id = " + QString::number(id);
        QSqlQuery q = this->db->read(constants::TABLE_TRANSACTIONS, param);
        return q;
    }
    // ... Ansonsten aktuell gesetzter Zeitraum abrufen (ist immer gesetzt)
    QPair<QDate, QDate> dates = this->getTimePeriod();
    QDate date_from = dates.first;
    QDate date_to = dates.second;
    if (constants::DEBUG){
        qDebug() << "Properties StateMgr";
        qDebug() << "QSqlQuery core::StateMgr::getTransactions() const: Date-Objects: " << date_from << date_to;
        qDebug() << "QSqlQuery core::StateMgr::getTransactions() const: Account (type, id): " << this->account_type_ << this->account_id_;
        qDebug() << "QSqlQuery core::StateMgr::getTransactions() const: Category (type, id): " << this->category_type_ << this->category_id_;
    }

    // Danach umwandeln in string
    QString from = date_from.toString("yyyy-MM-dd") + " 00:00:00";
    QString to = date_to.toString("yyyy-MM-dd") + " 23:59:59";
    // Ersten Parameter setzen
    QString param = "transaction_date <= '" + to + "' AND transaction_date >= '" + from + "'";

    // Jetzt prüfen, ob ein Account in der Seitenleiste ausgewählt wurde
    int account_id = this->getAccountId();
    if (account_id) param += " AND account_id = " + QString::number(account_id);

    // und noch prüfen, ob eine Kategorie oder Unterkategorie in der Seitenleiste ausgewählt wurde
    int category_id = this->getCategoryId();
    QString category_type = this->getCategoryType();
    if (category_id && category_type == "main_cat") param += " AND category_group_id = " + QString::number(category_id);
    if (category_id && category_type == "sub_cat") param += " AND category_id = " + QString::number(category_id);

    // Daten mit Parametern abrufen
    QSqlQuery q = this->db->read(constants::TABLE_TRANSACTIONS, param);
    return q;
}

// Get Balance (Saldo)
QSqlQuery core::StateMgr::getBalance(bool all) const
{
    QSqlQuery q;
    QString param = "";
    // Prüfen, ob ein Account in Seitenleiste ausgewählt wurde
    // Kategorienauswahl und Zeitraum bleiben unberücksichtigt, das verwirrt nur bei der Saldoanzeige...!

    int account_id = this->getAccountId();
    if (account_id && !all) param += "account_id = " + QString::number(account_id);

    if (param.length() > 0){
        q = this->db->read(constants::TABLE_TRANSACTIONS, param);
    }
    else {
        q = this->db->read(constants::TABLE_TRANSACTIONS);
    }
    return q;
}

// Get Currency (Währung), gibt nur die Währung als String zurück
QString core::StateMgr::getCurrency() const
{
    QSqlQuery q = this->db->read(constants::TABLE_ROOT);
    if (q.first()){
        return QString(q.value("currency").toString());
    }
    return constants::CURRENCY_DEFAULT;
}

QSqlQuery core::StateMgr::getIcons() const
{
    QSqlQuery q = this->db->read(constants::TABLE_ICONS);
    return q;
}

QSqlQuery core::StateMgr::getRoot() const
{
    QSqlQuery q = this->db->read(constants::TABLE_ROOT);
    return q;
}

// Aktualisiert alle UI Elemente
bool core::StateMgr::setAll()
{
    try {

        // States zurücksetzen
        QDate date_to = QDate::currentDate(); // Aktuelles Datum
        QDate date_from = date_to.addDays(-constants::TIME_PERIOD_DAYS_DEFAULT); // - 90 Tage
        QString category_type = constants::CATEGORY_TYPE_DEFAULT; // "all"
        int category_id = constants::CATEGORY_ID_DEFAULT; // 0
        QString account_type = constants::ACCOUNT_TYPE_DEFAULT; // "all"
        int account_id = constants::ACCOUNT_ID_DEFAULT; // 0

        this->setTimePeriodParam(date_from, date_to);
        this->setCategoryTypeParam(category_type);
        this->setCategoryIdParam(category_id);
        this->setAccountTypeParam(account_type);
        this->setAccountIdParam(account_id);

        QSqlQuery q;
        // Accounts abrufen
        q = this->getAccounts();
        this->setAccounts(q);
        // Kategorien abrufen
        q = this->getCategories();
        this->setCategories(q);
        // Saldo abrufen
        q = this->getBalance();
        this->setBalance(q);
        // Zeiträume zum Filtern hinzufügen
        this->setTimePeriodFilter();
        // Transaktionen abrufen
        q = this->getTransactions();
        this->setTransactions(q);

        return true;
    }
    catch (std::exception &e){
        qDebug() << "core::StateMgr::setAll() Exception" << e.what();
        return false;
    }

}

// Set Categories (Kategorien), aktualisiert UI
bool core::StateMgr::setCategories(QSqlQuery &q)
{
    QTreeWidget* el = this->parent_->findChild<QTreeWidget*>("treeWidget_category");
    QSqlQuery sub_categories = this->   getSubCategories();

    if (!el) {
        return false;
    }

    el->clear();

    QTreeWidgetItem* main = new QTreeWidgetItem(el);
    main->setText(0, "Alle Kategorien");
    main->setExpanded(true);
    main->setData(0, Qt::UserRole, 0); // Id = 0 für select all
    main->setData(0, Qt::UserRole +1, "Alle Kategorien");
    main->setData(0, Qt::UserRole +2, "all");

    while (q.next()){
        QTreeWidgetItem* root = new QTreeWidgetItem(main);
        const int icon_index = q.value("icon").toInt();
        const int categories_index = q.value("id").toInt();
        QIcon icon = this->convertSVGImageFromDb(icon_index);

        root->setIcon(0, icon);
        root->setText(0, q.value("name").toString());

        root->setData(0, Qt::UserRole, categories_index);
        root->setData(0, Qt::UserRole + 1, q.value("name").toString());
        root->setData(0, Qt::UserRole + 2, "main_cat");

        sub_categories.seek(-1); // Reset für neue Iteration

        // Setzen der Unterkategorien
        while (sub_categories.next()){
            const int sub_categories_parent = sub_categories.value("parent_group_id").toInt();
            QString name = sub_categories.value("name").toString();

            if (categories_index == sub_categories_parent && !name.isEmpty()){
                // qDebug() << sub_categories_index << name;
                QTreeWidgetItem* item = new QTreeWidgetItem(root);
                item->setText(0, name);
                item->setData(0, Qt::UserRole, sub_categories.value("id").toInt());
                item->setData(0, Qt::UserRole + 1, name);
                item->setData(0, Qt::UserRole + 2, "sub_cat");
            }
        }
    }
    return true;
}

// Set Accounts (Konten), aktualisiert UI
bool core::StateMgr::setAccounts(QSqlQuery& q)
{
    QTreeWidget* el = this->parent_->findChild<QTreeWidget*>("treeWidget_account");

    if (!el){
        return false;
    }

    el->clear();
    QTreeWidgetItem* root = new QTreeWidgetItem(el);
    root->setText(0, "Alle Konten");
    int id = 0;
    QString type = "all";
    root->setData(0, Qt::UserRole, id);
    root->setData(0, Qt::UserRole +1, type);
    root->setData(0, Qt::UserRole +2, "Alle Konten");

    // Füge Konten aus dem Query hinzu
    while (q.next()) {
        QString name = q.value("name").toString();
        id = q.value("id").toInt();
        type = "item";
        const int icon_index = q.value("icon").toInt();
        QIcon icon = this->convertSVGImageFromDb(icon_index);
        if (!name.isEmpty()) {
            QTreeWidgetItem* item = new QTreeWidgetItem(root);
            item->setIcon(0,icon);
            item->setText(0, name);  // Setzt den Namen des Kontos
            item->setData(0, Qt::UserRole, id);
            item->setData(0, Qt::UserRole +1, type);
            item->setData(0, Qt::UserRole +2, name);
        }
    }
    el->expandAll();  // Alle Items erweitern

    return true;
}

// Set Transactions (Transaktionen), aktualisiert UI
bool core::StateMgr::setTransactions(QSqlQuery &q)
{
    QTableWidget* el = this->parent_->findChild<QTableWidget* >("table_transactions");
    if (!el){
        return false;
    }

    el->clearContents();
    el->setRowCount(0);
    el->setSortingEnabled(false);

    // Styling für die Buttons
    el->setStyleSheet(
        ".btn_edit_delete_records { "
        "background: "
        "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(241, 241, 241, 255), "
        "stop:1 rgba(255, 255, 255, 255)); "
        "border: 1px solid #E3E3E3;; border-radius: 3px; "
        "} "
        ".btn_edit_delete_records:hover { "
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(241, 241, 241, 255), "
        "stop:0.5 rgba(209, 224, 230, 255), stop:1 rgba(245, 250, 251, 255)); "
        "} "
        );

    // Foreign Key Lookups vorbereiten (schnelle Abfrage)
    QHash<int, QString> accounts;
    QHash<int, QString> categories;
    QHash<int, QString> subcategories;

    QSqlQuery q_accounts = this->getAccounts();
    while (q_accounts.next()) {
        accounts[q_accounts.value("id").toInt()] = q_accounts.value("name").toString();
    }

    QSqlQuery q_category_group = this->getCategories();
    while (q_category_group.next()) {
        categories[q_category_group.value("id").toInt()] = q_category_group.value("name").toString();
    }

    QSqlQuery q_sub_categories = this->getSubCategories();
    while (q_sub_categories.next()) {
        subcategories[q_sub_categories.value("id").toInt()] = q_sub_categories.value("name").toString();
    }

    QString currency = this->getCurrency();

    // Festlegen, wie viele Zeilen die Tabelle erhalten soll (bessere Performance als el->insertRow()
    int row = 0;
    int max_rows = 0;
    while (q.next()){
        max_rows++;
    }
    el->setRowCount(max_rows);
    // Query wieder auf Anfang setzen
    q.seek(-1);

    while (q.next()){
        int id = q.value("id").toInt();
        QWidget* actionButtonContainer = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionButtonContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        TableActionButton* btnEdit = new TableActionButton(actionButtonContainer,row, id, "btn_edit", "Transaktion bearbeiten");
        TableActionButton* btnDelete = new TableActionButton(actionButtonContainer,row, id, "btn_delete", "Transaktion löschen");
        layout->addWidget(btnEdit);
        layout->addWidget(btnDelete);
        el->setCellWidget(row, 0, actionButtonContainer);

        // connect(btnEdit, &QPushButton::clicked, this, [this, id, row]() {
        //     this->handleEditEvent(id, row);
        // });

        // connect(btnDelete, &QPushButton::clicked, this, [this, id, row]() {
        //     this->handleDeleteEvent(id, row);
        // });

        connect(btnEdit, &TableActionButton::buttonClicked, this, &StateMgr::handleEditEvent);
        connect(btnDelete, &TableActionButton::buttonClicked, this, &StateMgr::handleDeleteEvent);

        // Datum der Transaktion abrufen
        QString date_str = q.value("transaction_date").toString().left(constants::DATE_STRING_LENGTH_DEFAULT);
        QDateTime dateTime = QDateTime::fromString(date_str, "yyyy-MM-dd HH:mm:ss");
        QString formatted_date = dateTime.isValid() ? QLocale(QLocale::German).toString(dateTime.date(), "dd.MM.yyyy") : date_str;

        // Schnelle Lookup-Abfragen Hash
        int category_group_id = q.value("category_group_id").toInt();
        int category_id = q.value("category_id").toInt();
        int account_id = q.value("account_id").toInt();

        QString main_category = categories.value(category_group_id, constants::CATEGORY_DEFAULT);
        QString sub_category = subcategories.value(category_id, constants::CATEGORY_DEFAULT);
        QString account = accounts.value(account_id, constants::ACCOUNT_DEFAULT);

        double dbamount = q.value("amount").toDouble();
        QString balance = QLocale(QLocale::German).toString(dbamount, 'f', 2);

        el->setItem(row, 1, new QTableWidgetItem(formatted_date));
        el->setItem(row, 2, new QTableWidgetItem(q.value("recipient").toString()));
        el->setItem(row, 3, new QTableWidgetItem(q.value("purpose").toString()));
        el->setItem(row, 4, new QTableWidgetItem(main_category));
        el->setItem(row, 5, new QTableWidgetItem(sub_category));
        el->setItem(row, 6, new QTableWidgetItem(balance));
        el->setItem(row, 7, new QTableWidgetItem(currency));
        el->setItem(row, 8, new QTableWidgetItem(account));

        row++;
    }

    el->resizeColumnsToContents();
    el->resizeRowsToContents();
    emit refreshChart();
    return true;
}

// Set Balance (Saldo), aktualisiert UI
bool core::StateMgr::setBalance(QSqlQuery &q)
{
    QLabel* el = this->parent_->findChild<QLabel*>("lbl_total");

    if (!el){
        return false;
    }

    // Label zurücksetzen auf Default Wert
    el->setText("---");

    QSqlQuery accounts = this->getAccounts();
    QSqlQuery root = this->getRoot();

    double balance = constants::START_BALANCE_DEFAULT;
    double running_balance = 0;

    // Währung aus Datenbank abfragen
    QString currency = this->getCurrency();

    // Prüfen, ob aktuell ein Account in der Seitenleiste ausgewählt wurde
    int account_id = this->getAccountId();

    // Eröffnungssaldo aus Datenbank abfragen
    while (accounts.next()){
        running_balance = accounts.value("opening_balance").toDouble();
        // ist ein Item in der Seitenleiste ausgewählt
        // dann nur Opening Balance von diesem Konto abrufen
        if (account_id != 0 && accounts.value("id").toInt() == account_id){
            balance += running_balance;
            break;
        }
        else if (account_id == 0){
            balance += running_balance;
        }
    }

    // Positionen aus Transaktionen einlesen und addieren
    // hier sind die vorausgewählten Konten bereits gefiltert (q SqlQuery), sollten welche gesetzt worden sein
    while (q.next()){
        running_balance = q.value("amount").toDouble();
        balance += running_balance;
    }

    // LabelText setzen
    QLocale locale(QLocale::German);
    QString balance_label = locale.toString(balance, 'f', 2) + " " + currency;
    el->setText(balance_label);
    return true;
}

// Für QTableWidgetItem
bool core::StateMgr::setTableItem(QMap<QString, QString> &form_data)
{
    QTableWidget* el = this->parent_->findChild<QTableWidget* >("table_transactions");
    if (!el){
        return false;
    }
    el->setSortingEnabled(false);
    // Foreign Key Lookups vorbereiten (schnelle Abfrage)
    QHash<int, QString> accounts;
    QHash<int, QString> categories;
    QHash<int, QString> subcategories;

    QSqlQuery q_accounts = this->getAccounts();
    while (q_accounts.next()) {
        accounts[q_accounts.value("id").toInt()] = q_accounts.value("name").toString();
    }

    QSqlQuery q_category_group = this->getCategories();
    while (q_category_group.next()) {
        categories[q_category_group.value("id").toInt()] = q_category_group.value("name").toString();
    }

    QSqlQuery q_sub_categories = this->getSubCategories();
    while (q_sub_categories.next()) {
        subcategories[q_sub_categories.value("id").toInt()] = q_sub_categories.value("name").toString();
    }

    QString currency = this->getCurrency();
    // Zeile setzen, Element soll ganz oben neu platziert werden
    int row = form_data["row"].toInt();
    // Falls neue Zeile eingesetzt werden soll ist der Row Index == -1
    // Eine neue Zeile wird gesetzt und Row auf rowCount()-1 gesetzt (Element wird unten hinzugefügt)
    // Nicht oben hinzufügen, sonst müssen alle PushButtonReferenzen aktualisiert werden (zeitaufwendig...)
    int id = form_data.value("id").toInt();
    if (row == -1){
        row = el->rowCount();
        el->insertRow(row);

        QWidget* actionButtonContainer = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionButtonContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        TableActionButton* btnEdit = new TableActionButton(actionButtonContainer,row, id, "btn_edit", "Transaktion bearbeiten");
        TableActionButton* btnDelete = new TableActionButton(actionButtonContainer,row, id, "btn_delete", "Transaktion löschen");
        layout->addWidget(btnEdit);
        layout->addWidget(btnDelete);
        el->setCellWidget(row, 0, actionButtonContainer);

        // connect(btnEdit, &QPushButton::clicked, this, [this, id, row]() {
        //     this->handleEditEvent(id, row);
        // });

        // connect(btnDelete, &QPushButton::clicked, this, [this, id, row]() {
        //     this->handleDeleteEvent(id, row);
        // });

        connect(btnEdit, &TableActionButton::buttonClicked, this, &StateMgr::handleEditEvent);
        connect(btnDelete, &TableActionButton::buttonClicked, this, &StateMgr::handleDeleteEvent);
    }

    // Datum der Transaktion abrufen
    QString date_str = form_data.value("transaction_date").left(constants::DATE_STRING_LENGTH_DEFAULT);
    QDateTime dateTime = QDateTime::fromString(date_str, "yyyy-MM-dd HH:mm:ss");
    QString formatted_date = dateTime.isValid() ? QLocale(QLocale::German).toString(dateTime.date(), "dd.MM.yyyy") : date_str;

    // Schnelle Lookup-Abfragen Hash
    int category_group_id = form_data.value("category_group_id").toInt();
    int category_id = form_data.value("category_id").toInt();
    int account_id = form_data.value("account_id").toInt();

    QString main_category = categories.value(category_group_id, constants::CATEGORY_DEFAULT);
    QString sub_category = subcategories.value(category_id, constants::CATEGORY_DEFAULT);
    QString account = accounts.value(account_id, constants::ACCOUNT_DEFAULT);

    double dbamount = form_data.value("amount").toDouble();
    QString balance = QLocale(QLocale::German).toString(dbamount, 'f', 2);

    el->setItem(row, 1, new QTableWidgetItem(formatted_date));
    el->setItem(row, 2, new QTableWidgetItem(form_data.value("recipient")));
    el->setItem(row, 3, new QTableWidgetItem(form_data.value("purpose")));
    el->setItem(row, 4, new QTableWidgetItem(main_category));
    el->setItem(row, 5, new QTableWidgetItem(sub_category));
    el->setItem(row, 6, new QTableWidgetItem(balance));
    el->setItem(row, 7, new QTableWidgetItem(currency));
    el->setItem(row, 8, new QTableWidgetItem(account));

    el->selectRow(row);
    el->setFocus();

    QSqlQuery q = this->getBalance();
    this->setBalance(q);
    this->main_window->logger.print_status(this->main_window->lbl_status, "SUCCESS", "Änderungen gespeichert (Transaktion)");

    return true;
}

// Füllt die ComboBox zur Auswahl des Transaktionszeitraums mit Selektionsmöglichkeiten
bool core::StateMgr::setTimePeriodFilter()
{
    QComboBox* cbx_time_period = this->parent_->findChild<QComboBox*>("comboBox_time_period");
    cbx_time_period->clear();

    QDate date = QDate::currentDate();
    int current_year = date.year();

    if (!cbx_time_period){
        return false;
    }

    QStringList data = {
        "Alle anzeigen",
        "letzte 30 Tage",
        "letzte 60 Tage",
        "letzte 90 Tage",
        "letzte 365 Tage",
        "Jahr " + QString::number(current_year),
        "Jahr " + QString::number(current_year -1),
        "Jahr " + QString::number(current_year -2),
        "Jahr " + QString::number(current_year -3)
    };

    QStringList type = {
        "all",
        "days",
        "days",
        "days",
        "days",
        "year",
        "year",
        "year",
        "year",
    };

    QStringList filter = {
        "*",
        "30",
        "60",
        "90",
        "365",
        QString::number(current_year),
        QString::number(current_year -1),
        QString::number(current_year -2),
        QString::number(current_year -3)
    };

    cbx_time_period->addItems(data);
    cbx_time_period->setCurrentIndex(3);

    for (int i = 0; i < data.length(); i++){
        cbx_time_period->setItemData(i, type[i], Qt::UserRole);
        cbx_time_period->setItemData(i, filter[i], Qt::UserRole+1);
    }

    return true;
}

bool core::StateMgr::handleEditEvent(int id, int row)
{
    QWidget* mainWd = qobject_cast<QWidget*>(this->parent_);
    TransactionDialog *w = new TransactionDialog(mainWd, id, row, this);

    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Transaktion bearbeiten");
    w->show();
    return true;
}

void core::StateMgr::confirmDeleteEvent(bool status)
{
    qDebug() << "StateMgr Delete status" << status;
    this->delete_status = status;
}

// Löscht einen Datensatz aus der Tabelle und aus der Datenbank
bool core::StateMgr::handleDeleteEvent(int id, int row)
{
    bool resp = QMessageBox::question(nullptr, "Löschen",
                          "Bist du dir sicher, dass du diesen Datensatz löschen möchtest?",
                          "Nein", "Ja");
    if (!resp) return false;

    QString filter = "id = " + QString::number(id);
    bool success = this->db->del(constants::TABLE_TRANSACTIONS, filter);
    // Abbruch falls Fehler in Datenbankabfrage
    if (!success) {
        QMessageBox::information(nullptr, "Hinweis", "Konnte Datensatz nicht löschen. Bitte versuche es später noch einmal.");
        return false;
    }
    QTableWidget* table = parent_->findChild<QTableWidget* >("table_transactions");

    table->removeRow(row);

    // Indexe der Action Buttons müssen angepasst werden,
    // da eine Zeile gelöscht wurde und sich dadurch die Rows verschieben

    for (int i = 0; i < table->rowCount(); i++) {
        QWidget* buttonContainer = table->cellWidget(i,0);
        TableActionButton* btnEdit = buttonContainer->findChild<TableActionButton*>("btn_edit");
        TableActionButton* btnDelete = buttonContainer->findChild<TableActionButton*>("btn_delete");

        if (!btnEdit || !btnDelete){
            qDebug() << "Could not find Child btn_edit and/or btn_delete on StateMgr::handleDeleteEvent";
            // Rufe refresh all auf um gesamte Tabelle zu aktualisieren
            this->setAll();
            return false;
        }

        btnEdit->setRow(i);
        btnDelete->setRow(i);

        // const int id_btnEdit = btnEdit->getId();
        // const int id_btnDelete = btnDelete->getId();

        // disconnect(btnEdit, &QPushButton::clicked, nullptr, nullptr);
        // disconnect(btnDelete, &QPushButton::clicked, nullptr, nullptr);

        // connect(btnEdit, &QPushButton::clicked, this, [this, id_btnEdit, i]() {
        //     this->handleEditEvent(id_btnEdit, i);
        // });

        // connect(btnDelete, &QPushButton::clicked, this, [this, id_btnDelete, i]() {
        //     this->handleDeleteEvent(id_btnDelete, i);
        // });
        // connect(btnEdit, &TableActionButton::buttonClicked, this, &StateMgr::handleEditEvent);
        // connect(btnDelete, &TableActionButton::buttonClicked, this, &StateMgr::handleDeleteEvent);


    }

    // Balance aktualisieren
    QSqlQuery q = this->getBalance();
    this->setBalance(q);
    this->main_window->logger.print_status(this->main_window->lbl_status, "WARNING","Transaktion gelöscht");
    QMessageBox::information(nullptr, "Hinweis", "Datensatz wurde gelöscht.");
    return true;
}

// Tool, erstellt Icon aus in Datenbank hinterlegtem SVG
QIcon core::StateMgr::convertSVGImageFromDb(const int& icon_index) const
{
    try {
        // Zuerst Datenbankabfrage für Icon aus Tabelle Icons
        QString filter = "id = " + QString::number(icon_index);
        QSqlQuery q = this->db->read(constants::TABLE_ICONS, filter);
        QByteArray svg;

        // Default Parameter ergänzen falls keine Datensätze vorhanden
        if (!q.first()){
            // Default Icon abrufen
            filter = "id = " + QString::number(constants::ICON_DEFAULT);
            q = this->db->read(constants::TABLE_ICONS, filter);

            // Default setzen
            if (q.first()){
                qDebug() << "Default Icon aufrufen";
                svg = q.value("icon").toByteArray();
                svg.replace("\"\"", "\"");
            }
            else {
                throw std::runtime_error("Could not fetch icon from database" + q.lastError().text().toStdString());
            }
        }
        else {
            svg = q.value("icon").toByteArray();
            svg.replace("\"\"", "\"");
        }

        QSvgRenderer renderer;
        renderer.load(svg);
        QPixmap pixmap(renderer.defaultSize());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        renderer.render(&painter);

        return QIcon(pixmap);  // Rückgabe des Icons
    }
    // Exception falls Icons nicht geladen werden können
    catch (std::runtime_error &e){
        qDebug() << "Runtime Error" << e.what();
        QIcon fallback_icon = QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout);
        return fallback_icon;
    }

}

// ********** StateMgmt Setter und Getter **********
// ********** Setter **********
bool core::StateMgr::setTimePeriodParam(QDate &date_from, QDate &date_to)
{
    if (date_from <= date_to){
        this->days_from_ = date_from;
        this->days_to_ = date_to;
        return true;
    }
    return false;
}

bool core::StateMgr::setCategoryTypeParam(QString &category_type)
{
    if (category_type.length() > 0){
        this->category_type_ = category_type;
        return true;
    }
    return false;
}

bool core::StateMgr::setCategoryIdParam(int &id)
{
    this->category_id_ = id;
    return true;
}

bool core::StateMgr::setAccountTypeParam(QString &account_type)
{
    if (account_type.length() > 0){
        this->account_type_ = account_type;
        return true;
    }
    return false;
}

bool core::StateMgr::setAccountIdParam(int &id)
{
    this->account_id_ = id;
    return true;
}

// ********** Getter **********
QPair<QDate, QDate> core::StateMgr::getTimePeriod() const
{
    return QPair<QDate, QDate>(this->days_from_, this->days_to_);
}

QString core::StateMgr::getCategoryType() const
{
    return this->category_type_;
}

int core::StateMgr::getCategoryId() const
{
    return this->category_id_;
}

QString core::StateMgr::getAccountType() const
{
    return this->account_type_;
}

int core::StateMgr::getAccountId() const
{
    return this->account_id_;
}

int core::StateMgr::getRootId() const
{
    QSqlQuery q = this->db->read(constants::TABLE_ROOT);
    q.first();
    int id = q.value("id").toInt();
    return id;
}


// Action Button füt TableViewItem
core::TableActionButton::TableActionButton(QWidget *parent, int row, int id, QString object_name, QString toolTip)
{
    // Objekteigenschaften festlegen
    this->setObjectName(object_name);
    if (object_name == "btn_edit"){
        this->setText("🖊️");
    }
    else if (object_name == "btn_delete"){
        this->setText("🗑");
    }
    else {
        qDebug() << "Wrong object name in class core::TableAcionButton. Choose from btn_edit or btn_delete";
        this->setText("❌");
    }

    this->setProperty("class", "btn_edit_delete_records");
    this->setCursor(Qt::PointingHandCursor);
    this->setWhatsThis(toolTip);
    this->setToolTip(toolTip);

    // Row und Id festlegen
    this->setId(id);
    this->setRow(row);

    connect(this, &QPushButton::clicked, this, &TableActionButton::emitButtonClicked);
}

// Row setzen
void core::TableActionButton::setRow(int row)
{
    this->row_ = row;
}
// Id setzen
void core::TableActionButton::setId(int id)
{
    this->id_ = id;
}
// Getter für Row
int core::TableActionButton::getRow() const
{
    return this->row_;
}
// Getter für Id
int core::TableActionButton::getId() const
{
    return this->id_;
}
// Getter für ObjectName
QString core::TableActionButton::getObjectName() const
{
    return this->objectName();
}

void core::TableActionButton::emitButtonClicked()
{
    emit buttonClicked(this->id_, this->row_);
}
