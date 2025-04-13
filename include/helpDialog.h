#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QWidget>
#include <QTextBrowser>
#include <QFile>
#include <QString>
#include <QPushButton>

namespace Ui {
class HelpDialog;
}
/**
 * @class HelpDialog
 * @brief Stellt einen Dialog mit Informationen und Hilfe zur Anwendung bereit.
 *
 * Diese Klasse zeigt dem Benutzer eine statisch geladene Hilfedatei
 * (HTML) im Dialogfenster an. Sie dient zur Erläuterung der Bedienoberfläche,
 * Funktionen und Nutzungshinweise von PennyWise.
 *
 * Features:
 * - Darstellung formatierter Hilfeinhalte in einem QTextBrowser
 * - Schließen über einfachen Button
 *
 * @note Die grafische Oberfläche ist in einer `.ui`-Datei gestaltet und wird dynamisch geladen.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (@class MainWindow)
 */

class HelpDialog : public QWidget
{
    Q_OBJECT
    QTextBrowser* text_browser;
    QPushButton* pushButton_quit;

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
