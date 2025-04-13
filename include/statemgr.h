#ifndef STATEMGR_H
#define STATEMGR_H
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QByteArray>
#include <QSvgRenderer>
#include <QSqlQuery>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QComboBox>
#include <QStringList>
#include <QDate>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPair>
#include <QString>

#include "database.h"
#include "constants.h"
#include "status_log.h"
#include "transactionDialog.h"

// Vorwärtsdeklaration
class MainWindow;

namespace core {
/**
 * @class StateMgr
 * @brief Verwaltet den globalen Zustand der Anwendung PennyWise.
 *
 * Die Klasse StateMgr kapselt alle Informationen und Operationen,
 * die für die Darstellung und Verarbeitung des aktuellen Zustands der Anwendung relevant sind.
 * Dazu gehören:
 * - Aktive Filtereinstellungen (z. B. Zeiträume, Konten, Kategorien)
 * - Verknüpfung zur aktuell geöffneten Datenbank
 * - Anbindung an das Hauptfenster (MainWindow) zur Aktualisierung der Benutzeroberfläche
 *
 * Die Klasse wird typischerweise beim Start der Anwendung initialisiert und bleibt während der gesamten Laufzeit aktiv.
 *
 *
 * @note Diese Klasse verwendet Qt-spezifische Datentypen und basiert auf QObject zur Integration in das Signal-Slot-System.
 *
 * @param parent Zeiger auf das übergeordnete QWidget (MainWindow)
 * @param db Zeiger auf die geöffnete Datenbankinstanz
 *
 * @see MainWindow, db::Database
 */

class StateMgr : public QObject
{
    Q_OBJECT
public:
    // Für die Initialisierung wird UI Widget und Datenbankinstanz benötigt
    StateMgr(QWidget *parent, db::Database *db);
    // Vorwärtsdeklaration für MainWindow um auf Logger zuzugreifen
    MainWindow* main_window;
    // Datenbankobjekt public
    db::Database* db;
    bool delete_status;
    // Wenn bestimmte Filterkriterien (Transaktionen, Balance) erforderlich sind muss vor Abruf der Datenbanktabellen
    // über die Settermethoden die jeweiligen Filter gesetzt werden.
    // Die Filter werden über die Objektauswahl in der UI gesetzt (Beispiel: User klickt auf TreeViewWidget mit
    // Accounts --> account_id_ wird in StateMgr gesetzt)

    QSqlQuery getCategories() const; // Kein Einfluss von StateObjekt, ruft konstant alle Kategorien ab
    QSqlQuery getSubCategories() const; // Kein Einfluss von StateObjekt, ruft konstant alle Kategorien ab
    QSqlQuery getAccounts() const; // Kein Einfluss von StateObjekt, ruft konstant alle Accounts ab
    QSqlQuery getTransactions(bool item = false, int id = 0) const; // Einfluss von StateObjekt(en) Zeitraum, Kategorie, Account
    QSqlQuery getBalance(bool all = false) const; // Einfluss von StateObjekt Account
    QString getCurrency() const; // Ruft die Datenbankwährung ab
    QSqlQuery getIcons() const; // Ruft alle Icons aus der Datenbank ab
    QSqlQuery getRoot() const;

    // Update
    bool setAll();
    bool setCategories(QSqlQuery& q);
    bool setAccounts(QSqlQuery& q);
    bool setTransactions(QSqlQuery& q);
    bool setBalance(QSqlQuery& q);
    bool setTableItem(QMap<QString, QString>& form_data); // neues TableItem setzen

    // StateMgmt Setter und Getter
    // **** Getter ****
    QPair<QDate, QDate> getTimePeriod() const;
    QString getCategoryType() const;
    int getCategoryId() const;
    QString getAccountType() const;
    int getAccountId() const;
    int getRootId() const;
    // **** Setter ****
    bool setTimePeriodParam(QDate& date_from, QDate& date_to);
    bool setCategoryTypeParam(QString& category_type);
    bool setCategoryIdParam(int& id);
    bool setAccountTypeParam(QString& account_type);
    bool setAccountIdParam(int& id);

    // ComboBox Update (Zeitraum Transaktionen)
    bool setTimePeriodFilter();

    // Tools
    QIcon convertSVGImageFromDb(const int& icon_index) const; // lädt SVG Images aus Datenbank und konvertiert in QIcon
signals:
    void refreshChart();
private:
    // Klassen über Konstruktor
    QWidget* parent_;
    // Private Properties für Setter und Getter
    QDate days_to_;
    QDate days_from_;
    QString category_type_;
    int category_id_;
    QString account_type_;
    int account_id_;
public slots:
    // Button Handler für Buttons in der Transaktionstabelle (löschen und bearbeiten)
    // Übergeben wird sowohl id für Datenbank als auch row für Tabellenwidget
    bool handleDeleteEvent(int id, int row);
    bool handleEditEvent(int id, int row);
    void confirmDeleteEvent(bool status);
};




// Benötigt für Delete und Edit Buttons im TableWidget
class TableActionButton : public QPushButton
{
    Q_OBJECT
private:
    int row_;
    int id_;
public:
    TableActionButton(QWidget* parent, int row, int id, QString object_name, QString toolTip);
    void setRow(int row);
    void setId(int id);
    int getRow() const;
    int getId() const;
    QString getObjectName() const;
signals:
    void buttonClicked(int id, int row); // Signal für Klick mit ID, Row

private slots:
    void emitButtonClicked(); // Slot zum Senden des Signals
};
}

#endif // STATEMGR_H
