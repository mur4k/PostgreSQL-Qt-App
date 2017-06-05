#include "enhancetablewidget.h"

void config_default_behavior(QTableWidget* table) {
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->verticalHeader()->setVisible(true);
    table->setCurrentCell(0,0);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    for (int i = 0; i < table->horizontalHeader()->count(); ++i){
        table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        table->setSortingEnabled(true);
//        table->connect(table->horizontalHeader(),SIGNAL(table->horizontalHeader()->sortIndicatorChanged(i,Qt::SortOrder::AscendingOrder)), table ,SLOT(table->sortByColumn(i,Qt::SortOrder::AscendingOrder)));
//        table->connect(table->horizontalHeader(),SIGNAL(table->horizontalHeader()->sortIndicatorChanged(i,Qt::SortOrder::DescendingOrder)), table ,SLOT(table->sortByColumn(i,Qt::SortOrder::DescendingOrder)));
    }

}

bool fill_table_with_query(QTableWidget* table, QString query) {
    table->clearContents();
    table->setRowCount(0);
    QSqlQuery sql_query;
    sql_query.prepare(query);
    sql_query.exec();

    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
        return false;
    }

    else {
        int n = table->columnCount();
        table->setRowCount(sql_query.size());
        sql_query.first();
        int row = 0;
        do {
            for (int i = 0; i < n; i++){
                QTableWidgetItem* item = new QTableWidgetItem();
                item->setData(Qt::DisplayRole, sql_query.value(i));
                table->setItem(row, i, item);
            }
            row++;
        } while (sql_query.next());
        return true;
    }
}

int fill_combobox_with_query(QComboBox* cb, QString query){
    cb->clear();
    QSqlQuery sql_query;
    sql_query.prepare(query);
    sql_query.exec();

    cb->clear();
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
        return 1;
    }

    else {
        sql_query.first();
        do {
            cb->addItem(sql_query.value(0).toString());
        } while (sql_query.next());
        return 0;
    }
}

int fill_lb_with_query(QLabel* lb,QString query){
    lb->clear();
    QSqlQuery sql_query;
    sql_query.prepare(query);
    sql_query.exec();
    sql_query.first();
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
        return 1;
    }

    else {
        lb->setText(sql_query.value(0).toString());
        return 0;
    }
}
