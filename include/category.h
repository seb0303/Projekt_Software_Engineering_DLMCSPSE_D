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
/**
 * @class Category
 * @brief Modelliert Finanzkategorien und Subkategorien in PennyWise.
 *
 * Diese Klasse kapselt alle Logiken zur Verwaltung von Kategorien und Subkategorien,
 * die für das Sortieren und Klassifizieren von Transaktionen verwendet werden.
 *
 * Die Klasse stellt Methoden zum Abrufen, Bearbeiten und Löschen von Kategorien bereit.
 * Dabei wird sowohl mit Haupt- als auch mit Unterkategorien gearbeitet.
 * Zusätzlich steuert sie die Interaktion mit dem zugehörigen Bearbeitungsdialog.
 *
 * Features:
 * - Abruf aller Kategorien und Subkategorien
 * - Editieren und Löschen über zugewiesene Dialoge
 * - Signal für Löschbestätigung an UI
 *
 * @note Die Klasse ist mit Qt-Signals/Slots ausgestattet und nutzt `QMap`, `QPair`, `QVector`.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class CategoryDialog)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 */

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
