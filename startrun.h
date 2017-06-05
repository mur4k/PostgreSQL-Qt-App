#ifndef STARTRUN_H
#define STARTRUN_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "enhancetablewidget.h"

namespace Ui {
class StartRun;
}

class StartRun : public QDialog
{
    Q_OBJECT

public:
    explicit StartRun(QWidget *parent, QSqlDatabase db1, int run_id1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    int passangers_attended;
    ~StartRun();

private slots:
    void on_start_run_submit_buttonBox_accepted();

    void on_start_run_submit_buttonBox_rejected();

private:
    Ui::StartRun *ui;
    QSqlDatabase db;
    int run_id;
};

#endif // STARTRUN_H
