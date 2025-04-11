#ifndef ICONPICKERDIALOG_H
#define ICONPICKERDIALOG_H

#include <QWidget>
#include <QMap>
#include <QIcon>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QScrollArea>

#include "constants.h"

namespace Ui {
class IconPickerDialog;
}

namespace core {
class StateMgr;
}
class IconPickerDialog : public QWidget
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QMap<int, QIcon> data_;
    int selected_item_id_;
    int prev_selected_item_id_;
    QHash<int, QPushButton*> buttons_hash_;
    QPushButton* submit_;
    QPushButton* quit_;
    QLabel* lbl_icon_;

public:
    explicit IconPickerDialog(QWidget *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr, QLabel* lbl_icon = nullptr);
    ~IconPickerDialog();
    void setSelectedItemId(int id);
    int getSelectedItemId() const;

private:
    Ui::IconPickerDialog *ui;
    void onSubmitButtonClicked_();

};

#endif // ICONPICKERDIALOG_H
