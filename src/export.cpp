#include "../include/export.h"
#include "../include/statemgr.h"

Export::Export(QObject *parent, core::StateMgr *ptrStateMgr)
    : QObject{parent}
    ,ptrStateMgr_(ptrStateMgr)
{}

void Export::exportTables(QSet<QString> &tables)
{
    for (auto i = tables.cbegin(), end = tables.cend(); i != end; ++i){
        QString table =  *i;
        QSqlQuery q = ptrStateMgr_->db->read(table);

        // Desktop Pfad initialisieren
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QWidget* parent = qobject_cast<QWidget*>(this);

        // Dateidialog Speichern-unter öffnen
        QString fileName = QFileDialog::getSaveFileName(
            parent,
            "CSV speichern",
            desktopPath + "/" + table + ".csv",
            "CSV Dateien (*.csv);;Alle Dateien (*)"
        );

        // Prüfen ob nichts ausgefüllt wurde
        if (fileName.isEmpty()) {
            qDebug() << "Export has been cancelled";
            return;
        }
        qDebug() << "Export" << table << "with filename" << fileName;

        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Could not open file" << file.errorString();
            return;
        }

        QTextStream out(&file);
        QSqlRecord record = q.record();
        int columnCount = record.count();

        // Spaltenüberschriften schreiben
        for (int i = 0; i < columnCount; ++i) {
            out << "\"" << record.fieldName(i) << "\"";
            if (i < columnCount - 1)
                out << ",";
        }
        out << "\n";

        // Datenzeilen schreiben
        while (q.next()) {
            for (int i = 0; i < columnCount; ++i) {
                out << "\"" << q.value(i).toString().replace("\"", "\"\"") << "\"";
                if (i < columnCount - 1)
                    out << ",";
            }
            out << "\n";
        }

        file.close();
    }
    QMessageBox::information(nullptr, "Hinweis", "Die Tabelle(n) wurde(n) erfolgreich exportiert");
}

void Export::exportDatabase()
{
    QString db_path = ptrStateMgr_->db->getFullPath();
    QFile db(db_path);
    qDebug() << "Selected database item for export" << db.fileName();

    if (!db.exists()) {
        qWarning() << "No such file.";
        return;
    }

    // Desktop-Pfad als Startordner
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QWidget* parent = qobject_cast<QWidget*>(this);

    // Speichern unter... Dialog öffnen
    QString savePath = QFileDialog::getSaveFileName(
        parent,
        "Datenbank exportieren als...",
        desktopPath + "/" + QFileInfo(db).fileName(),
        "PennyWise Datenbanken (*.pwdb);;Alle Dateien (*)"
        );

    if (savePath.isEmpty()) {
        qDebug() << "Export has been cancelled";
        return;
    }

    // Datei kopieren
    if (QFile::copy(db_path, savePath)) {
        qDebug() << "Db copied to" << savePath;
        QMessageBox::information(nullptr, "Hinweis", "Die Datenbank wurde erfolgreich exportiert");
    } else {
        qWarning() << "Error copying database: " << db.errorString();
    }
}
