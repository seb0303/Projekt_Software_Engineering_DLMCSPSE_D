#ifndef TRANSACTIONDIALOG_H
#define TRANSACTIONDIALOG_H

#include <QWidget>
#include <QDate>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QRegularExpression>
#include <QLabel>
#include <QPushButton>
#include <QHash>
#include <QPair>
#include <QSqlQuery>

#include "transaction.h"
#include "constants.h"

namespace Ui {
class TransactionDialog;
class mainWindow;
}
namespace core {
class StateMgr;
}

class TransactionDialog : public QWidget
{
    Q_OBJECT
    Transaction* ptrTransaction;
    core::StateMgr* ptrStateMgr_;
    int table_row;
    QSqlQuery data;

    QLabel* lbl_header;
    QLabel* lbl_currency;
    QLabel* lbl_id;
    QLabel* lbl_type;

    QDateEdit* dateEdit_date;

    QLineEdit* lineEdit_amount;
    QLineEdit* lineEdit_recipient;
    QTextEdit* textEdit_purpose;

    QPushButton* btn_submit;
    QPushButton* btn_cancel;

    QMap<int, QPair<QString, QString>> categories;
    QMap<int, QPair<int, QString>> sub_categories;
    QPair<int, QString> currency;
    QMap<int, QString> accounts;

    QComboBox* comboBox_categories;
    QComboBox* comboBox_sub_categories;
    QComboBox* comboBox_accounts;

    QMap<QString, QString> form_data;

public:
    explicit TransactionDialog(QWidget *parent = nullptr, const int id = 0, const int row = -1, core::StateMgr* ptrStateMgr = nullptr);
    ~TransactionDialog();

private:
    Ui::TransactionDialog *ui;
    void handleCategoryCbxChangeEvent_();
    bool handleSubmitEvent_();
};

#endif // TRANSACTIONDIALOG_H
