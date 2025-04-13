#ifndef CATEGORYEDITDIALOG_H
#define CATEGORYEDITDIALOG_H

#include <QWidget>
#include <QString>
#include <QIcon>
#include <QMap>
#include <QPair>

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>

namespace Ui {
class CategoryEditDialog;
}

class IconPickerDialog;
class CategoryDialog;
namespace core {
class StateMgr;
}
/**
 * @class CategoryDialog
 * @brief Stellt ein Dialogfenster zur Anzeige und Verwaltung von Kategorien bereit.
 *
 * Diese Klasse stellt die Benutzeroberfläche für das Anlegen, Bearbeiten und Löschen
 * von Kategorien und Subkategorien bereit, die für die Organisation von Transaktionen verwendet werden.
 *
 * Der Dialog ist in Tabs und Listen untergliedert und kommuniziert mit der
 * Logikklasse @class Category zur Durchführung von Aktionen.
 *
 * Features:
 * - Auswahl bestehender Kategorien
 * - Bearbeitung und Hierarchisierung über Subkategorien
 * - Integration in das zentrale Zustandsmanagement @class StateMgr
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class MainWindow)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */

class CategoryEditDialog : public QWidget
{
    Q_OBJECT
    QWidget* parent_;
    QString type_;
    QLabel* lbl_header_;
    QLabel* lbl_icon_;
    QComboBox* cbx_categories_;
    QLineEdit* txb_name_;
    QPushButton* btn_changeIcon_;
    QPushButton* btn_submit_;
    QPushButton* btn_quit_;

    QGroupBox* grBox_name_;
    QGroupBox* grBox_categories_;
    QGroupBox* grBox_icon_;

    int current_id_;
    int current_parent_id_;

    IconPickerDialog* icon_picker_dialog_;
    core::StateMgr* ptrStateMgr_;

public:
    explicit CategoryEditDialog(QWidget *parent = nullptr, core::StateMgr *ptrStateMgr = nullptr);
    void openEditDialog(int id, int icon_id, QIcon icon, QString name); // Für Category
    void openEditDialog(int id, QString name, QMap<int, QPair<QIcon, QString>> data, int parent_id); // Für SubCategory
    void openNewDialog(); // Für Category
    void openNewDialog(QMap<int, QPair<QIcon, QString>> data); // Für SubCategory
    ~CategoryEditDialog();

private:
    Ui::CategoryEditDialog *ui;
    IconPickerDialog* generateIconWidget_();
    void onSubmitButtonClicked_();
};

#endif // CATEGORYEDITDIALOG_H
