#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QPair>
#include <QIcon>
#include <QString>
#include <QSqlQuery>
#include <QRegularExpression>

namespace core {
class StateMgr;
}
class Account : public QObject
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
public:
    explicit Account(QObject *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    // Ruft alle Konten ab, die in der Datenbank gespeichert sind
    QMap<int, QPair<QIcon, QString>> getAccounts();
    // Ruft ein bestimmtes Konto ab und gibt die Details als QMap zurücl
    QMap<QString, QVariant> getAccountDetails(int id);
    // Speichert einen Datensatz in der Datenbank (neuer oder bearbeiteter Datensatz) und gibt die Id zurück
    bool saveAccount(QString& type, QMap<QString, QVariant>& form_data);

private:
    bool validate_records_(QString &type, QMap<QString, QVariant>& form_data);
};

#endif // ACCOUNT_H
