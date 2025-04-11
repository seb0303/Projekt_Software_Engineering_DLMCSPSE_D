#include "../include/category.h"
#include "../include/statemgr.h"
#include "../include/categoryEditDialog.h"
#include "../include/categoryDialog.h"

struct Categories {
    QIcon icon;
    QString name;
    QString type;
};

struct SubCategories {
    QString name;
    int parent_id;
};

Category::Category(QWidget *parent, core::StateMgr* ptrStateMgr)
    : QObject{parent}
    , ptrStateMgr_(ptrStateMgr)
{
    parent_ = parent;
    connect(this, &Category::confirmDeleteStatus, ptrStateMgr, &core::StateMgr::confirmDeleteEvent);
}

// Lädt die Hauptkategorien und gibt die Daten als QMap zurück
// int = Datensatz-ID, Categories = struct mit Datensätzen zur ID
QMap<int, Categories> Category::getCategories() const
{
    QMap<int, Categories> out;

    QSqlQuery q = this->ptrStateMgr_->getCategories();

    while(q.next()){
        const int icon_id = q.value("icon").toInt();
        const int i = q.value("id").toInt();

        Categories el{
            ptrStateMgr_->convertSVGImageFromDb(icon_id),
            q.value("name").toString(),
            q.value("type").toString()
        };

        out[i] = el;
    }

    if (constants::DEBUG){
        qDebug() << "Categories loaded:";
        for (int key: out.keys()){
            QString name = out[key].name;
            qDebug() << "Key:" << key << "Name:" << name;
        }
    }

    return out;
}

// Lädt die Unterkategorien und gibt die Daten als QMap zurück
// int = Datensatz-ID, SubCategories = struct mit Datensätzen zur ID
QMap<int, SubCategories> Category::getSubCategories() const
{
    QMap<int, SubCategories> out;

    QSqlQuery q = this->ptrStateMgr_->getSubCategories();

    while (q.next()){
        const int i = q.value("id").toInt();

        SubCategories el{
            q.value("name").toString(),
            q.value("parent_group_id").toInt()
        };

        out[i] = el;
    }

    return out;
}

void Category::addItem(QString &type)
{
    qDebug() << "Category::addItem slot called with type:" << type;
    CategoryEditDialog* category_widget = generateCategoryWidget_();
    if (type == "category"){
        category_widget->openNewDialog();
    }
    else if (type == "subCategory"){
        QSqlQuery q = ptrStateMgr_->getCategories();
        QMap<int, QPair<QIcon, QString>> data;
        QPair<QIcon, QString> inner;
        while (q.next()){
            inner.first = ptrStateMgr_->convertSVGImageFromDb(q.value("icon").toInt());
            inner.second = q.value("name").toString();
            data[q.value("id").toInt()] = inner;
        }
        category_widget->openNewDialog(data);
    }

}
void Category::editItem(QString &type, int& id)
{
    qDebug() << "Category::editItem slot called with type:" << type << "and id:" << id;
    CategoryEditDialog* category_widget = generateCategoryWidget_();
    QSqlQuery q = ptrStateMgr_->getCategories();
    bool status = false;
    // Kategorie bearbeiten
    // Wird aufgerufen, wenn Nutzer auf Button "edit" im Bereich der Hauptkategorien klickt
    if (type == "category") {
        QIcon icon;
        QString name;
        int icon_id;
        while (q.next()){
            if (q.value("id").toInt() == id){
                icon_id = q.value("icon").toInt();
                icon = ptrStateMgr_->convertSVGImageFromDb(icon_id);
                name = q.value("name").toString();
                status = true;
                break;
            }
        }
        if (!status) return;
        category_widget->openEditDialog(id, icon_id, icon, name);
    }
    // Unterkategorien bearbeiten
    // Wird aufgerufen, wenn Nutzer auf Button "edit" im Bereich der Unterkategorien klickt
    else if (type == "subCategory"){
        QSqlQuery sub = ptrStateMgr_->getSubCategories();
        QString name;
        int parent_id;
        while (sub.next()){
            int sub_id = sub.value("id").toInt();
            if (id == sub_id){
                name = sub.value("name").toString();
                parent_id = sub.value("parent_group_id").toInt();
                status = true;
                break;
            }
        }
        if (!status) return;
        QMap<int, QPair<QIcon, QString>> data;
        QPair<QIcon, QString> inner;
        while (q.next()){
            inner.first = ptrStateMgr_->convertSVGImageFromDb(q.value("icon").toInt());
            inner.second = q.value("name").toString();
            data[q.value("id").toInt()] = inner;
        }
        category_widget->openEditDialog(id, name, data, parent_id);
    }
}
void Category::deleteItem(QString &type, int& id)
{
    QString table;
    QString filter = "id = " + QString::number(id);
    bool resp;

    if (type == "category"){
        table = constants::TABLE_CATEGORIES;
    }
    else if (type == "subCategory"){
        table = constants::TABLE_SUBCATEGORIES;
    }
    resp = ptrStateMgr_->db->del(table, filter);
    emit confirmDeleteStatus(resp);
}

CategoryEditDialog *Category::generateCategoryWidget_()
{
    category_edit_dialog_ = new CategoryEditDialog(parent_, ptrStateMgr_);
    category_edit_dialog_->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    category_edit_dialog_->setWindowModality(Qt::WindowModal);
    category_edit_dialog_->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    category_edit_dialog_->setWindowTitle("Eigenschaften");
    return category_edit_dialog_;
}
