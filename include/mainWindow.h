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
