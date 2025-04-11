#include "../include/overviewDialog.h"
#include "../ui/ui_overviewDialog.h"
#include "../include/statemgr.h"

OverviewDialog::OverviewDialog(QWidget *parent, core::StateMgr *ptrStateMgr)
    : QWidget(parent)
    , ui(new Ui::OverviewDialog)
    ,ptrStateMgr_(ptrStateMgr)
{
    ui->setupUi(this);
    if (!ptrStateMgr) {
        this->close();
        this->deleteLater();
        return;
    }

    pushButton_quit_ = this->ui->pushButton_quit;
    connect(pushButton_quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });

    table_accounts_ = this->ui->table_accounts;

    QLocale german(QLocale::German, QLocale::Germany);
    QSqlQuery q_accounts = ptrStateMgr_->getAccounts();
    QSqlQuery q_transactions = ptrStateMgr_->getBalance(true);
    QString currency = ptrStateMgr_->getCurrency();
    QMap<int, double> current_balance;
    int row = 0;

    // Saldo ermitteln
    while (q_accounts.next()){
        current_balance[q_accounts.value("id").toInt()] = q_accounts.value("opening_balance").toDouble();
        row++;
    }
    while (q_transactions.next()){
        current_balance[q_transactions.value("account_id").toInt()] += q_transactions.value("amount").toDouble();
    }
    q_accounts.seek(-1);
    table_accounts_->setRowCount(row);
    row = 0;

    while (q_accounts.next()){
        table_accounts_->setItem(row, 0, new QTableWidgetItem(q_accounts.value("id").toString()));
        table_accounts_->setItem(row, 1, new QTableWidgetItem(q_accounts.value("name").toString()));

        double value = q_accounts.value("opening_balance").toDouble();
        QString formatted = german.toString(value, 'f', 2);
        table_accounts_->setItem(row, 2, new QTableWidgetItem(formatted));

        value = current_balance[q_accounts.value("id").toInt()];
        formatted = german.toString(value, 'f', 2);
        table_accounts_->setItem(row, 3, new QTableWidgetItem(formatted));

        table_accounts_->setItem(row, 4, new QTableWidgetItem(currency));
        table_accounts_->setItem(row, 5, new QTableWidgetItem(q_accounts.value("created_at").toString().left(10)));
        row++;
    }
    table_accounts_->resizeColumnsToContents();
}

OverviewDialog::~OverviewDialog()
{
    delete ui;
}
