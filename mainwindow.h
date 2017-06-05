#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "loginwindow.h"
#include "enhancetablewidget.h"
#include "newroute.h"
#include "newrun_buswindow.h"
#include "newrun_driverswindow.h"
#include "newrun_routewindow.h"
#include "startrun.h"
#include "unscheduledstop.h"
#include "ncreport.h"
#include "ncreportoutput.h"
#include "ncreportpreviewoutput.h"
#include "ncreportpreviewwindow.h"



namespace Ui {
class MainWindow;
}

//class that manages all forms and shows all necessary tables
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    ~MainWindow();

private slots:

    void on_exit_pushButton_clicked();

    void on_stop_info_hide_pushButton_clicked();

    void on_route_info_hide_pushButton_clicked();

    void on_new_route_pushButton_clicked();

    void on_Routes_general_tableWidget_cellDoubleClicked(int row, int column);

    void on_new_run_pushButton_clicked();

    void on_drivers_runs_registered_hide_pushButton_clicked();


    void on_bus_runs_registered_hide_pushButton_clicked();

    void refresh_table_drivers();
    void refresh_table_runs();
    void refresh_table_routes();
    void refresh_table_buses();
    void refresh_table_runs_stops(int run_id);
    void refresh_table_buses_registered(int bus_servicebook_id);
    void refresh_table_drivers_registered(int driver_id);


    void on_Buses_general_tableWidget_cellDoubleClicked(int row, int column);

    void on_Drivers_general_tableWidget_cellDoubleClicked(int row, int column);

    void on_Runs_general_tableWidget_cellDoubleClicked(int row, int column);

    void on_stop_accept_pushButton_clicked();

    void on_stop_end_pushButton_clicked();

    void on_additional_stop_pushButton_clicked();

    void on_start_run_pushButton_clicked();

    void on_report_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QString login;
    QString password;
    QString ip;
    QString port;

    //role info
    QString role;
    QString getRole();

    int current_run;


    void set_output_format();

    void change_observer_mode(bool state);

    void create_db_connection();
};

#endif // MAINWINDOW_H
