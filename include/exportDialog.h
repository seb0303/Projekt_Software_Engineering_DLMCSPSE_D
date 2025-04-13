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
/**
 * @class ExportDialog
 * @brief Stellt einen Dialog zur Auswahl und Durchführung von Datenexporten bereit.
 *
 * Diese Klasse stellt die Benutzeroberfläche für den Export von Datenbanktabellen oder der gesamten Datenbank bereit.
 * Der Benutzer kann gezielt auswählen, welche Daten (z. B. Konten, Kategorien, Transaktionen) exportiert werden sollen.
 * Die Auswahl wird an die @class Export-Logik übergeben, welche den eigentlichen Exportvorgang durchführt.
 *
 * Features:
 * - Checkbox-Auswahl für Tabellenexport (Accounts, Kategorien, Subkategorien, Transaktionen)
 * - Start des Exportvorgangs per Button
 * - Signale zur Steuerung von `Export`-Methoden
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class MainWindow)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */

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
