#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QTableView>
#include <QAbstractItemModel>
#include <QString>
#include <QMap>
/**
 * @class Search
 * @brief Führt benutzerdefinierte Suchanfragen auf Transaktionsdaten durch und zeigt die Ergebnisse an.
 *
 * Die Klasse `Search` dient der Verarbeitung und Darstellung von Suchanfragen innerhalb der PennyWise-Anwendung.
 * Sie nimmt einen vom Benutzer eingegebenen Text entgegen, verarbeitet diesen als Filterkriterium
 * und zeigt die gefilterten Transaktionen in einer übergebenen Tabellenansicht an.
 *
 *
 * Features:
 * - Annahme von Suchbegriffen als `QString`
 * - Filterung der Transaktionsdatenbank basierend auf dem Textinhalt
 * - Darstellung der Ergebnisse in einem Qt `QTableView`
 * - Slot-basierte Integration mit UI-Signalen (z. B. Sucheingabefeld)
 *
 * @param parent Zeiger auf das übergeordnete QObject (@class MainWindow)
 */

class Search : public QObject
{
    Q_OBJECT
public:
    explicit Search(QObject *parent = nullptr);

public slots:
    void handleSearchEvent(QString query, QTableView* ptrTableView);
};

#endif // SEARCH_H
