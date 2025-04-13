#ifndef OVERVIEWDIALOG_H
#define OVERVIEWDIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QMap>

namespace Ui {
class OverviewDialog;
}

namespace core {
class StateMgr;
}
/**
 * @class OverviewDialog
 * @brief Stellt eine tabellarische Übersicht über alle Benutzerkonten bereit.
 *
 * Diese Klasse zeigt dem Benutzer eine zusammenfassende Tabelle mit allen gespeicherten Konten,
 * inklusive Name, Währung, aktuellem Kontostand und weiteren Informationen.
 * Sie dient der schnellen Analyse und Kontrolle des Gesamtbestands.
 *
 * Die Daten werden aus der aktuell geladenen Datenbank bezogen
 * und über @class StateMgr aktualisiert bzw. koordiniert.
 *
 * Features:
 * - Dynamische Darstellung der Kontoinformationen in einer Tabelle (`QTableWidget`)
 * - Unterstützung mehrerer Konten und Währungen
 * - Schließen des Dialogs über einen Button
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class MainWindow)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */

class OverviewDialog : public QWidget
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QPushButton* pushButton_quit_;
    QTableWidget* table_accounts_;
public:
    explicit OverviewDialog(QWidget *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    ~OverviewDialog();

private:
    Ui::OverviewDialog *ui;
};

#endif // OVERVIEWDIALOG_H
