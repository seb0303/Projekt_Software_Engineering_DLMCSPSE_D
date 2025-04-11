#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QWidget>
#include <QTextBrowser>
#include <QFile>
#include <QString>
#include <QPushButton>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QWidget
{
    Q_OBJECT
    QTextBrowser* text_browser;
    QPushButton* pushButton_quit;

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
