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
/**
 * @class IconPickerDialog
 * @brief Stellt eine Dialogoberfläche zur Auswahl von Symbolen für Kategorien bereit.
 *
 * Diese Klasse ermöglicht dem Benutzer, ein Symbol (Icon) aus einer vordefinierten Liste auszuwählen,
 * welches dann z. B. einer Kategorie oder einem Konto zugewiesen werden kann.
 *
 * Die Icons werden dynamisch aus einer Datenquelle geladen und in einem scrollbaren Raster angezeigt.
 * Die Auswahl wird dem aufrufenden Widget (z. B. einem QLabel) übergeben.
 *
 * Features:
 * - Anzeige von Icons mit IDs zur Auswahl
 * - Verknüpfung zur zentralen Zustandsverwaltung @class StateMgr
 * - Rückgabe der ausgewählten ID an das aufrufende Widget
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class CategoryEditDialog oder @class AccountDialog)
 * @param ptrStateMgr Zeiger auf das zentrale Zustandsobjekt (@class StateMgr)
 * @param lbl_icon Pointer auf ein Label der aufrufenden Parent-Instanz, das das aktuell gewählte Symbol nach erfolgter Auswahl anzeigt
 */

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
