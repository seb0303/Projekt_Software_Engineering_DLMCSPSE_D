#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include <QWidget>
#include <QIcon>
#include <QComboBox>
#include <QMap>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>

namespace core {
class StateMgr;
}

namespace Ui {
class CategoryDialog;
}

class Category;

struct Categories;

struct SubCategories;

class CategoryDialog : public QWidget
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QComboBox* cbx_categories_;
    QListWidget* listWidget_sub_category;
    Category* category_;
    QMap<int, SubCategories> subCategories_data_;

    QPushButton* pushButton_quit_;
    QPushButton* pushButton_category_add_;
    QPushButton* pushButton_category_edit_;
    QPushButton* pushButton_category_delete_;
    QPushButton* pushButton_subCategory_add_;
    QPushButton* pushButton_subCategory_edit_;
    QPushButton* pushButton_subCategory_delete_;

public:
    explicit CategoryDialog(QWidget *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    bool operation_done;
    ~CategoryDialog();

private:
    Ui::CategoryDialog *ui;
    void refreshCategories_(QMap<int, Categories>& data);
    void refreshSubCategories_(QMap<int, SubCategories>& data);
public slots:
    void handleCategoryChangeEvent();
    void onOpenAddDialogClicked(QPushButton* elem);
    void onOpenEditDialogClicked(QPushButton* elem);
    void onOpenDeleteDialogClicked(QPushButton* elem);
    // handleCategoryEditEvent und handleSubCategoryEditEvent werden von categoryEditDialog aufgerufen wenn:
    // Case 1 -> eine neue Kategorie angelegt wurde
    // Case 2 -> eine bestehende Kategorie bearbeitet wurde
    void handleCategoryEditEvent(QString type, QIcon icon = {}, QString name = "", int id = 0);
    // Case 3 -> eine neue Unterkategorie angelegt wurde
    // Case 4 -> eine bestehende Unterkategorie bearbeitet wurde
    void handleSubCategoryEditEvent(int parent_id);
    void checkDeleteStatus(bool status); // prüft, ob die Löschung einer Kategorie oder Unterkategorie erfolgreich war
signals:
    void openAddDialog(QString& type);
    void openEditDialog(QString& type, int& id);
    void openDeleteDialog(QString& type, int& id);
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // CATEGORYDIALOG_H
