#include "../include/account.h"
#include "../include/statemgr.h"

Account::Account(QObject *parent, core::StateMgr *ptrStateMgr)
    : QObject{parent}
    , ptrStateMgr_(ptrStateMgr)
{}

QMap<int, QPair<QIcon, QString> > Account::getAccounts()
{
    QMap<int, QPair<QIcon, QString> > out;
    QSqlQuery q = this->ptrStateMgr_->getAccounts();
    while (q.next()){
        QIcon icon = ptrStateMgr_->convertSVGImageFromDb(q.value("icon").toInt());
        QString name = q.value("name").toString();
        QPair<QIcon, QString> inner{icon, name};
        out[q.value("id").toInt()] = inner;
    }
    return out;
}

QMap<QString, QVariant> Account::getAccountDetails(int id)
{
    QMap<QString, QVariant> out;
    QSqlQuery q = ptrStateMgr_->getAccounts();
    QString currency_str = ptrStateMgr_->getCurrency();
    while (q.next()){
        // Prüfung ob mit angefordertem Element übereinstimmt
        if (q.value("id").toInt() == id){
            out["id"] = q.value("id").toInt();
            out["icon"] = q.value("icon").toInt();
            out["name"] = q.value("name").toString();
            out["account_holder"] = q.value("account_holder").toString();
            out["currency_id"] = currency_str;
            out["account_number"] = q.value("account_number").toString();
            out["iban"] = q.value("iban").toString();
            out["bank_code"] = q.value("bank_code").toString();
            out["bic"] = q.value("bic").toString();
            out["bank_name"] = q.value("bank_name").toString();
            out["opening_balance"] = q.value("opening_balance").toDouble();
            break;
        }
    }
    return out;
}

bool Account::saveAccount(QString &type, QMap<QString, QVariant> &form_data)
{
    if (constants::DEBUG){
        qDebug() << "Submitted values to saveAccount with type" << type;
        for (auto [key, value] : form_data.asKeyValueRange()) {
            qDebug() << "Key:" << key << "Name:" << value;
        }
    }
    // Validierung der übermittelten Daten
    bool resp = validate_records_(type, form_data);
    if (!resp) return false; // Return falls Pflichtfelder und Formate nicht eingehalten
    resp = ptrStateMgr_->db->update_or_create_account(form_data);
    if (!resp) return false;
    return true;
}

bool Account::validate_records_(QString& type, QMap<QString, QVariant> &form_data)
{
    // Pflichtfelder name, icon
    // Zusatzprüfung opening balance Regex

    // Prüfung der ID und des Typs
    if (type == "new"){
        if (form_data["id"].toInt() != 0){
            form_data["status_message"] = "Falsche ID für Neuanlage gesetzt. Übermittelte ID: " + form_data["id"].toString();
            return false;
        }
    }
    else if (type == "edit"){
        if (form_data["id"].toInt() == 0){
            form_data["status_message"] = "ID bei Bearbeitung darf nicht null sein. Übermittelte ID: " + form_data["id"].toString();
            return false;
        }
    }
    else {
        form_data["status_message"] = "Falscher Parameter. Zulässige Typen -> (new und edit)";
        return false;
    }
    if (form_data["name"].toString() == ""){
        form_data["status_message"] = "Bitte vergib einen Namen für das Konto";
        return false;
    }
    if (form_data["icon"].toInt() == 0){
        form_data["status_message"] = "Bitte wähle ein Icon für dein Konto aus";
        return false;
    }


    // RegEx um sicherzustellen, dass nur Beträge in Deutschem Format in AmountLineEdit eingegeben werden dürfen (Beispiel: 102,37, 1.234,56)
    QRegularExpression regex("^(-?((0|[1-9][0-9]*)?(,[0-9]{1,2})?)|Delete)$");
    form_data["opening_balance"] = form_data["opening_balance"].toString().replace(".","");
    QString amountText = form_data["opening_balance"].toString();
    // Prüfe, ob der Text dem Regex entspricht
    QRegularExpressionMatch match = regex.match(amountText);
    if (!match.hasMatch()){
        form_data["status_message"] = "Format bei Betrag nicht eingehalten. Betrag muss dem deutschen Format entsprechen (Beispiel: 76, 102,37 oder 1.234,56)";
        return false;
    }

    return true;
}
