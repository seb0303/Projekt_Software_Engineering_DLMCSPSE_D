#include "../include/database.h"

db::Database::Database(QString path,
                       QString database_name,
                       bool passwordRequired,
                       QString password,
                       QString connection_name,
                       QObject *parent)
{

    // Laden der Datenbank mit Demo Datensätzen, Parameter bei Objektinstanziierung werden überschrieben!
    // if (constants::DEBUG){
    //     this->database_name_ = constants::DB_DEMO_NAME;
    //     this->path_ = constants::DB_PATH_DEMO;
    // }

    this->database_name_ = database_name;
    this->path_ = path;

    this->driver_ = constants::DB_DRIVER;
    this->connection_name_ = connection_name;
    this->password_ = password;
    this->password_req_ = passwordRequired;

    this->db_ = QSqlDatabase::addDatabase(this->driver_, this->connection_name_);
    this->full_path_ = QDir(this->path_).filePath(this->database_name_);

    this->db_.setDatabaseName(this->full_path_);

    this->validateDatabase_();
}

// Prüft Verbindung zur SQLite Datenbank.
// Gibt true zurück, wenn die Datenbank geöffnet werden konnte.
bool db::Database::connect()
{
    if (!this->db_.open()) {
        if (constants::DEBUG){
            qDebug() << "Error while connecting to database" << this->db_.lastError().text();
        }
        return false;
    }
    else {
        QString fullPath = QDir(this->path_).filePath(this->database_name_);
        qInfo() << "Db Connection established" << "Source:" << this->full_path_;
        return true;
    }
}

// Datenbank Connection schließen
// Objekt kann nicht mehr verwendet werden bevor nicht Connect ausgelöst wird
bool db::Database::close()
{
    if (this->db_.isOpen()){
        this->db_.close();
        if (constants::DEBUG){
            qDebug() << "Connection closed";
        }
        return true;
    }
    return false;
}

QString db::Database::getFullPath()
{
    return this->full_path_;
}

// Liest die Datensätze einer als Parameter anzugebenden Tabelle
// 13.02 Sollte noch hinzugefügt werden: Queryklausel WHERE, etc...
QSqlQuery db::Database::read(QString table, QString filter)
{
    QString q;

    if (filter == ""){
        q = "SELECT * FROM " + table + ";";
    }
    else {
        q = "SELECT * FROM " + table + " WHERE " + filter + ";";
    }

    QSqlQuery query(this->db_);

    if (!query.exec(q)) {
        // Läuft durch bis main, exit Anweisung
        throw std::invalid_argument("Something went wrong while fetching data from the database");
    }

    return query;
}

bool db::Database::update_or_create_transaction(QMap<QString, QString>& form_data, QString type)
{

    QSqlQuery query(this->db_);
    QString table = constants::TABLE_TRANSACTIONS;
    // Transaktionsdatum in Timestamp Format bringen
    form_data["transaction_date"] = form_data["transaction_date"] + " 00:00:00";
    form_data["amount"] = form_data["amount"].replace(",",".");

    if (type == "update"){
        query.prepare("UPDATE " + table + " SET "
                                          "transaction_date = :date, "
                                          "amount = :amount, "
                                          "recipient = :recipient, "
                                          "purpose = :purpose, "
                                          "category_id = :category_id, "
                                          "category_group_id = :category_group_id, "
                                          "account_id = :account_id, "
                                          "currency_id = :currency_id "
                                          "WHERE id = :id;");
    }
    else if (type == "insert"){
        QString sql_statement =
        "INSERT INTO " + table +R"(
        (transaction_date, amount, recipient, purpose, category_id, category_group_id, account_id, currency_id)
        VALUES (:date, :amount, :recipient, :purpose, :category_id, :category_group_id, :account_id, :currency_id)
        )";
        query.prepare(sql_statement);
    }

    else {
        qDebug() << "Wrong parameter. Options: 'update' or 'create'";
        return false;
    }


    query.bindValue(":date", form_data["transaction_date"]);
    query.bindValue(":amount", form_data["amount"].toDouble());
    query.bindValue(":recipient", form_data["recipient"]);
    query.bindValue(":purpose", form_data["purpose"]);
    query.bindValue(":category_id", form_data["category_id"].toInt());
    query.bindValue(":category_group_id", form_data["category_group_id"].toInt());
    query.bindValue(":account_id", form_data["account_id"].toInt());
    query.bindValue(":currency_id", form_data["currency_id"].toInt());

    // Bei Update Statement noch die id mit einbinden
    if (type == "update") query.bindValue(":id", form_data["id"].toInt());

    qDebug() << form_data;

    if (!query.exec()) {
        qDebug() << "SQL-Fehler:" << query.lastError().text();
        throw std::invalid_argument("Something went wrong while updating data at the database");
        return false;
    }

    // Noch die Id im form_data aktualisieren wenn alles geklappt hat
    if (type == "insert") {
        int newId = query.lastInsertId().toInt();
        form_data["id"] = QString::number(newId);
    }
    return true;
}

bool db::Database::update_or_create_category(int id, int icon, QString name)
{
    QSqlQuery query(this->db_);
    QString table = constants::TABLE_CATEGORIES;

    if (id == 0) { // Create
        QString sql_statement =
        "INSERT INTO " + table +R"(
        (icon, name, type)
        VALUES (:icon, :name, :type)
        )";
        query.prepare(sql_statement);

        query.bindValue(":icon", icon);
        query.bindValue(":name", name);
        query.bindValue(":type", "none"); // Alles mit none hinzufügen? Vllt noch prüfen...
    }
    else { // Update
        query.prepare("UPDATE " + table + " SET "
                                          "icon = :icon, "
                                          "name = :name, "
                                          "type = :type "
                                          "WHERE id = :id;");
        query.bindValue(":icon", icon);
        query.bindValue(":name", name);
        query.bindValue(":type", "none"); // Alles mit none hinzufügen? Vllt noch prüfen...
        query.bindValue(":id", id);
    }

    if (!query.exec()) {
        qDebug() << "SQL-Fehler:" << query.lastError().text();
        throw std::invalid_argument("Something went wrong while updating data at the database");
        return false;
    }
    return true;

}

bool db::Database::update_or_create_subCategory(int id, int parent_id, QString name)
{
    QSqlQuery query(this->db_);
    QString table = constants::TABLE_SUBCATEGORIES;

    if (id == 0) { // Create
        QString sql_statement =
            "INSERT INTO " + table +R"(
        (name, parent_group_id)
        VALUES (:name, :parent_group_id)
        )";
        query.prepare(sql_statement);

        query.bindValue(":name", name);
        query.bindValue(":parent_group_id", parent_id);
    }
    else { // Update
        query.prepare("UPDATE " + table + " SET "
                                          "name = :name, "
                                          "parent_group_id = :parent_group_id "
                                          "WHERE id = :id;");
        query.bindValue(":name", name);
        query.bindValue(":parent_group_id", parent_id);
        query.bindValue(":id", id);
    }

    if (!query.exec()) {
        qDebug() << "SQL-Fehler:" << query.lastError().text();
        throw std::invalid_argument("Something went wrong while updating data at the database");
        return false;
    }
    return true;
}

bool db::Database::update_or_create_account(QMap<QString, QVariant> &form_data)
{
    QSqlQuery query(this->db_);
    QString table = constants::TABLE_ACCOUNTS;
    int id = form_data["id"].toInt();

    if (id == 0) { // Create
        QString sql_statement =
            "INSERT INTO " + table +R"(
        (icon, name, account_holder, currency_id, account_number, iban, bank_code, bic, bank_name, opening_balance)
        VALUES (:icon, :name, :account_holder, :currency_id, :account_number, :iban, :bank_code, :bic, :bank_name, :opening_balance);
        )";
        query.prepare(sql_statement);

    }
    else { // Update
        query.prepare("UPDATE " + table + " SET "
                                          "icon = :icon, "
                                          "name = :name, "
                                          "account_holder = :account_holder, "
                                          "currency_id = :currency_id, "
                                          "account_number = :account_number, "
                                          "iban = :iban, "
                                          "bank_code = :bank_code, "
                                          "bic = :bic, "
                                          "bank_name = :bank_name, "
                                          "opening_balance = :opening_balance "
                                          "WHERE id = :id;");
    }
    // Id hinzufügen falls Edit Dialog
    if (id != 0) query.bindValue(":id", id);
    query.bindValue(":icon", form_data["icon"].toInt());
    query.bindValue(":name", form_data["name"].toString());
    query.bindValue(":account_holder", form_data["account_holder"].toString());
    query.bindValue(":currency_id", form_data["currency_id"].toInt());
    query.bindValue(":account_number", form_data["account_number"].toString());
    query.bindValue(":iban", form_data["iban"].toString());
    query.bindValue(":bank_code", form_data["bank_code"].toString());
    query.bindValue(":bic", form_data["bic"].toString());
    query.bindValue(":bank_name", form_data["bank_name"].toString());
    query.bindValue(":opening_balance", form_data["opening_balance"].toDouble());

    if (!query.exec()) {
        qDebug() << "SQL-Fehler:" << query.lastError().text();
        throw std::invalid_argument("Something went wrong while updating data at the database");
        return false;
    }
    form_data["status_message"] = "Datensatz gesichert";
    if (id == 0) {
        // Noch die neue ID hinzufügen
        form_data["id"] = query.lastInsertId().toInt();
    }
    return true;
}

bool db::Database::update_root(QMap<QString, QVariant> &form_data)
{
    QSqlQuery query(this->db_);
    QString table = constants::TABLE_ROOT;
    int id = form_data["id"].toInt();

    query.prepare("UPDATE " + table + " SET "
                                      "database_name = :database_name, "
                                      "currency = :currency "
                                      "WHERE id = :id;");
    query.bindValue(":database_name", form_data["database_name"].toString());
    query.bindValue(":currency", form_data["currency"].toString());
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SQL-Fehler:" << query.lastError().text();
        throw std::invalid_argument("Something went wrong while updating data at the database");
        return false;
    }
    return true;
}

// Löscht einen Record aus der Datenbank der angegebenen Tabelle (Filter erforderlich)
bool db::Database::del(QString table, QString filter)
{
    QString q;

    if (constants::DEBUG){
        qDebug() << "Delete Method called, submitted values: " << table << filter;
    }

    // Query vorbereiten
    q = "DELETE FROM " + table + " WHERE " + filter + ";";

    QSqlQuery query(this->db_);
    query.exec("PRAGMA foreign_keys = ON;");
    if (!query.exec(q)) {
        // Läuft durch bis main, exit Anweisung
        throw std::invalid_argument("Something went wrong while deleting data from the database");
        return false;
    }
    return true;
}

// Erstellt eine neue Datenbank
bool db::Database::createDb(const QMap<QString, QString> form_data)
{
    if (constants::DEBUG){
        qDebug() << "Submitted values:";
        for (auto [key, value]: form_data.asKeyValueRange()){
            qDebug() << key << value;
        }
    }

    // Neue Datenbank initialisieren
    QSqlDatabase db = QSqlDatabase::addDatabase(constants::DB_DRIVER);
    QString db_name = form_data.value("db_name").replace(" ","");

    // path = ./db/resources/DBNAME.pwdb
    QString db_path = constants::ROOT + "/" + constants::DB_PATH + "/" + db_name + constants::DB_FILE_EXT;
    QString ddl_path = constants::ROOT + "/" + constants::DB_CONFIG;

    // Initialisierung
    db.setDatabaseName(db_path);

    // Prüfen, ob die Datenbank geöffnet werden kann. Falls nicht Funktion beenden.
    if (!db.open()) {
        throw std::invalid_argument("Could not create Database.");
        return false;
    }

    // DDL File setzen
    QFile ddl_file(ddl_path);

    // Prüfen, ob DDL File geöffnet werden kann. Falls nicht Funktion beenden.
    if (!ddl_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("Could not parse DDL File.");
        return false;
    }

    // Daten aus DDL File einlesen und in TextStream speichern
    QTextStream config(&ddl_file);
    QString sql = config.readAll();
    ddl_file.close();

    // Zuletzt noch die einzelnen Tabellen die erstellt werden sollen in einer Liste aufteilen
    // So kann der Query Tabelle für Tabelle ausgeführt werden
    QStringList sql_statements = sql.split(";", Qt::SkipEmptyParts);

    for (QString& el: sql_statements){
        bool resp = db::Database::createDbExecuteQueries_(el);
        if (!resp){
            throw std::invalid_argument("Could not create table for " + el.toStdString());
            return false;
        }
    }

    // Datensätze aus User Form extrahieren
    QString currency = form_data.value("currency");
    QString account_name = form_data.value("account_name");
    QString bank_name = form_data.value("bank_name");
    QString account_holder = form_data.value("account_holder");

    bool convert;
    double balance = 0;
    balance = form_data.value("balance").toDouble(&convert);
    const int currency_id = 1;

    QSqlQuery query;

    // Root Tabelle Datensatz hinzufügen
    query.prepare("INSERT INTO Root (database_name, currency) "
                  "VALUES (:database_name, :currency)");
    query.bindValue(":database_name", db_name);
    query.bindValue(":currency", currency);

    db::Database::createDbExecuteQueries_(query);

    // Icons Tabelle Datensätze hinzufügen
    QString icons_path = constants::ROOT + "/" + constants::DB_ICONS_DATA;
    QFile icons_file(icons_path);

    // Prüfen, ob Icons CSV File geöffnet werden kann. Falls nicht Funktion beenden.
    if (!icons_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("Could not parse Icons file for Database Setup.");
        return false;
    }

    // Daten aus Icons File einlesen und in TextStream speichern
    QTextStream icons(&icons_file);
    QList<QList<QString>> icons_data;

    while (!icons.atEnd()) {
        QString line = icons.readLine();
        QList<QString> row = line.split(";");
        icons_data.append(row);
    }

    icons_file.close();

    for (const QList<QString>& element: icons_data){
        query.prepare("INSERT INTO Icons (name, icon, color_code, color_code_name) "
                      "VALUES (:name, :icon, :color_code, :color_code_name)");
        QString svg = element[2];

        // svg = svg.replace("\"\"", "\"");
        if (svg[svg.length() -1] == "\""){
            svg.remove(svg.length() - 1, 1);
        }
        if (svg[0] == "\""){
            svg.remove(0,1);
        }
        query.bindValue(":name", element[1]);
        query.bindValue(":icon", svg);
        query.bindValue(":color_code", element[3]);
        query.bindValue(":color_code_name", element[4]);

        db::Database::createDbExecuteQueries_(query);

    }

    // Accounts Tabelle Datensatz hinzufügen
    query.prepare("INSERT INTO Accounts (name, icon, account_holder, currency_id, opening_balance, bank_name) "
                  "VALUES (:name, :icon, :account_holder, :currency_id, :opening_balance, :bank_name)");
    query.bindValue(":name", account_name);
    query.bindValue(":icon", constants::ICON_DEFAULT);
    query.bindValue(":account_holder", account_holder);
    query.bindValue(":currency_id", currency_id);
    query.bindValue(":opening_balance", balance);
    query.bindValue(":bank_name", bank_name);

    db::Database::createDbExecuteQueries_(query);

    // Kategorien hinzufügen (unkategorisiert)
    query.prepare("INSERT INTO Categories_group (icon, name, type) VALUES (:icon, :name, :type)");
    query.bindValue(":icon", constants::ICON_DEFAULT); // 88 rotes svg label
    query.bindValue(":name", constants::CATEGORY_DEFAULT); // unkategorisiert
    query.bindValue(":type", constants::ACCOUNT_DEFAULT); //none

    db::Database::createDbExecuteQueries_(query);

    // Unterkategorien hinzufügen (unkategorisiert)
    query.prepare("INSERT INTO Categories (name, parent_group_id) VALUES (:name, :parent_group_id)");
    query.bindValue(":name", constants::CATEGORY_DEFAULT); // unkategorisiert
    query.bindValue(":parent_group_id", 1); //none

    db::Database::createDbExecuteQueries_(query);

    return true;
}

bool db::Database::createDbExecuteQueries_(QString& q)
{
    QSqlQuery query(q);
    if (constants::DEBUG){
        qDebug() << "Proceeds:" << q;
    }
    if (!query.exec(q)) {
        throw std::invalid_argument(query.lastError().text().toStdString());
        return false;
    }
    return true;
}

bool db::Database::createDbExecuteQueries_(QSqlQuery& q)
{
    if (!q.exec()){
        throw std::invalid_argument(q.lastError().text().toStdString());
        return false;
    }
    return true;
}

void db::Database::validateDatabase_()
{
    // Abbruch bei nicht gültiger Datenbank
    if (!this->db_.open()) {
        QMessageBox::critical(nullptr, "Error", "Fehler beim Öffnen der Datenbank");
        QCoreApplication::quit();
    }

    QSqlQuery query(this->db_);
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        QString error = "Keine valide SQLite-Datenbank (sqlite_master nicht gefunden): " + query.lastError().text();
        QMessageBox::critical(nullptr, "Error", error);
        QCoreApplication::quit();
    }
    bool validateRoot = false;
    QString param = "SELECT * FROM " + constants::TABLE_ROOT + ";";
    query.prepare(param);
    query.exec();
    while (query.next()){
        int id = query.value("id").toInt();
        if (id){
            validateRoot = true;
        }
    }
    if (!validateRoot){
        QMessageBox::critical(nullptr, "Error", "Fehler beim Öffnen der Datenbank, Root konnte nicht validiert werden");
        QCoreApplication::quit();
    }
}
