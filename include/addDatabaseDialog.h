#ifndef ADDDATABASEDIALOG_H
#define ADDDATABASEDIALOG_H

#include <QWidget>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QRegularExpressionValidator>
#include <QLineEdit>
#include <QDebug>
#include <QPushButton>
#include <QMap>
#include <QPointer>

#include <exception>

#include "welcomescreenDialog.h"
#include "mainWindow.h"
#include "database.h"
#include "constants.h"
#include "init.h"

namespace Ui {
class AddDatabaseDialog;
}

class AddDatabaseDialog : public QWidget
{
    Q_OBJECT
    QComboBox* cbx_currency;
    QStringList currency;
    QLabel* lbl_currency;
    QLabel* lbl_passwordsDoNotMatch;
    QLineEdit* lineEdit_balance;
    QLineEdit* lineEdit_dbname;
    QLineEdit* lineEdit_password;
    QLineEdit* lineEdit_repeatPassword;
    QPushButton* btn_back;
    QPushButton* btn_submit;

    qsizetype current_password_length;

    QMap<QString, QString> form_data;

public:
    explicit AddDatabaseDialog(QWidget *parent = nullptr);
    ~AddDatabaseDialog();
    void btnClickEventBack();
    bool btnClickEventSubmit();
private:
    Ui::AddDatabaseDialog *ui;
    bool validateData_();
    void checkPasswordsAreEqual_();
    void checkPasswordsLength_();
};

#endif // ADDDATABASEDIALOG_H
