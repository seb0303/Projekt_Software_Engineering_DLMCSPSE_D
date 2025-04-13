#ifndef EXPORT_H
#define EXPORT_H

#include <QObject>
#include <QWidget>
#include <QSet>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>

namespace core {
class StateMgr;
}
/**
 * @class Export
 * @brief Bietet Funktionen zum Exportieren von Datenbankinhalten in externe Dateien.
 *
 * Diese Klasse stellt die Logik zur Verfügung, um ausgewählte Datenbanktabellen oder
 * die gesamte Datenbank in externen Dateiformaten (CSV) zu exportieren.
 *
 * Der Export erfolgt über das Dateisystem und nutzt Standard-Dialoge über die Win-API zur Auswahl des Speicherorts.
 * Die zu exportierenden Daten werden aus der SQLite-Datenbank gelesen, aufbereitet und
 * in einer kommaseparierten Textform gespeichert.
 *
 * Features:
 * - Export einzelner oder mehrerer Tabellen
 * - Komplett-Export der Datenbankstruktur und Inhalte
 * - Dateinamen basieren auf Standardverzeichnissen (`QStandardPaths`)
 *
 * @note Die Klasse verwendet Qt-Komponenten wie `QSqlQuery`, `QTextStream`, `QFile`, `QFileDialog`.
 *
 * @param parent Zeiger auf das übergeordnete QObject (@class ExportDialog)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */

class Export : public QObject
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
public:
    explicit Export(QObject *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
signals:
public slots:
    void exportTables(QSet<QString>& tables);
    void exportDatabase();
};

#endif // EXPORT_H
