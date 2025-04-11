#ifndef WELCOMESCREENDIALOG_H
#define WELCOMESCREENDIALOG_H

#include <QCoreApplication>
#include <QWidget>
#include <QPushButton>
#include <QFrame>
#include <QDebug>
#include <QMessageBox>
#include <QPointer>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QDebug>

#include "addDatabaseDialog.h"
#include "mainWindow.h"
#include "constants.h"

namespace Ui {
class WelcomeScreen;
}

class WelcomeScreen : public QWidget
{
    Q_OBJECT
    QPushButton* btn_addDb_;
    QPushButton* btn_startDemo_;
public:
    explicit WelcomeScreen(QWidget *parent = nullptr);
    ~WelcomeScreen();

    // fügt einen Dialog hinzu, wenn es bereits Datenbanken im System gibt
    void showOpenDbDialog();
private:
    Ui::WelcomeScreen *ui;
    void btnClickEventAddDb();
    void btnClickEventStartDemo();
    void btnClickEventOpenDb();
};

#endif // WELCOMESCREENDIALOG_H
