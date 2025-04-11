#include "../include/search.h"

Search::Search(QObject *parent)
    : QObject{parent}
{}

void Search::handleSearchEvent(QString query, QTableView *ptrTableView)
{
    QAbstractItemModel* table_model = ptrTableView->model();
    qDebug() << query << query.isEmpty();
    // Kein Suchstring?
    if (query.isEmpty()){
        for (int row = 0; row < table_model->rowCount(); ++row) {
            ptrTableView->setRowHidden(row, false);
        }
        return;
    }

    query = query.toLower();
    bool match = false;
    for (int row = 0; row < table_model->rowCount(); ++row) {
        for (int col = 0; col < table_model->columnCount(); ++col){
            QModelIndex index = table_model->index(row, col);
            QString data = table_model->data(index).toString();
            if (data.toLower().contains(query)){
                match = true;
            }
        }
        if (!match) ptrTableView->setRowHidden(row, true);
        match = false;
    }
}

