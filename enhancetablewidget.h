#ifndef ENHANCETABLEWIDGET_H
#define ENHANCETABLEWIDGET_H
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QHeaderView>
#include <QLabel>
#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQueryModel>
#include <QtDebug>
#include <QMessageBox>

void config_default_behavior(QTableWidget* table);
bool fill_table_with_query(QTableWidget* table, QString query);
int fill_combobox_with_query(QComboBox* cb, QString query);
int fill_lb_with_query(QLabel* lb,QString query);


#endif // ENHANCETABLEWIDGET_H
