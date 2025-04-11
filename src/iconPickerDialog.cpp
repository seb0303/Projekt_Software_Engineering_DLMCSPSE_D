#include "../include/iconPickerDialog.h"
#include "../ui/ui_iconPickerDialog.h"
#include "../include/statemgr.h"

IconPickerDialog::IconPickerDialog(QWidget *parent, core::StateMgr* ptrStateMgr, QLabel* lbl_icon)
    : QWidget(parent)
    , ui(new Ui::IconPickerDialog)
{
    ui->setupUi(this);
    ptrStateMgr_ = ptrStateMgr;
    lbl_icon_ = lbl_icon;

    // Sicherheitsprüfung
    if (!parent || !ptrStateMgr || !lbl_icon){
        QMessageBox::information(nullptr, "Hinweis", "Icon Dialog konnte nicht geladen werden");
        this->close();
        this->deleteLater();
    }

    prev_selected_item_id_ = 0;
    selected_item_id_ = 0;

    QWidget* scrollAreaWidgetContents_ = this->ui->scrollAreaWidgetContents;
    QVBoxLayout* layout_ = new QVBoxLayout(scrollAreaWidgetContents_);

    submit_ = this->ui->pushButton_save;
    quit_ = this->ui->pushButton_quit;

    connect(quit_, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });

    connect(submit_, &QPushButton::clicked, this, [this](){
        this->onSubmitButtonClicked_();
    });

    layout_->setContentsMargins(3,3,3,3);
    layout_->setSpacing(0);

    // Icons aus der Datenbank abrufen und in Map übertragen
    QSqlQuery q = ptrStateMgr_->getIcons();
    while (q.next()) {
        QIcon icon = ptrStateMgr_->convertSVGImageFromDb(q.value("id").toInt());
        data_[q.value("id").toInt()] = icon;
    }

    int counter = 0;
    QFrame* frame_icon_row_ = new QFrame(this);
    QHBoxLayout* layout_icon_row_ = new QHBoxLayout(frame_icon_row_);

    for (int i : data_.keys()) {
        QIcon icon = data_[i];
        QPushButton* btn_icon = new QPushButton(icon, "", frame_icon_row_);
        btn_icon->setIconSize(QSize(30,30));
        btn_icon->setProperty("class", "btn_icon");
        btn_icon->setObjectName("btn_icon");
        btn_icon->setFixedSize(50, 50);
        btn_icon->setProperty("id", i);
        btn_icon->setCursor(Qt::PointingHandCursor);

        connect(btn_icon, &QPushButton::clicked, this, [this, i](){
            this->setSelectedItemId(i);
        });
        layout_icon_row_->addWidget(btn_icon);
        counter++;

        // Containergröße erreicht?
        if (counter % constants::ICON_DEFAULT_CONTAINER_SIZE == 0) {
            layout_->addWidget(frame_icon_row_); // Füge das fertige Frame zur ScrollArea hinzu

            // Neuen Frame + Layout für nächste Zeile vorbereiten
            if (counter != constants::ICON_MAX_DATASETS) {
                frame_icon_row_ = new QFrame(this);
                layout_icon_row_ = new QHBoxLayout(frame_icon_row_); // Layout direkt übergeben
            }
        }
    }

    // Falls noch verbleibende Icons vorhanden sind, letzte Zeile hinzufügen
    if (counter % constants::ICON_DEFAULT_CONTAINER_SIZE != 0) {
        layout_->addWidget(frame_icon_row_);
    }

    scrollAreaWidgetContents_->setLayout(layout_);
    QList<QPushButton*> buttons_ = this->findChildren<QPushButton*>("btn_icon");
    for (QPushButton* button: buttons_){
        int btn_id = button->property("id").toInt();
        buttons_hash_[btn_id] = button;
    }
}


IconPickerDialog::~IconPickerDialog()
{
    delete ui;
}

void IconPickerDialog::setSelectedItemId(int id)
{
    this->prev_selected_item_id_ = this->selected_item_id_;
    this->selected_item_id_ = id;

    if (selected_item_id_ != 0){
        this->buttons_hash_[selected_item_id_]->setStyleSheet("background: lightblue;");
    }
    if (prev_selected_item_id_ != 0){
        this->buttons_hash_[prev_selected_item_id_]->setStyleSheet("background: #F4F4F4;");
    }
}

int IconPickerDialog::getSelectedItemId() const
{
    return this->selected_item_id_;
}

void IconPickerDialog::onSubmitButtonClicked_()
{
    int id = this->getSelectedItemId();
    if (id == 0) {
        QMessageBox::information(nullptr, "Hinweis", "Bitte wähle einen Eintrag aus.");
        return;
    }
    QIcon icon = ptrStateMgr_->convertSVGImageFromDb(id);
    this->lbl_icon_->setPixmap(icon.pixmap(50,50));
    this->lbl_icon_->setProperty("id", id);

    this->close();
    this->deleteLater();
}
