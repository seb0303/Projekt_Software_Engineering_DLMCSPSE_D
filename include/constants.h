#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QDate>

/**
 * @namespace constants
 * @brief Stellt zentrale, global verfügbare Konstanten für die PennyWise-Anwendung bereit.
 *
 * Dieser Namensraum enthält anwendungsweite Konstanten, die z. B. für Datenbankpfade,
 * Dateiendungen, Tabellennamen, Konfigurationsdateien, Debug-Modi und Standardwerte
 * verwendet werden.
 *
 * Die Trennung der Konstanten in diesen Namensraum sorgt für eine klare Struktur,
 * Wiederverwendbarkeit und einfache Wartung.
 *
 * Features:
 * - Pfade zur Standard- und Demo-Datenbank
 * - Dateierweiterungen und Treiberkonfiguration
 * - Tabellen- und Attributnamen für SQLite
 * - Defaultwerte wie Standardwährung, Startsaldo, Zeitfenster
 *
 */

namespace constants {

    constexpr bool DEBUG = false;

    // Datenbankkonstanten
    inline const QString ROOT = QStringLiteral(".");
    inline const QString DB_PATH = QStringLiteral("db/resources");
    inline const QString DB_PATH_DEMO = QStringLiteral("db/demo");
    inline const QString DB_DEMO_NAME = QStringLiteral("demo");
    inline const QString DB_DRIVER = QStringLiteral("QSQLITE");
    inline const QString DB_CONFIG = QStringLiteral("db/config/sqlite_build.ddl");
    inline const QString DB_ICONS_DATA = QStringLiteral("db/config/icons.csv");
    inline const QString DB_FILE_EXT = QStringLiteral(".pwdb");

    // Datenbanktabellen
    inline const QString TABLE_ACCOUNTS = QStringLiteral("Accounts");
    inline const QString TABLE_ICONS = QStringLiteral("Icons");
    inline const QString TABLE_CATEGORIES = QStringLiteral("Categories_group");
    inline const QString TABLE_SUBCATEGORIES = QStringLiteral("Categories");
    inline const QString TABLE_TRANSACTIONS = QStringLiteral("Transactions");
    inline const QString TABLE_ROOT = QStringLiteral("Root");

    // DEFAULTS
    constexpr int ICON_DEFAULT = 88; // Rotes SVG Label
    inline const QString CURRENCY_DEFAULT = QStringLiteral("EUR"); // Euro Label falls Fehler in Db
    constexpr double START_BALANCE_DEFAULT = 0; // Startsaldo
    inline const QString CATEGORY_DEFAULT = QStringLiteral("Unkategorisiert");
    inline const QString ACCOUNT_DEFAULT = QStringLiteral("none");
    constexpr int DATE_STRING_LENGTH_DEFAULT = 19;
    inline const QDate MAX_DATE_FROM(1900,1,1); // Maximales Abrufdatum
    constexpr int TIME_PERIOD_DAYS_DEFAULT = 90; // Standardmäßig werden die letzten 90 Tage in der Transaktionstabelle abgerufen
    inline const QString ACCOUNT_LABEL = QStringLiteral("Alle Konten");
    inline const QString CATEGORY_TYPE_DEFAULT = "all";
    constexpr int CATEGORY_ID_DEFAULT = 0;
    inline const QString ACCOUNT_TYPE_DEFAULT = "all";
    constexpr int ACCOUNT_ID_DEFAULT = 0;
    constexpr int ICON_DEFAULT_CONTAINER_SIZE = 12; // 12 Icons je Kategorie mit insgesamt 16 Icons also 192 Auswahlmöglichkeiten
    constexpr int ICON_MAX_DATASETS = 192;
    constexpr int ROOT_ID = 1;

}
#endif // CONSTANTS_H
