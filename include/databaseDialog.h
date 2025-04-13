#ifndef DATABASEDIALOG_H
#define DATABASEDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QStringList>
#include <QMap>
#include <QVariant>

#include "constants.h"

namespace Ui {
class DatabaseDialog;
}
namespace core {
class StateMgr;
}
/**
 * @class DatabaseDialog
 * @brief Stellt einen Dialog zur Bearbeitung der aktuellen Datenbankeigenschaften bereit.
 *
 * Diese Klasse ermöglicht dem Benutzer, Metadaten der aktuell geöffneten Datenbank
 * (z. B. Name, Währung, Passwort) einzusehen und zu ändern. Sie bietet darüber hinaus
 * Funktionen zum Zurücksetzen des Passworts sowie zur Löschung der Datenbankdatei (nicht implementiert).
 *
 * Die Eingaben werden validiert und über @class StateMgr mit der zugrunde liegenden Datenbank synchronisiert.
 *
 * Features:
 * - Anzeigen von Metadaten wie Erstellungsdatum und Datenbank-ID
 * - Passwortänderung mit Validierung der Eingabefelder (nicht implementiert)
 * - Löschen der aktuellen Datenbankinstanz
 * - Auswahl einer neuen Standardwährung
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class MainWindow)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */

class DatabaseDialog : public QWidget
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QPushButton* pushButton_database_delete_;
    QPushButton* pushButton_quit_;
    QPushButton* pushButton_save_;
    QPushButton* pushButton_submitPassword_;
    QLabel* label_id_;
    QLabel* label_createdAt_;
    QLabel* label_passwordsDoNotMatch_;
    QLineEdit* lineEdit_name_;
    QLineEdit* lineEdit_passwordOld_;
    QLineEdit* lineEdit_passwordNew_;
    QLineEdit* lineEdit_passwordNewRepeat_;
    QComboBox* comboBox_currency_;

public:
    explicit DatabaseDialog(QWidget *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    ~DatabaseDialog();

private:
    Ui::DatabaseDialog *ui;
    void save_data_();
    void submit_passwort_change_();
    void delete_database_();
    bool checkPasswordsAreEqual_();
    bool checkPasswordLength_();
};

#endif // DATABASEDIALOG_H
