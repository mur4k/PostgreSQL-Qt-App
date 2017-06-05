#ifndef NEWROUTE_H
#define NEWROUTE_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "enhancetablewidget.h"
#include "newroute_stopoverwindow.h"

namespace Ui {
class NewRoute;
}

class NewRoute : public QDialog
{
    Q_OBJECT

public:
    explicit NewRoute(QWidget *parent,QSqlDatabase db1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    ~NewRoute();

private slots:
    void on_newroute_submit_buttonBox_accepted();

    void on_newroute_submit_buttonBox_rejected();

    void on_newroute_new_stopover_pushButton_clicked();

    void on_newroute_Route_stops_tableWidget_itemSelectionChanged();

    void on_newroute_stopover_delete_pushButton_clicked();

private:
    QSqlDatabase db;
    Ui::NewRoute *ui;
    int insert_stopover(QString* coordinates,QString* notes,QDateTime* parrival,QDateTime* pdepature);
};

#endif // NEWROUTE_H
