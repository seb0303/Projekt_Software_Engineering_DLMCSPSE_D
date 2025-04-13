#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QIcon>
#include <QGroupBox>
#include <QList>
#include <QIcon>

namespace Ui {
class AccountDialog;
}
namespace core {
class StateMgr;
}
class IconPickerDialog;

class Account;
/**
 * @class AccountDialog
 * @brief Stellt einen Dialog zur Erstellung oder Bearbeitung von Bankkonten bereit.
 *
 * Diese Klasse bildet die Benutzerschnittstelle für das Erstellen, Bearbeiten und Validieren
 * von Konten (z. B. Girokonto, Sparkonto, Kreditkarte).
 *
 * Der Dialog sammelt Benutzerangaben wie Kontoname, Währung und Startsaldo
 * und übergibt sie an die Datenbanklogik (@class Account) zur Speicherung.
 *
 * Features:
 * - Eingabeformular für Kontodaten
 * - Fehlerprüfung und Feedback bei ungültigen Eingaben
 * - Integration in das zentrale Zustandsmanagement (`StateMgr`)
 *
 * @note Die UI ist in einer `.ui`-Datei gestaltet und wird zur Laufzeit geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class MainWindow)
 * @param stateMgr Zeiger (@class StateMgr) für die Verwaltung der UI und States
 */

class AccountDialog : public QWidget
{
    Q_OBJECT
    QWidget* parent_;
    core::StateMgr* ptrStateMgr_;
    Account* account_;
    IconPickerDialog* icon_picker_dialog_;

    QListWidget* listWidget_accounts_;
    QGroupBox* groupBox_accountData_;
    QLineEdit* lineEdit_name_;
    QLineEdit* lineEdit_accountHolder_;
    QLabel* label_currency_;
    QLabel* label_currency2_;
    QLineEdit* lineEdit_accountNumber_;
    QLineEdit* lineEdit_bankName_;
    QLineEdit* lineEdit_bankCode_;
    QLineEdit* lineEdit_iban_;
    QLineEdit* lineEdit_bic_;
    QLineEdit* lineEdit_openingBalance_;
    QLabel* label_icon_;
    QLabel* label_id_;

    QPushButton* pushButton_account_add_;
    QPushButton* pushButton_account_delete_;
    QPushButton* pushButton_changeIcon_;
    QPushButton* pushButton_save_;
    QPushButton* pushButton_quit_;

    QMap<int, QPair<QIcon, QString>> allAccountsMasterdata_;
    QMap<QString, QVariant> singleAccountMasterdata_;
    QList<QLineEdit*> allLineEdits_;
    QString operation_type_;
    int account_id_;
    int currency_id_;
    // checkt, ob der Nutzer etwas geändert hat. Wenn nicht, wird UI nach verlassen nicht aktualisiert.
    bool operation_done_;

public:
    explicit AccountDialog(QWidget *parent = nullptr, core::StateMgr *ptrStateMgr = nullptr);
    ~AccountDialog();
private:
    Ui::AccountDialog *ui;
    // Setzt die Stammdaten des jeweiligen Accounts in das Form ein
    void setSingleAccountMasterdata_(QMap<QString, QVariant> singleAccountMasterdata);
    // Entfernt alle Stammdaten aus dem Form um einen "New Account" Dialog zu öffnen
    void setNewAccountDialog_();
    // Ruft alle verfügbaren Accounts aus der Datenbank ab und schreibt die Daten in das ListWidget
    void getAllAccountsMasterdata_();
    // Ruft einen einzelnen Datensatz ab
    void getSingleAccountMasterdata_(int id);
    // Speichert den Datensatz in der Datenbank
    void saveData();
    // Löscht ein Konto
    void deleteAccount();
    // Generiert ein IconPickerWidget
    IconPickerDialog* generateIconWidget_();
};

#endif // ACCOUNTDIALOG_H
