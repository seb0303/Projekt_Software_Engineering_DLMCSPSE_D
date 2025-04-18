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
/**
 * @class Account
 * @brief Modelliert ein Bankkonto mit Name, Währung, Saldo und weiteren zugehörigen Eigenschaften.
 *
 * Diese Klasse verwaltet Kontodaten innerhalb von PennyWise.
 * Sie ermöglicht das Abrufen, Anzeigen und Speichern von Kontoinformationen
 * in der zugrunde liegenden SQLite-Datenbank.
 *
 * Die Klasse greift auf die Datenbank über eine `StateMgr`-Instanz zu
 * und kapselt die Validierung sowie Datenverarbeitung.
 *
 * Funktionen:
 * - Abrufen aller gespeicherten Konten
 * - Laden eines einzelnen Kontos anhand seiner ID
 * - Validierung und Speichern eines neuen oder bearbeiteten Kontos
 *
 * @note Verwendet Qt-spezifische Datentypen wie `QMap`, `QVariant`, `QIcon`.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class AccountDialog)
 * @param stateMgr Zeiger (@class StateMgr) für die Verwaltung der UI und States
 *
 * @see StateMgr
 */

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
    // Speichert einen Datensatz in der Datenbank (neuer oder bearbeiteter Datensatz) und gibt die ID des Accounts zurück
    bool saveAccount(QString& type, QMap<QString, QVariant>& form_data);

private:
    bool validate_records_(QString &type, QMap<QString, QVariant>& form_data);
};

#endif // ACCOUNT_H
