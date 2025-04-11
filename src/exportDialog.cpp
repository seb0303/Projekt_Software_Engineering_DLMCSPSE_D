#include "../include/exportDialog.h"
#include "../ui/ui_exportDialog.h"
#include "../include/statemgr.h"
#include "../include/export.h"

ExportDialog::ExportDialog(QWidget *parent, core::StateMgr *ptrStateMgr)
    : QWidget(parent)
    , ui(new Ui::ExportDialog)
    , ptrStateMgr_(ptrStateMgr)
{
    ui->setupUi(this);
    export_ = new Export(this, ptrStateMgr_);

    // Ui Objekte initialisieren
    pushButton_quit_ = this->ui->pushButton_quit;
    pushButton_export_ = this->ui->pushButton_export;
    checkBox_accounts_ = this->ui->checkBox_accounts;
    checkBox_categories_ = this->ui->checkBox_categories;
    checkBox_subCategories_ = this->ui->checkBox_subCategories;
    checkBox_transactions_ = this->ui->checkBox_transactions;

    connect(pushButton_quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });
    connect(pushButton_export_, &QPushButton::clicked, this, &ExportDialog::handleExport_);
    connect(this, &ExportDialog::exportData, export_, &Export::exportTables);
    connect(this, &ExportDialog::exportDb, export_, &Export::exportDatabase);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::handleExport_()
{
    // Prüfen, welche CheckBox(en) gesetzt wurde(n)
    export_selection_ = {};
    if (checkBox_accounts_->checkState()){
        export_selection_.insert(constants::TABLE_ACCOUNTS);
    }
    if (checkBox_categories_->checkState()){
        export_selection_.insert(constants::TABLE_CATEGORIES);
    }
    if (checkBox_subCategories_->checkState()){
        export_selection_.insert(constants::TABLE_SUBCATEGORIES);
    }
    if (checkBox_transactions_->checkState()){
        export_selection_.insert(constants::TABLE_TRANSACTIONS);
    }

    // Prüfen, ob etwas ausgewählt wurde
    // Falls nicht, MessageBox und return
    if (export_selection_.count() == 0){
        QMessageBox::information(nullptr, "Hinweis", "Bitte wähle mindestens eine Tabelle aus");
        return;
    }
    emit exportData(export_selection_);
}
