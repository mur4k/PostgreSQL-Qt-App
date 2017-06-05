#ifndef NEWRUN_ROUTEWINDOW_H
#define NEWRUN_ROUTEWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "enhancetablewidget.h"
#include "newroute.h"

namespace Ui {
class NewRun_RouteWindow;
}

class NewRun_RouteWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewRun_RouteWindow(QWidget *parent, QSqlDatabase db1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    QDateTime start;
    int route_id;
    int servicebook_id;
    QList <int> drivers_id;
    ~NewRun_RouteWindow();

public slots:
  void setValues(int route_value, QDateTime start_value);
  void setBus(int bus_value);
  void setDrivers(QList<int> drivers_values);
  void createRun();

signals:
  void valuesChanged(int new_route_value, QDateTime new_start_value);
  void busChanged(int new_bus_value);
  void driversChanged(QList<int> new_drivers_values);
  void runCreated();

private slots:
    void on_newrun_route_info_hide_pushButton_clicked();

    void on_newrun_Routes_general_tableWidget_cellDoubleClicked(int row, int column);

    void on_newrun_new_route_pushButton_clicked();

    void on_newrun_buttonBox_rejected();

    void on_newrun_buttonBox_accepted();

private:
    Ui::NewRun_RouteWindow *ui;
    QSqlDatabase db;
    void refresh_table_routes();
};

#endif // NEWRUN_ROUTEWINDOW_H
