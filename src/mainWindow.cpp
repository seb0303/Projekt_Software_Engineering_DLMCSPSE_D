#include "../include/mainWindow.h"
#include "../ui/ui_mainWindow.h"

MainWindow::MainWindow(
    QWidget *parent,
    QString path,
    QString database_name,
    bool passwordRequired,
    QString password,
    QString connection_name
    )
    : QWidget(parent)
    , database_(path, database_name, passwordRequired, password, connection_name, this)
    , logger()
    , stateMgr_(this, &this->database_)
    , ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose); // Aus dem Heap löschen wenn geschlossen
    ui->setupUi(this);

    if(!database_.connect()){
        throw std::runtime_error("Could not connect to database");
    }

    // UI Komponenten initialisieren
    lbl_status = this->ui->lbl_status;
    lbl_account_selection = this->ui->lbl_account_selection;
    comboBox_time_period_ = this->ui->comboBox_time_period;
    treeWidget_account = this->ui->treeWidget_account;
    treeWidget_category = this->ui->treeWidget_category;
    pushButton_action_refresh_all = this->ui->pushButton_action_refresh_all;
    pushButton_action_add_transaction = this->ui->pushButton_action_add_transaction;
    pushButton_action_resetTable = this->ui->pushButton_action_resetTable;
    pushButton_action_category = this->ui->pushButton_action_category;
    pushButton_action_account = this->ui->pushButton_action_account;
    pushButton_action_db = this->ui->pushButton_action_db;
    pushButton_action_overview_sidebar = this->ui->pushButton_action_overview_sidebar;
    pushButton_help = this->ui->pushButton_help;
    pushButton_action_export = this->ui->pushButton_action_export;
    pushButton_action_backup = this->ui->pushButton_action_backup;
    lineEdit_search = this->ui->lineEdit_search;
    table_transactions_ = this->ui->table_transactions;

    // Logger und Timer initialisieren
    logger.print_status(lbl_status, "REFRESH", "Initialisiere...");
    // setIntervall (Event jede Sekunde) um die Uhrzeit in der Statusleiste zu aktualisieren
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timer->start(1000);
    updateDateTime();

    // Charts initialisieren
    frame_charts = this->ui->frame_charts;
    QLayout* frame_charts_layout = new QHBoxLayout(frame_charts);
    chart = nullptr;

    radioButton_pieChart = this->ui->radioButton_pieChart;
    radioButton_barChart = this->ui->radioButton_barChart;

    // Search Objekt initialisieren
    search_ = new Search(this);

    // Connectors
    // ComboBox TimePeriod (Veränderung des Zeitraums)
    connect(comboBox_time_period_, &QComboBox::currentIndexChanged, this, &MainWindow::handleComboBoxTimePeriodChange);
    // TreeView Category (Veränderung der Kategorie)
    connect(treeWidget_category, &QTreeWidget::currentItemChanged, this, &MainWindow::handleTreeViewCategoryChange);
    // BtnRefreshAll (komplette Ui aktualisieren)
    connect(pushButton_action_refresh_all, &QPushButton::clicked, this, &MainWindow::handleBtnRefreshAll);
    // QTreeWidget Accounts
    connect(treeWidget_account, &QTreeWidget::itemSelectionChanged, this, &MainWindow::handleAccountChangeEvent);
    // Hinzufügen einer Transaktion
    connect(pushButton_action_add_transaction, &QPushButton::clicked, this, &MainWindow::handleNewTransactionDialog);
    // Tabellenbreite auf Standard zurücksetzen
    connect(pushButton_action_resetTable, &QPushButton::clicked, this, [this](){
        QTableWidget* tbl = this->findChild<QTableWidget* >("table_transactions");
        if (!tbl){
            qDebug() << "Could not find table";
            return;
        }
        tbl->resizeColumnsToContents();
    });
    // Kategorien hinzufügen oder ändern
    connect(pushButton_action_category, &QPushButton::clicked, this, &MainWindow::handleCategoryDialog);
    // Accounts hinzufügen oder ändern
    connect(pushButton_action_account, &QPushButton::clicked, this, &MainWindow::handleAccountDialog);
    // Übersicht über Konten anzeigen
    connect(pushButton_action_overview_sidebar, &QPushButton::clicked, this, &MainWindow::handleOverviewDialog);
    // Help Dialog anzeigen
    connect(pushButton_help, &QPushButton::clicked, this, &MainWindow::handleHelpDialog);
    // Export Dialog anzeigen
    connect(pushButton_action_export, &QPushButton::clicked, this, &MainWindow::handleExportDialog);
    // Datenbankverwaltung anzeigen
    connect(pushButton_action_db, &QPushButton::clicked, this, &MainWindow::handleDatabaseDialog);
    // Backup erstellen
    connect(pushButton_action_backup, &QPushButton::clicked, this, &MainWindow::handleBackupDialog);
    // Diagramme toggeln
    connect(radioButton_barChart, &QRadioButton::clicked, this, [this](){
       initChart("bar");
    });
    connect(radioButton_pieChart, &QRadioButton::clicked, this, [this](){
        initChart("pie");
    });
    // Search Query
    connect(lineEdit_search, &QLineEdit::textChanged, this, &MainWindow::handleSearchQuery_);
    connect(this, &MainWindow::startSearchQuery, search_, &Search::handleSearchEvent);

    logger.print_status(lbl_status, "SUCCESS", "Alles aktualisiert");

    stateMgr_.setAll();
    comboBox_time_period_prior_index = comboBox_time_period_->currentIndex();

}

MainWindow::~MainWindow() {
    qDebug() << "MainWindow Deconstructed, Memory released";
    delete ui;
}

void MainWindow::updateDateTime(){
    auto date_time = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
    this->ui->lbl_time->setText(date_time);
}

void MainWindow::handleComboBoxTimePeriodChange() {

    QString type = this->comboBox_time_period_->currentData(Qt::UserRole).toString();
    QString filter = this->comboBox_time_period_->currentData(Qt::UserRole +1).toString();

    if (type == "all"){
        bool resp = QMessageBox::question(nullptr,"Alle Datensätze abrufen", "Bist du sicher, dass du alle Datensätze abrufen möchtest? "
                                        "Dieser Vorgang kann einige Zeit in Anspruch nehmen.", "Nein", "Ja");
        if (resp){
            // Cursor setzen
            QApplication::setOverrideCursor(Qt::WaitCursor);
            this->logger.print_status(this->lbl_status, "REFRESH", "Aktualisiere Tabelle...");
            QApplication::processEvents();

            // StateMgr Properties für get_state setzen
            QDate date_from = constants::MAX_DATE_FROM;
            QDate date_to = QDate::currentDate();
            this->stateMgr_.setTimePeriodParam(date_from, date_to);
            // Query für alle Datensätze
            QSqlQuery q = this->stateMgr_.getTransactions();
            this->stateMgr_.setTransactions(q);
            QApplication::processEvents();
            QApplication::restoreOverrideCursor();
            QMessageBox::information(nullptr, "Information", "Alle Datensätze wurden erfolgreich abgerufen");
            this->logger.print_status(this->lbl_status, "SUCCESS", "Alles aktualisiert");
            return;
        }
        else {
            // Falls Nutzer nicht alle Datensätze abrufen möchte, Index der ComboBox
            // zurücksetzen auf Prior Index und Funktion beenden
            this->comboBox_time_period_->setCurrentIndex(this->comboBox_time_period_prior_index);
            return;
        }
    }
    else if (type == "days"){
        int convert_days = filter.toInt();
        QApplication::setOverrideCursor(Qt::WaitCursor);
        this->logger.print_status(this->lbl_status, "REFRESH", "Aktualisiere Tabelle...");
        QApplication::processEvents();

        // StateMgr Properties für get_state setzen
        QDate date_from = QDate::currentDate().addDays(-convert_days);
        QDate date_to = QDate::currentDate();
        bool resp = this->stateMgr_.setTimePeriodParam(date_from, date_to);
        // Query für Filter bestimmter Zeitraum
        QSqlQuery q = this->stateMgr_.getTransactions();
        this->stateMgr_.setTransactions(q);
        QApplication::restoreOverrideCursor();
        this->logger.print_status(this->lbl_status, "SUCCESS", "Alles aktualisiert");

        // Check ob Zeitraum gesetzt wurde
        if (!resp) qDebug() << "Could not set TimePeriodParam on void MainWindow::handleComboBoxTimePeriodChange()";
    }
    else if (type == "year"){
        int convert_year = filter.toInt();
        QDate date_from(convert_year, 1,1);
        QDate date_to(convert_year, 12,31);
        this->stateMgr_.setTimePeriodParam(date_from, date_to);

        QApplication::setOverrideCursor(Qt::WaitCursor);
        this->logger.print_status(this->lbl_status, "REFRESH", "Aktualisiere Tabelle...");
        QApplication::processEvents();

        // Query für Filter bestimmter Zeitraum
        QSqlQuery q = this->stateMgr_.getTransactions();
        this->stateMgr_.setTransactions(q);
        QApplication::restoreOverrideCursor();
        this->logger.print_status(this->lbl_status, "SUCCESS", "Alles aktualisiert");

        // StateMgr Properties für get_state setzen
        bool resp = this->stateMgr_.setTimePeriodParam(date_from, date_to);
        if (!resp) qDebug() << "Could not set TimePeriodParam on void MainWindow::handleComboBoxTimePeriodChange()";
    }


    QApplication::restoreOverrideCursor();
    this->comboBox_time_period_prior_index = this->comboBox_time_period_->currentIndex();
}

void MainWindow::handleTreeViewCategoryChange()
{
    QTreeWidgetItem* selected_element = treeWidget_category->currentItem();
    if (!selected_element) return;
    QString category_type = selected_element->data(0, Qt::UserRole +2).toString();
    int id = selected_element->data(0, Qt::UserRole).toInt();
    qDebug() << category_type << id;

    bool ok;

    ok = stateMgr_.setCategoryIdParam(id);
    if (!ok) {
        qDebug() << "Could not set Category ID Param on Function MainWindow::handleTreeViewCategoryChange()";
        return;
    }
    ok  = stateMgr_.setCategoryTypeParam(category_type);
    if (!ok) {
        qDebug() << "Could not set Category Type Param on Function MainWindow::handleTreeViewCategoryChange()";
        return;
    }
    QSqlQuery q = stateMgr_.getTransactions();
    stateMgr_.setTransactions(q);
    this->handleSidebarEventLabelUpdate();
}

void MainWindow::handleSidebarEventLabelUpdate()
{
    // Sicherheitsprüfung
    if (!lbl_account_selection) return;

    QTreeWidgetItem* account_item = treeWidget_account->currentItem();
    QTreeWidgetItem* category_item = treeWidget_category->currentItem();
    QString account_name = "Alle Konten";
    QString category_name = "Alle Kategorien";
    if (account_item) {
        account_name = account_item->data(0, Qt::UserRole + 2).toString();
    }
    lbl_account_selection->setText(account_name);

    if (!category_item) return;
    category_name = category_item->data(0, Qt::UserRole +1).toString();

    QString label_html =
        "<html><head/><body><p>" +
        account_name +
        "  |  <em style='font-size: small; font-weight: normal;'>Kategorie: " +
        category_name +
        "</em></p></body></html>";

    lbl_account_selection->setText(label_html);
}

// Aktualisiert die komplette UI
void MainWindow::handleBtnRefreshAll()
{
    this->logger.print_status(this->lbl_status, "WARNING", "Aktualisiere UI...");
    QApplication::processEvents();
    this->stateMgr_.setAll();
    this->logger.print_status(this->lbl_status, "SUCCESS", "Alles aktualisiert");
}

// Ermittelt selektierten Account in TreeView (Sidebar) und lädt die
// selektierten Datensätze aus der Datenbank
bool MainWindow::handleAccountChangeEvent()
{
    QTreeWidget* el = this->ui->treeWidget_account;
    QTreeWidgetItem* item = el->currentItem();
    int id = item->data(0, Qt::UserRole).toInt();
    QString type = item->data(0, Qt::UserRole +1).toString();

    if (type == "all"){
        id = constants::ACCOUNT_ID_DEFAULT;
        this->stateMgr_.setAccountIdParam(id);
        this->stateMgr_.setAccountTypeParam(type);
        // Ui aktualisieren
        this->stateMgr_.setAll();
    }
    else if (type=="item"){
        bool resp1 = this->stateMgr_.setAccountIdParam(id);
        bool resp2 = this->stateMgr_.setAccountTypeParam(type);
        QSqlQuery q = this->stateMgr_.getTransactions();
        this->stateMgr_.setTransactions(q);
        q = this->stateMgr_.getBalance();
        this->stateMgr_.setBalance(q);

        if (!resp1 || !resp2) qDebug() << "Could not set TimePeriodParam on bool MainWindow::handleAccountChangeEvent()";
    }
    this->handleSidebarEventLabelUpdate();
    return true;
}

bool MainWindow::handleNewTransactionDialog()
{
    TransactionDialog *w = new TransactionDialog(this, 0, -1, &stateMgr_);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Neuen Umsatz hinzufügen");
    w->show();
    return true;

}

bool MainWindow::handleCategoryDialog()
{
    CategoryDialog *w = new CategoryDialog(this, &stateMgr_);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Kategorien verwalten");
    w->show();
    return true;

}

bool MainWindow::handleAccountDialog()
{
    AccountDialog* w = new AccountDialog(this, &stateMgr_);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Konten verwalten");
    w->show();
    return true;
}

bool MainWindow::handleOverviewDialog()
{
    OverviewDialog* w = new OverviewDialog(this, &stateMgr_);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Übersicht");
    w->show();
    return true;
}

bool MainWindow::handleHelpDialog()
{
    HelpDialog* w = new HelpDialog(this);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Hilfe");
    w->show();
    return true;
}

bool MainWindow::handleExportDialog()
{
    ExportDialog* w = new ExportDialog(this, &stateMgr_);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Export");
    w->show();
    return true;
}

bool MainWindow::handleBackupDialog()
{
    ExportDialog w(this, &stateMgr_);
    emit w.exportDb();
    return true;
}

bool MainWindow::handleDatabaseDialog()
{
    DatabaseDialog* w = new DatabaseDialog(this, &stateMgr_);
    w->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    w->setWindowModality(Qt::WindowModal);
    w->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    w->setWindowTitle("Datenbankverwaltung");
    w->show();
    return true;
}

void MainWindow::initChart(QString type)
{
    QLayout* layout = frame_charts->layout();

    if (chart) {
        if (chartView) {
            layout->removeWidget(chartView);
            delete chartView;
            chartView = nullptr;
        }

        delete chart;
        chart = nullptr;
    }

    chart = new Chart(this, type, &stateMgr_);
    chartView = chart->generateContainer();
    layout->addWidget(chartView);

    frame_charts->setContentsMargins(0, 0, 0, 0);
}


void MainWindow::handleChartRefresh()
{
    if (radioButton_barChart->isChecked()){
        initChart("bar");
        return;
    }
    if (radioButton_pieChart->isChecked()){
        initChart("pie");
    }
}

void MainWindow::handleSearchQuery_(QString query)
{
    emit startSearchQuery(query, table_transactions_);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
