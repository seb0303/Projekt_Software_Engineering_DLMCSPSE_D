#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QDateTime>
#include <QTimer>
#include <QString>
#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidget>
#include <QFrame>
#include <QRadioButton>

#include "status_log.h"
#include "database.h"

#include "statemgr.h"

#include "transactionDialog.h"
#include "categoryDialog.h"
#include "accountDialog.h"
#include "overviewDialog.h"
#include "helpDialog.h"
#include "exportDialog.h"
#include "databaseDialog.h"
#include "chart.h"
#include "search.h"

namespace Ui {
class MainWindow;
}
/**
 * @class MainWindow
 * @brief Hauptfenster und zentrale Steuerkomponente der PennyWise-Anwendung.
 *
 * Die Klasse MainWindow bildet das Hauptfenster der Anwendung PennyWise und
 * stellt die zentrale Benutzeroberfläche für den Zugriff auf alle Kernfunktionen bereit.
 * Sie organisiert den Zugriff auf Dialoge, verwaltet Statusmeldungen, koordiniert
 * die Darstellung und ruft Daten aus der Datenbank über das Zustandsobjekt @class StateMgr ab.
 *
 * Das MainWindow dient als Einstiegspunkt nach dem Laden oder Erstellen einer Datenbank (@class WelcomeScreen oder @class AddDatabaseDialog)
 * und hält Referenzen auf alle logischen und visuellen Komponenten:
 * - Transaktionen hinzufügen, bearbeiten, löschen
 * - Kategorien und Konten verwalten
 * - Finanzdaten filtern und durchsuchen
 * - Exportfunktionen starten und Backups durchführen
 * - Statistische Diagramme anzeigen
 * - Systemstatus darstellen
 *
 * Die Klasse enthält Initialisierungslogik für die UI-Elemente sowie
 * Signal-Slot-Verbindungen zur Reaktion auf Benutzeraktionen.
 *
 * Features:
 * - Navigation zu allen Funktionalitäten über Buttons, Menü oder UI-Steuerelemente
 * - Echtzeit-Aktualisierung des Statuslabels über @class Status_Log
 * - Start und Verwaltung aller sekundären Dialoge (Accounts, Transactions, Exports usw.)
 * - Verwaltung und Übergabe der aktiven Datenbankverbindung an andere Komponenten
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (nullptr -> kein parent, Speicherverwaltung manuell notwendig über Dekonstruktor)
 * @param path Dateipfad zur geladenen Datenbank
 * @param database_name Name der Datenbankdatei
 * @param passwordRequired Gibt an, ob beim Öffnen der Datenbank ein Passwort abgefragt wurde
 * @param password Optional übergebenes Passwort für verschlüsselte Datenbanken
 * @param connection_name default für benutzerspezifische Datenbank, demo für Demodatenbank
 */

class MainWindow : public QWidget
{
    Q_OBJECT
    db::Database database_;
public:
    // Status Label und Logger für Statusupdates
    Status_Log logger;
    QLabel* lbl_status;
    // Constructor mit Datenbankparametern
    explicit MainWindow(
        QWidget *parent = nullptr,
        QString path = "",
        QString database_name = "",
        bool passwordRequired = false,
        QString password = "",
        QString connection_name = "default"
        );

    ~MainWindow();

    // Zeit- und Datumsangaben im MainWindow (Statuszeile)
    void updateDateTime();
    // Index Change ComboBox Zeitraum
    void handleComboBoxTimePeriodChange();
    // Index Change TreeView Kategorien
    void handleTreeViewCategoryChange();
    // Header Label Update
    // Wird aktualisiert, wenn ein Item in der Seitenleiste (Kategorie oder Account) ausgewählt wird
    void handleSidebarEventLabelUpdate();
    // Komplette UI aktualisieren
    void handleBtnRefreshAll();
    // Handler für Sidebar
    // Wird aufgerufen wenn Nutzer Konten in TreeView selektiert
    bool handleAccountChangeEvent();
    // Handler für neuen Umsatz und/oder Bearbeitung eines bestehendes Umsatzes
    bool handleNewTransactionDialog();
    // Handler für Kategorien
    bool handleCategoryDialog();
    // Handler für Accounts
    bool handleAccountDialog();
    // Handler für Übersicht
    bool handleOverviewDialog();
    // Handler für Hilfe
    bool handleHelpDialog();
    // Handler für Export
    bool handleExportDialog();
    // Handler für Backup
    bool handleBackupDialog();
    // Handler für Datenbankverwaltung
    bool handleDatabaseDialog();
    // Chart initialisieren
    void initChart(QString type = "bar");
public slots:
    void handleChartRefresh();
private slots:
    void handleSearchQuery_(QString query);
private:
    Ui::MainWindow *ui;
    QLabel* lbl_account_selection;
    QComboBox* comboBox_time_period_;
    QPushButton* pushButton_action_refresh_all;
    QPushButton* pushButton_action_add_transaction;
    QPushButton* pushButton_action_category;
    QPushButton* pushButton_action_account;
    QPushButton* pushButton_action_db;
    QPushButton* pushButton_action_resetTable;
    QPushButton* pushButton_action_overview_sidebar;
    QPushButton* pushButton_help;
    QPushButton* pushButton_action_export;
    QPushButton* pushButton_action_backup;
    QRadioButton* radioButton_pieChart;
    QRadioButton* radioButton_barChart;
    QTreeWidget* treeWidget_account;
    QTreeWidget* treeWidget_category;
    QLineEdit* lineEdit_search;
    QTableView* table_transactions_;
    QFrame* frame_charts;
    Chart* chart;
    QWidget* chartView;
    Search* search_;
    core::StateMgr stateMgr_;
    int comboBox_time_period_prior_index;
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void startSearchQuery(QString query, QTableView* table_view);
};

#endif
