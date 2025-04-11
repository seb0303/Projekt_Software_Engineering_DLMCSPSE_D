#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSet>
#include <QString>
#include <QMessageBox>

#include "constants.h"

namespace Ui {
class ExportDialog;
}
namespace core {
class StateMgr;
}
class Export;

class ExportDialog : public QWidget
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QPushButton* pushButton_quit_;
    QPushButton* pushButton_export_;
    QCheckBox* checkBox_accounts_;
    QCheckBox* checkBox_categories_;
    QCheckBox* checkBox_subCategories_;
    QCheckBox* checkBox_transactions_;
    QSet<QString> export_selection_;
    Export* export_;

public:
    explicit ExportDialog(QWidget *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    ~ExportDialog();

private:
    Ui::ExportDialog *ui;
private slots:
    void handleExport_();
signals:
    void exportData(QSet<QString>& tables);
    void exportDb();
};

#endif // EXPORTDIALOG_H
