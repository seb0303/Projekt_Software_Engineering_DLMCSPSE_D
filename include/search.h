#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QTableView>
#include <QAbstractItemModel>
#include <QString>
#include <QMap>

class Search : public QObject
{
    Q_OBJECT
public:
    explicit Search(QObject *parent = nullptr);

public slots:
    void handleSearchEvent(QString query, QTableView* ptrTableView);
};

#endif // SEARCH_H
