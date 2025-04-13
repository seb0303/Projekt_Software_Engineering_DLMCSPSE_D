#ifndef WELCOMESCREENDIALOG_H
#define WELCOMESCREENDIALOG_H

#include <QCoreApplication>
#include <QWidget>
#include <QPushButton>
#include <QFrame>
#include <QDebug>
#include <QMessageBox>
#include <QPointer>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QDebug>

#include "addDatabaseDialog.h"
#include "mainWindow.h"
#include "constants.h"

namespace Ui {
class WelcomeScreen;
}
/**
 * @class WelcomeScreen
 * @brief Startbildschirm der Anwendung zur Auswahl oder Erstellung einer Datenbank.
 *
 * Diese Klasse stellt die erste Benutzeroberfläche nach dem Start von PennyWise dar.
 * Der Benutzer hat hier die Möglichkeit, eine neue Datenbank zu erstellen, eine vorhandene zu öffnen
 * oder eine Demo-Datenbank zu laden.
 *
 * Der Dialog dient als Einstiegspunkt in die Anwendung und leitet bei Bedarf zu @class AddDatabaseDialog
 * oder @class MainWindow weiter. Er verwaltet außerdem die Dateiauswahl über den Qt-Dateidialog.
 *
 * Features:
 * - Öffnen bestehender PennyWise-Datenbanken über Dateiauswahl
 * - Erstellen neuer Datenbanken über einen separaten Dialog
 * - Starten eines Demomodus mit vorgefüllter Datenbank
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (nullptr oder QApplication-Instanz)
 */

class WelcomeScreen : public QWidget
{
    Q_OBJECT
    QPushButton* btn_addDb_;
    QPushButton* btn_startDemo_;
public:
    explicit WelcomeScreen(QWidget *parent = nullptr);
    ~WelcomeScreen();

    // fügt einen Dialog hinzu, wenn es bereits Datenbanken im System gibt
    void showOpenDbDialog();
private:
    Ui::WelcomeScreen *ui;
    void btnClickEventAddDb();
    void btnClickEventStartDemo();
    void btnClickEventOpenDb();
};

#endif // WELCOMESCREENDIALOG_H
