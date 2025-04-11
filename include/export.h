#ifndef EXPORT_H
#define EXPORT_H

#include <QObject>
#include <QWidget>
#include <QSet>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>

namespace core {
class StateMgr;
}

class Export : public QObject
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
public:
    explicit Export(QObject *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
signals:
public slots:
    void exportTables(QSet<QString>& tables);
    void exportDatabase();
};

#endif // EXPORT_H
