#ifndef STATUS_LOG_H
#define STATUS_LOG_H
#include <string>
#include <QString>
#include <QLabel>
#include <QDebug>

/**
 * @class Status_Log
 * @brief Verwaltet und zeigt Statusnachrichten im Hauptfenster der Anwendung PennyWise an.
 *
 * Die Klasse `Status_Log` dient der strukturierten Anzeige von Statusmeldungen im UI,
 * z. B. nach erfolgreichen Operationen, bei Warnungen oder Fehlern.
 * Sie kapselt dabei unterschiedliche Nachrichtentypen und übergibt diese formatiert an ein `QLabel`.
 *
 * Diese Art von Rückmeldung wird vor allem im @class MainWindow genutzt,
 * um Benutzer über den Status ihrer Aktionen (z. B. Speichern, Löschen, Fehler) zu informieren.
 *
 * Features:
 * - Verwaltung unterschiedlicher Nachrichtentypen (Error, Warning, Success, Refresh)
 * - Anzeige einer formatierten Nachricht in einem gegebenen Qt-Label
 * - Fehlerrobuste Methode (noexcept), um UI-Rückmeldung unabhängig von der Logik zu ermöglichen
 *
 * @note Error wird derzeit nicht verwendet, kritische Fehlermeldungen werden über QMessageBox behandelt.
 *
 * @param target Zeiger auf das `QLabel`, in dem die Statusmeldung angezeigt werden soll
 * @param type Typ der Nachricht: `"error"`, `"warning"`, `"success"`, `"refresh"`
 * @param message Die darzustellende Nachricht in der UI
 */

class Status_Log
{
private:
    QString error_;
    QString warning_;
    QString success_;
    QString refresh_;
public:
    Status_Log();
    void print_status(QLabel* target, const std::string type, const std::string message) noexcept;
};

#endif // STATUS_LOG_H
