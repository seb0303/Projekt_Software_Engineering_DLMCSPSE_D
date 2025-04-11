#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QObject>
#include <QMap>
#include <QPair>
#include <QDebug>
#include <QSqlQuery>
#include <QString>

namespace core {
class StateMgr;  // Vorwärtsdeklaration
}

class Transaction : public QObject
{
    Q_OBJECT
    core::StateMgr* ptrStateMgr_;
    QPair<bool, QString> validateData_(QMap<QString, QString>& form_data);
public:
    explicit Transaction(QObject *parent = nullptr, core::StateMgr* ptrStateMgr = nullptr);
    QMap<int, QPair<QString, QString>> getCategories();
    QMap<int, QPair<int, QString>> getSubCategories();
    QPair<int, QString> getCurrency();
    QMap<int, QString> getAccounts();
    QSqlQuery getTransaction(const int id);

public slots:
    QPair<bool, QString> save_data(QMap<QString, QString>& form_data, const int row = 0);
};

#endif // TRANSACTION_H
