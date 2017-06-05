#ifndef NEWRUN_BUSWINDOW_H
#define NEWRUN_BUSWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "enhancetablewidget.h"

namespace Ui {
class NewRun_BusWindow;
}

class NewRun_BusWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewRun_BusWindow(QWidget *parent, QSqlDatabase db1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    ~NewRun_BusWindow();
    QDateTime start;
    int route_id;
    int servicebook_id;
    QList <int> drivers_id;

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

    void on_newrun_bus_runs_registered_hide_pushButton_clicked();

    void on_newrun_bussubmit_buttonBox_accepted();

    void on_newrun_bussubmit_buttonBox_rejected();

    void on_newrun_Buses_general_tableWidget_cellDoubleClicked(int row, int column);

private:
    QSqlDatabase db;
    Ui::NewRun_BusWindow *ui;

    void refresh_table_buses_registered(int bus_servicebook_id);

    void refresh_table_buses_avaliable_on_route_at_moment();
    void create_run();
};

#endif // NEWRUN_BUSWINDOW_H
