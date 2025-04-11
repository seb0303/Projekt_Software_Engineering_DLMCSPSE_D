#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>

#include "constants.h"

namespace db {

    class Database : public QObject
    {
        Q_OBJECT
    public:
        explicit Database(QString path,
                          QString database_name,
                          bool passwordRequired,
                          QString password = "",
                          QString connection_name = "default",
                          QObject *parent = nullptr);

        // Public Methods
        bool connect();
        bool close();
        QString getFullPath();
        // bool create();
        QSqlQuery read(QString table, QString filter="");
        bool update_or_create_transaction(QMap<QString, QString> &form_data, QString type); // type = "update" ? Update : Create
        bool update_or_create_category(int id, int icon, QString name); // id = 0 ? Create : Update
        bool update_or_create_subCategory(int id, int parent_id, QString name); // id = 0 ? Create : Update
        bool update_or_create_account(QMap<QString, QVariant> &form_data);
        bool update_root(QMap<QString, QVariant> &form_data);
        bool del(QString table, QString filter);

        // static
        static bool createDb(const QMap<QString, QString> form_data);
        static bool createDbExecuteQueries_(QString& q);
        static bool createDbExecuteQueries_(QSqlQuery& q);

    private:
        QSqlDatabase db_;
        QString driver_;
        QString database_name_;
        QString connection_name_;
        QString password_;
        bool password_req_;
        QString path_;
        QString full_path_;
        void validateDatabase_();
    signals:
    };
}

#endif // DATABASE_H
