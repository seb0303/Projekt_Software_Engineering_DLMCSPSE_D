#include "../include/init.h"

namespace fs = std::filesystem;

// Prüft, ob bereits eine Datenbank angelegt wurde
// Falls ja, wird false zurückgegeben (init = false, der Welcome Screen wird nicht angezeigt)
// Falls nein, wird true zurückgegeben (--> Welcome Screen wird angezeigt)
bool initApp::checkInit(){
    // Funktion überschrieben, Datenbank öffnen Dialog wird immer angezeigt
    // Hintergrund: wenn App neu installiert wird und ein Backup vorhanden ist würde sonst kein "Öffnen Dialog" erscheinen
    // Code ggf. noch anpassen bzw. init Funktion löschen
    return false;

    // try {
    //     QString root = ".";
    //     QString db_path = constants::DB_PATH;
    //     QString path = root + "/" + db_path;


    //     // Abfangen von Fehler, falls Pfad nicht verfügbar --> Anwendung wird beendet
    //     if (!fs::exists(path.toStdString()) || !fs::is_directory(path.toStdString())) {
    //         throw std::runtime_error("Could not resolve path: " + path.toStdString());
    //     }

    //     // Prüfen, ob Datenbankfiles .pwdb (keine Verzeichnisse) in Pfad verfügbar sind
    //     for (const auto& item : fs::directory_iterator(path.toStdString())) {
    //         if (fs::is_regular_file(item.path())) {
    //             if (item.path().extension() == (constants::DB_FILE_EXT).toStdString()){
    //                 return false;
    //             }
    //         }
    //     }
    //     return true;
    // }
    // catch (const fs::filesystem_error& e) {
    //     throw std::runtime_error("Filesystem error: " + std::string(e.what()));
    // }
}
