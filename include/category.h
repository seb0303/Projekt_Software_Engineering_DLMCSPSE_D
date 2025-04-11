#ifndef CATEGORY_H
#define CATEGORY_H

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QIcon>
#include <QString>
#include <QMap>
#include <QSqlQuery>
#include <QPair>

#include "constants.h"

namespace core {
class StateMgr;
}

class CategoryEditDialog;
class CategoryDialog;

struct Categories;
struct SubCategories;

class Category : public QObject
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QWidget* parent_;
    CategoryEditDialog* category_edit_dialog_;
public:
    explicit Category(QWidget *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    QMap<int, Categories> getCategories() const;
    QMap<int, SubCategories> getSubCategories() const;
public slots:
    void addItem(QString& type);
    void editItem(QString& type, int& id);
    void deleteItem(QString& type, int& id);
private:
    CategoryEditDialog* generateCategoryWidget_();
signals:
    void confirmDeleteStatus(bool status);

};

#endif // CATEGORY_H
