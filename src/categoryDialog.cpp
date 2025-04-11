#include "../include/categoryDialog.h"
#include "../ui/ui_categoryDialog.h"

#include "../include/statemgr.h"
#include "../include/category.h"

struct Categories {
    QIcon icon;
    QString name;
    QString type;
};

struct SubCategories {
    QString name;
    int parent_id;
};

CategoryDialog::CategoryDialog(QWidget *parent, core::StateMgr* ptrStateMgr)
    : QWidget(parent)
    , ui(new Ui::CategoryDialog)
    , ptrStateMgr_(ptrStateMgr)
{
    ui->setupUi(this);

    // Sicherheitsprüfung
    if (!parent || !ptrStateMgr) {
        QMessageBox::information(nullptr, "Hinweis", "Kategorie Dialog konnte nicht geladen werden");
        this->close();
        this->deleteLater();
    }

    // ComboBox (Hauptkategorie) und ListWidget (Unterkategorie) initialisieren
    cbx_categories_ = this->ui->comboBox_category;
    listWidget_sub_category = this->ui->listWidget_sub_category;
    listWidget_sub_category->clear();

    // Buttons initialisieren
    pushButton_quit_ = this->ui->pushButton_quit;
    pushButton_category_add_ = this->ui->pushButton_category_add;
    pushButton_category_edit_ = this->ui->pushButton_category_edit;
    pushButton_category_delete_ = this->ui->pushButton_category_delete;
    pushButton_subCategory_add_ = this->ui->pushButton_subCategory_add;
    pushButton_subCategory_edit_ = this->ui->pushButton_subCategory_edit;
    pushButton_subCategory_delete_ = this->ui->pushButton_subCategory_delete;

    // Neue Instanz für Kategorieklasse
    category_ = new Category(this, ptrStateMgr_);

    // Hauptkategorien abfragen und setzen
    QMap<int, Categories> categories_data = category_->getCategories();
    this->refreshCategories_(categories_data);

    // Unterkategorien abfragen, nicht setzen sondern als Property stehen lassen
    // Wichtig: bei Änderungen aktualisieren!!
    subCategories_data_ = category_->getSubCategories();

    // connect Funktion wenn Index der Kategorie geändert wird sollen alle zugehörigen Unterkategorien geladen werden
    connect(cbx_categories_, &QComboBox::currentIndexChanged, this, &CategoryDialog::handleCategoryChangeEvent);
    // connect für quit
    connect(pushButton_quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });
    // connect für AddDialog Category
    connect(pushButton_category_add_, &QPushButton::clicked, this, [this](){
        this->onOpenAddDialogClicked(pushButton_category_add_);
    });
    // connect für EditDialog mit Parameter Category
    connect(pushButton_category_edit_, &QPushButton::clicked, this, [this](){
        this->onOpenEditDialogClicked(pushButton_category_edit_);
    });
    // connect für DeleteDialog mit Parameter Category
    connect(pushButton_category_delete_, &QPushButton::clicked, this, [this](){
        this->onOpenDeleteDialogClicked(pushButton_category_delete_);
    });
    // connect für AddDialog SubCategory
    connect(pushButton_subCategory_add_, &QPushButton::clicked, this, [this](){
        this->onOpenAddDialogClicked(pushButton_subCategory_add_);
    });
    // connect für EditDialog mit Parameter SubCategory
    connect(pushButton_subCategory_edit_, &QPushButton::clicked, this, [this](){
        this->onOpenEditDialogClicked(pushButton_subCategory_edit_);
    });
    // connect für DeleteDialog mit Parameter SubCategory
    connect(pushButton_subCategory_delete_, &QPushButton::clicked, this, [this](){
        this->onOpenDeleteDialogClicked(pushButton_subCategory_delete_);
    });
    //connect für Class "Category" / Add Dialog
    connect(this, &CategoryDialog::openAddDialog, category_, &Category::addItem);
    //connect für Class "Category" / Edit Dialog
    connect(this, &CategoryDialog::openEditDialog, category_, &Category::editItem);
    //connect für Class "Category" / Delete Dialog
    connect(this, &CategoryDialog::openDeleteDialog, category_, &Category::deleteItem);

    // Connect für ComboBox auslösen
    cbx_categories_->setCurrentIndex(-1);
    cbx_categories_->setCurrentIndex(0);

    // Operation Done? setzen
    // wenn true wird beim close Event die UI aktualisiert (zeitaufwendig)
    // wenn false wird der Dialog einfach geschlossen
    operation_done = false;
}

CategoryDialog::~CategoryDialog()
{
    delete ui;
}

void CategoryDialog::refreshCategories_(QMap<int, Categories> &data)
{
    qDebug() << "Refresh Categories called";
    this->cbx_categories_->clear();
    for (auto [key, value] : data.asKeyValueRange()) {
        this->cbx_categories_->addItem(value.icon, value.name);
        cbx_categories_->setItemData(cbx_categories_->count()-1, key, Qt::UserRole);
    }
}

void CategoryDialog::refreshSubCategories_(QMap<int, SubCategories> &data)
{
    this->listWidget_sub_category->clear();
    const int parent_id = this->cbx_categories_->currentData(Qt::UserRole).toInt();

    for (auto [key, value] : data.asKeyValueRange()) {
        if (value.parent_id == parent_id){
            QIcon icon(":/icons/static/img/tag.svg");
            QListWidgetItem* el = new QListWidgetItem(icon, value.name);
            el->setData(Qt::UserRole, key);
            el->setData(Qt::UserRole + 1, value.parent_id);
            this->listWidget_sub_category->addItem(el);
        }
    }
}

void CategoryDialog::handleCategoryChangeEvent()
{
    subCategories_data_ = category_->getSubCategories();
    this->refreshSubCategories_(subCategories_data_);
}

void CategoryDialog::onOpenAddDialogClicked(QPushButton *elem)
{
    QString object_name = elem->objectName();
    QString type;
    if (object_name == "pushButton_category_add"){
        type = "category";
    }
    else if (object_name == "pushButton_subCategory_add"){
        type = "subCategory";
    }
    else {
        qDebug() << "Error in Slot CategoryDialog::onOpenAddDialogClicked; Wrong type specifier. Submitted object:" << object_name;
        return;
    }
    emit openAddDialog(type);
}

void CategoryDialog::onOpenEditDialogClicked(QPushButton *elem)
{
    QString object_name = elem->objectName();
    QString type;
    int id;
    if (object_name == "pushButton_category_edit"){
        type = "category";
        id = cbx_categories_->currentData(Qt::UserRole).toInt();
        if (id == 0 || cbx_categories_->currentIndex() == -1){
            QMessageBox::information(nullptr, "Hinweis", "Keine gültige Kategorie vorhanden. Bitte lege zunächst eine neue Kategorie an");
            return;
        }
    }
    else if (object_name == "pushButton_subCategory_edit"){
        type = "subCategory";
        QListWidgetItem* item = listWidget_sub_category->currentItem();
        if (!item) {
            QMessageBox::information(nullptr, "Hinweis", "Bitte wähle eine Unterkategorie aus der Liste aus.");
            return; // Prüfung zur Vermeidung von nullptr
        }

        id = item->data(Qt::UserRole).toInt();
    }
    else {
        qDebug() << "Error in Slot CategoryDialog::onOpenEditDialogClicked; Wrong type specifier. Submitted object:" << object_name;
        return;
    }
    if (id == 1){
        QMessageBox::information(nullptr, "Hinweis", "'Unkategorisiert' kann nicht bearbeitet werden, da es sich um einen Default Parameter handelt. Bitte wähle einen anderen Eintrag aus der Liste.");
        return;
    }
    emit openEditDialog(type, id);
}

void CategoryDialog::onOpenDeleteDialogClicked(QPushButton *elem)
{
    QString object_name = elem->objectName();
    QString type;
    int id;
    if (object_name == "pushButton_category_delete"){
        type = "category";
        id = cbx_categories_->currentData(Qt::UserRole).toInt();
        if (id == 0 || cbx_categories_->currentIndex() == -1){
            QMessageBox::information(nullptr, "Hinweis", "Keine gültige Kategorie vorhanden. Bitte lege zunächst eine neue Kategorie an");
            return;
        }
    }
    else if (object_name == "pushButton_subCategory_delete"){
        type = "subCategory";
        QListWidgetItem* item = listWidget_sub_category->currentItem();
        if (!item) {
            QMessageBox::information(nullptr, "Hinweis", "Bitte wähle eine Unterkategorie aus der Liste aus.");
            return; // Prüfung zur Vermeidung von nullptr
        }
        id = item->data(Qt::UserRole).toInt();
    }
    else {
        qDebug() << "Error in Slot CategoryDialog::onOpenEditDialogClicked; Wrong type specifier. Submitted object:" << object_name;
        return;
    }
    // Löschung von 'Unkategorisiert' verhindern (default)
    if (id == 1){
        QMessageBox::information(nullptr, "Hinweis", "'Unkategorisiert' kann nicht gelöscht werden da es sich um einen Default Parameter handelt. itte wähle einen anderen Eintrag aus der Liste.");
        return;
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle("Löschen bestätigen");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Möchtest du diesen Datensatz wirklich löschen? Datensätze, die diese Kategorie referenzieren werden als 'Unkategorisiert' überschrieben.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int result = msgBox.exec();
    if (result == QMessageBox::Yes){
        emit openDeleteDialog(type, id);
        // Prüfen ob erfolgreich
        bool resp = this->ptrStateMgr_->delete_status;
        if (!resp){
            QMessageBox::information(nullptr, "Hinweis", "Fehler beim Löschen des Datensatzes");
            return;
        }
        QMessageBox::information(nullptr, "Hinweis", "Datensatz wurde gelöscht");
        // Kategorien und Unterkategorien aktualisieren
        QMap<int, Categories> categories_data = category_->getCategories();
        this->refreshCategories_(categories_data);
        this->subCategories_data_ = category_->getSubCategories();
        // Connect Methode auslösen
        this->cbx_categories_->setCurrentIndex(-1);
        this->cbx_categories_->setCurrentIndex(0); // Index des Elements setzen
        operation_done = true;
    }

}

// handleCategoryEditEvent und handleSubCategoryEditEvent werden von categoryEditDialog aufgerufen wenn:
// Case 1 -> eine neue Kategorie angelegt wurde
// Case 2 -> eine bestehende Kategorie bearbeitet wurde
// Case 3 -> eine neue Unterkategorie angelegt wurde
// Case 4 -> eine bestehende Unterkategorie bearbeitet wurde
void CategoryDialog::handleCategoryEditEvent(QString type, QIcon icon, QString name, int id)
{
    // Eine neue Kategorie wurde angelegt
    if (type == "new"){
        QMap<int, Categories> categories_data = category_->getCategories(); // Alle Kategorien laden
        this->refreshCategories_(categories_data); // Alle Kategorien aktualisieren
        this->subCategories_data_ = category_->getSubCategories(); // Unterkategorien laden
        this->cbx_categories_->setCurrentIndex(this->cbx_categories_->count()-1); // Index auf das neue Element setzen
    }
    // Eine bestehende Kategorie wurde bearbeitet
    else if (type == "edit"){
        // cbx_categories_->setItemData(cbx_categories_->count()-1, key, Qt::UserRole);
        // Iteration um den Index mit der richtigen Id zu finden
        // ist sicherer als nur das aktuell ausgewählte Element anzupassen
        for (int i = 0; i < this->cbx_categories_->count(); i++){
            int cbx_item_id = this->cbx_categories_->itemData(i, Qt::UserRole).toInt();
            if (cbx_item_id == id){
                this->cbx_categories_->setItemText(i, name); // neuen Namen setzen
                this->cbx_categories_->setItemIcon(i, icon); // neues Icon setzen
                this->subCategories_data_ = category_->getSubCategories(); // Unterkategorien laden
                this->cbx_categories_->setCurrentIndex(i); // Index des Elements setzen
                break; // Schleife abbrechen
            }
        }
    }
}
void CategoryDialog::handleSubCategoryEditEvent(int parent_id)
{
    // Eine neue Unterkategorie wurde angelegt oder eine bestehende bearbeitet
    this->subCategories_data_ = category_->getSubCategories(); // Unterkategorien laden
    // Iteration bis das Parent Element gefunden wurde
    for (int i = 0; i < this->cbx_categories_->count(); i++){
        int cbx_item_id = this->cbx_categories_->itemData(i, Qt::UserRole).toInt();
        if (cbx_item_id == parent_id){
            // Connect Methode auslösen
            this->cbx_categories_->setCurrentIndex(-1);
            this->cbx_categories_->setCurrentIndex(i); // Index des Elements setzen
            break; // Schleife abbrechen
        }
    }
}

void CategoryDialog::checkDeleteStatus(bool status)
{
    qDebug() << "Object has been deleted" << status;
}

void CategoryDialog::closeEvent(QCloseEvent *event)
{
    qDebug() << "Categories closed";
    // UI aktualisieren
    if (operation_done){
        QApplication::setOverrideCursor(Qt::WaitCursor);
        this->ptrStateMgr_->setAll();
        QApplication::processEvents();
        QApplication::restoreOverrideCursor();
    }
}

