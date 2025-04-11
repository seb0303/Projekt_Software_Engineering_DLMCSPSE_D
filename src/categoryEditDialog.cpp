#include "../include/categoryEditDialog.h"
#include "../ui/ui_categoryEditDialog.h"

#include "../include/iconPickerDialog.h"
#include "../include/statemgr.h"
#include "../include/categoryDialog.h"

CategoryEditDialog::CategoryEditDialog(QWidget *parent, core::StateMgr *ptrStateMgr)
    : QWidget(parent)
    , ptrStateMgr_(ptrStateMgr)
    , ui(new Ui::CategoryEditDialog)
{
    ui->setupUi(this);
    parent_ = parent;
    lbl_header_ = this->ui->label_header;
    lbl_icon_ = this->ui->label_icon;
    cbx_categories_ = this->ui->comboBox_categories;
    txb_name_ = this->ui->lineEdit_name;
    btn_changeIcon_ = this->ui->pushButton_editIcon;
    btn_submit_ = this->ui->pushButton_save;
    btn_quit_ = this->ui->pushButton_quit;

    grBox_name_ = this->ui->groupBox_name;
    grBox_categories_ = this->ui->groupBox_categories;
    grBox_icon_ = this->ui->groupBox_icon;

    this->setFixedSize(300,250);

    current_id_ = 0;
    current_parent_id_ = 0;

    connect(btn_changeIcon_, &QPushButton::clicked, this, [this](){
        this->setCursor(Qt::WaitCursor);
        QApplication::processEvents();
        IconPickerDialog* w = this->generateIconWidget_();
        this->unsetCursor();
        w->show();
    });

    connect(btn_quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });

    connect(btn_submit_, &QPushButton::clicked, this, [this](){
        this->onSubmitButtonClicked_();
    });
}

// Category Edit
void CategoryEditDialog::openEditDialog(int id, int icon_id, QIcon icon, QString name)
{
    // Current Id setzen
    current_id_ = id;
    this->type_ = "category";
    // Kategorie-GroupBox ausblenden
    grBox_categories_->hide();
    lbl_icon_->setPixmap(icon.pixmap(40,40));
    lbl_icon_->setProperty("id", icon_id);
    txb_name_->setText(name);
    this->show();
}
// SubCategory Edit
void CategoryEditDialog::openEditDialog(int id, QString name, QMap<int, QPair<QIcon, QString> > data, int parent_id)
{   this->type_ = "subCategory";
    // Current Id setzen
    current_id_ = id;
    // Parent Id setzen
    current_parent_id_ = parent_id;
    txb_name_->setText(name);
    // Icon Group Box ausblenden
    grBox_icon_->hide();
    qDebug() << "SubCategory Edit called with id" << current_id_ << ", name" << name << "and parent" << current_parent_id_;
    for (QMap<int, QPair<QIcon, QString>>::const_iterator i = data.constBegin(); i != data.constEnd(); ++i) {
        QPair<QIcon, QString> inner = i.value();
        cbx_categories_->addItem(inner.first, inner.second);
        int current_size = cbx_categories_->count() -1;
        cbx_categories_->setItemData(current_size, i.key(), Qt::UserRole);
        if (i.key() == current_parent_id_){
            cbx_categories_->setCurrentIndex(current_size);
        }
    }
    this->show();
}
// Category new
void CategoryEditDialog::openNewDialog()
{
    // Kategorie-GroupBox ausblenden
    grBox_categories_->hide();
    this->type_ = "category";
    QIcon icon(":/icons/static/img/img_not_found.svg");
    lbl_icon_->setPixmap(icon.pixmap(40,40));
    lbl_icon_->setProperty("id", 0);
    this->show();
}
// SubCategory new
void CategoryEditDialog::openNewDialog(QMap<int, QPair<QIcon, QString> > data)
{
    lbl_icon_->setText("");
    grBox_icon_->hide();
    this->type_ = "subCategory";
    for (QMap<int, QPair<QIcon, QString>>::const_iterator i = data.constBegin(); i != data.constEnd(); ++i) {
        QPair<QIcon, QString> inner = i.value();
        cbx_categories_->addItem(inner.first, inner.second);
        int current_size = cbx_categories_->count() -1;
        cbx_categories_->setItemData(current_size, i.key(), Qt::UserRole);
    }
    this->show();

}

CategoryEditDialog::~CategoryEditDialog()
{
    delete ui;
}

IconPickerDialog *CategoryEditDialog::generateIconWidget_()
{
    // Hier aufgehört am 29.03!!!
    // To Do - Im StateMgr einen Icon Abruf einbauen der ein SqlQuery zurückgibt.
    // Danach konvertieren in QMap<int, QIcon> (also mit StateMgr convertToSVG
    // und übergeben an icon_picker_dialog
    icon_picker_dialog_ = new IconPickerDialog(parent_, ptrStateMgr_, lbl_icon_);
    icon_picker_dialog_->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    icon_picker_dialog_->setWindowModality(Qt::WindowModal);
    icon_picker_dialog_->setAttribute(Qt::WA_DeleteOnClose);  // Löscht sich selbst nach dem Schließen
    icon_picker_dialog_->setWindowTitle("Icons");
    return icon_picker_dialog_;
}

void CategoryEditDialog::onSubmitButtonClicked_()
{

    QString name = txb_name_->text();
    bool resp = false;
    int icon_id;
    int category_id;
    // Validierung
    if (name == ""){
        QMessageBox::information(nullptr, "Hinweis", "Bitte vergib einen Namen für das Objekt");
        return;
    }
    // Logik
    if (this->type_ == "category"){
        icon_id = lbl_icon_->property("id").toInt();
        if (!icon_id) icon_id = constants::ICON_DEFAULT;
        qDebug() << "Selected Category" << this->current_id_ << "with Icon" << icon_id;
        resp = this->ptrStateMgr_->db->update_or_create_category(this->current_id_, icon_id, name);
    }
    else if (this->type_ == "subCategory"){
        category_id = cbx_categories_->currentData(Qt::UserRole).toInt();
        qDebug() << "Selected SubCategory" << this->current_id_ << "with Parent Id" << this->current_parent_id_ << "and new parent" << category_id;
        this->current_parent_id_ = category_id; // Überschreiben für Datenbankupdate
        resp = this->ptrStateMgr_->db->update_or_create_subCategory(this->current_id_, this->current_parent_id_, name);
    }

    // Prüfung ob erfolgreich in Datenbank hinzugefügt
    // Falls nicht erscheint MessageBox, der Dialog wird aber nicht beendet
    if (!resp){
        QMessageBox::information(nullptr, "Hinweis", "Der Datensatz konnte nicht hinzugefügt werden.");
        return;
    }

    // Hier müsste jetzt noch rein, dass die Kategorien in dem CategoryDialog angepasst werden
    // also ein neuer Kategorienamen oder ein neues Icon muss im CategoryDialog aktualisiert werden
    // oder die Liste mit Unterkategorien angepasst werden...
    qDebug() << "CategoryEditDialog parent is:" << this->parent();
    // Prüfung ob nullptr
    if (this->parent()){
        CategoryDialog* category_dialog = qobject_cast<CategoryDialog*>(this->parent_);
        // Case 1
        // Neue Kategorie wurde angelegt
        // Kenzeichnung: type = category und current_id = 0
        if (this->type_ == "category" && this->current_id_ == 0){
            category_dialog->handleCategoryEditEvent("new");
        }
        // Case 2
        // Kategorie wurde bearbeitet
        // Kenzeichnung: type = category und current_id != 0
        if (this->type_ == "category" && this->current_id_ != 0){
            QIcon icon = this->ptrStateMgr_->convertSVGImageFromDb(icon_id);
            category_dialog->handleCategoryEditEvent("edit", icon, name, this->current_id_);
        }
        // Case 3 und 4
        // Unterkategorie wurde angelegt oder eine bestehende bearbeitet
        // Kenzeichnung: type = subCategory
        if (this->type_ == "subCategory"){
            category_dialog->handleSubCategoryEditEvent(this->current_parent_id_ );
        }
        category_dialog->operation_done = true;
    }

    this->close();
    this->deleteLater();
}
