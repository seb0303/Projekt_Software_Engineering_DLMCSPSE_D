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
/**
 * @class AddDatabaseDialog
 * @brief Stellt ein Eingabeformular zur Erstellung einer neuen Datenbank bereit.
 *
 * Diese Klasse bildet die Benutzerschnittstelle zum Erstellen einer neuen PennyWise-Datenbankdatei
 * inklusive Startkonto, Währungsauswahl und optionalem Passwortschutz (Verschlüsselung noch nicht implementiert).
 *
 * Der Dialog sammelt Benutzereingaben wie Datenbankname, Passwort und Startsaldo
 * und übergibt sie an die Initialisierungslogik der Anwendung. Zusätzlich wird die Datenbankweite Währung konfiguriert.
 *
 * Features:
 * - Überprüfung der Passwortfelder auf Übereinstimmung
 * - Dropdown-Menü zur Währungsauswahl
 * - Validierung von Startsaldo und Datenbanknamen
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *       Die erzeugten Datenbanken werden als SQLite-Dateien gespeichert.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (nullptr -> kein Parent, Speichermanagement manuell)
 */

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
