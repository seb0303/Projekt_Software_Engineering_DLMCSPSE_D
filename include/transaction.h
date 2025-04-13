#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QObject>
#include <QMap>
#include <QPair>
#include <QDebug>
#include <QSqlQuery>
#include <QString>

namespace core {
class StateMgr;  // Vorwärtsdeklaration
}
/**
 * @class Transaction
 * @brief Verwaltet Transaktionsdaten und Schnittstellen zur Datenbank für Buchungen.
 *
 * Die Klasse `Transaction` dient als Logikeinheit zur Verarbeitung von Einnahmen- und Ausgabentransaktionen.
 * Sie ruft relevante Daten wie Kategorien, Subkategorien, Konten und Währungen ab,
 * validiert Eingaben und speichert Transaktionen in der SQLite-Datenbank.
 *
 * Die Klasse ist eng mit dem zentralen Zustandsobjekt @class StateMgr verknüpft
 * und kann direkt mit UI-Komponenten (z. B. Dialogen) verbunden werden.
 *
 * Features:
 * - Abruf von Kategorien und Unterkategorien
 * - Abruf von Konten und Währungsinformationen
 * - Laden einzelner Transaktionen anhand ihrer ID
 * - Validierung und Speicherung von Transaktionsdaten
 *
 *
 * @param parent Zeiger auf das übergeordnete QObject (@class TransactionDialog)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */
class Transaction : public QObject
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QPair<bool, QString> validateData_(QMap<QString, QString>& form_data);
public:
    explicit Transaction(QObject *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    QMap<int, QPair<QString, QString>> getCategories();
    QMap<int, QPair<int, QString>> getSubCategories();
    QPair<int, QString> getCurrency();
    QMap<int, QString> getAccounts();
    QSqlQuery getTransaction(const int id);

public slots:
    QPair<bool, QString> save_data(QMap<QString, QString>& form_data, const int row = 0);
};

#endif // TRANSACTION_H
