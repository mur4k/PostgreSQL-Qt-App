#ifndef NEWRUN_DRIVERSWINDOW_H
#define NEWRUN_DRIVERSWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "enhancetablewidget.h"

namespace Ui {
class NewRun_DriversWindow;
}

class NewRun_DriversWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewRun_DriversWindow(QWidget *parent, QSqlDatabase db1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    QDateTime start;
    int route_id;
    int servicebook_id;
    QList <int> drivers_id;
    ~NewRun_DriversWindow();

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
    void on_newrun_submitdrivers_buttonBox_accepted();

    void on_newrun_submitdrivers_buttonBox_rejected();

    void on_newrun_add_driver_pushButton_clicked();

    void on_newrun_remove_driver_pushButton_clicked();

    void on_newrun_Drivers_general_tableWidget_cellDoubleClicked(int row, int column);

    void on_newrun_drivers_runs_registered_hide_pushButton_clicked();

private:
    QSqlDatabase db;
    Ui::NewRun_DriversWindow *ui;

    void refresh_table_drivers_registered(int driver_id);

    void refresh_table_drivers_avaliable_on_route_at_moment();
    void create_run();

};

#endif // NEWRUN_DRIVERSWINDOW_H
