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
