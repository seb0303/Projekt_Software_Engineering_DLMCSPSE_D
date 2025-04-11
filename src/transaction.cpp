#include "../include/transaction.h"
#include "../include/statemgr.h"

Transaction::Transaction(QObject *parent, core::StateMgr *ptrStateMgr)
    : QObject{parent}
    ,ptrStateMgr_(ptrStateMgr)
{
    if (!ptrStateMgr_) {
        QMessageBox::information(nullptr, "Hinweis", "Fehler bei Transaktionsdialog");
        return;
    }
}

QMap<int, QPair<QString, QString>> Transaction::getCategories()
{
    QSqlQuery q = ptrStateMgr_->getCategories();
    // <id, <Typ (Einnahmen / Ausgaben), Bezeichnung>
    QMap<int, QPair<QString, QString>> out;
    QPair<QString, QString> data;
    while(q.next()){
        data.first = q.value("type").toString();
        data.second = q.value("name").toString();
        out[q.value("id").toInt()] = data;
    }
    return out;
}

QMap<int, QPair<int, QString>> Transaction::getSubCategories()
{
    QSqlQuery q = ptrStateMgr_->getSubCategories();
    QMap<int, QPair<int, QString>> out;
    QPair<int, QString> data;
    while(q.next()){
        data.first = q.value("parent_group_id").toInt();
        data.second = q.value("name").toString();
        out[q.value("id").toInt()] = data;
    }
    return out;
}

QPair<int, QString> Transaction::getCurrency()
{
    QString q = ptrStateMgr_->getCurrency();
    QPair<int, QString> out;
    out.first = 1;
    out.second = q;

    return out;
}

QMap<int, QString> Transaction::getAccounts()
{
    QSqlQuery q = ptrStateMgr_->getAccounts();
    QMap<int, QString> out;

    while(q.next()){
        out[q.value("id").toInt()] = q.value("name").toString();
    }
    return out;
}

QSqlQuery Transaction::getTransaction(const int id)
{
    QSqlQuery q = ptrStateMgr_->getTransactions(true, id);
    return q;
}

QPair<bool, QString> Transaction::save_data(QMap<QString, QString>& form_data, const int row)
{

    QPair<bool, QString> resp = this->validateData_(form_data);
    if (!resp.first){
        QString message = "Pflichtfeld fehlt: " + resp.second;
        resp.second = message;
        return resp;
    }
    // Datum umwandeln für Datenbankimport
    qDebug() << form_data["transaction_date"];
    QDate date = QDate::fromString(form_data["transaction_date"], "dd.MM.yyyy");
    if (!date.isValid()){
        resp.first = false;
        resp.second = "Das Datum entspricht nicht dem Format DD.MM.JJJJ";
        return resp;
    }
    QString date_str = date.toString("yyyy-MM-dd");
    form_data["transaction_date"] = date_str;

    QString operation;
    if (form_data["id"] == "neu") {
        operation = "insert";
    }
    else {
        operation = "update";
    }

    // Daten an StateMgr übergeben um Datenbank zu aktualisieren
    bool status = ptrStateMgr_->db->update_or_create_transaction(form_data, operation);
    // Abbruch falls es ein Problem beim Aktualisieren der Datensätze gab
    if (!status){
        resp.first = false;
        resp.second = "Ein Fehler bei der Aktualisierung der Datenbank ist aufgetreten";
        return resp;
    }

    // Wenn status true muss jetzt das UI Update erfolgen

    status = ptrStateMgr_->setTableItem(form_data);

    if (status){
        resp.first = true;
        resp.second = "ok";
        // Balance aktualisieren ???? noch prüfen...
        // QSqlQuery q = ptrStateMgr_->getBalance();
        // ptrStateMgr_->setBalance(q);
    }
    else {
        resp.first = false;
        resp.second = "Ein Fehler beim Hinzufügen oder Bearbeiten eines Datensatzes ist aufgetreten";
        qWarning () << "Error at transaction.cpp (QPair<bool, QString> Transaction::save_data(param)) while creating or updating a transaction";
    }
    return resp;

}
QPair<bool, QString> Transaction::validateData_(QMap<QString, QString>& form_data)
{
    QPair<bool, QString> resp;
    resp.first = false;

    // Zuerst Regex für Eingabe des Betrags
    // RegEx um sicherzustellen, dass nur Beträge in Deutschem Format in AmountLineEdit eingegeben werden dürfen (Beispiel: 102,37, 1.234,56)
    QRegularExpression regex("^(-?((0|[1-9][0-9]*)?(,[0-9]{1,2})?)|Delete)$");
    form_data["amount"] = form_data["amount"].replace(".","");
    QString amountText = form_data["amount"];
    // Prüfe, ob der Text dem Regex entspricht
    QRegularExpressionMatch match = regex.match(amountText);
    if (!match.hasMatch()){
        resp.second = "Format bei Betrag nicht eingehalten. Betrag muss dem deutschen Format entsprechen (Beispiel: 76, 102,37 oder 1.234,56)";
        return resp;
    }

    // Pflichtfelder checken
    // Id, Konto, Datum, Betrag, Kategorien
    // Rest ist optional und muss nicht angegeben werden
    if (form_data["id"] == "") {
        resp.second = "id";
    }
    else if (form_data["account_id"] == "") {
        resp.second = "Konto";
    }
    else if (form_data["transaction_date"] == ""){
        resp.second = "Transaktionsdatum";
    }
    else if (form_data["amount"] == ""){
        resp.second = "Betrag";
    }
    else if (form_data["category_id"].toInt() == 0){
        resp.second = "Unterkategorie";
    }
    else if (form_data["category_group_id"].toInt() == 0){
        resp.second = "Kategorie";
    }
    else {
        resp.first = true;
        resp.second = "ok";
    }

    return resp;
}
