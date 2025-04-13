#ifndef INIT_H
#define INIT_H

#include <QString>
#include <QDir>
#include <filesystem>

#include "../include/constants.h"

namespace initApp {
// Aktuell nicht in Verwendung
// checkInit() hat ursprünglich die Aufgabe zu prüfen, ob bereits Datenbanken angelegt wurden oder nicht
// Falls ja, soll der "Datenbank öffnen" Button im WelcomeScreen angezeigt werden
// Diese Logik wurde aber deaktiviert, da ein Nutzer sonst bei Neuinstallation der Anwendung bspw. keine Backup-Datenbanken öffnen kann,
// ohne zuvor eine neue Datenbank angelegt zu haben
    bool checkInit();
}


#endif // INIT_H
