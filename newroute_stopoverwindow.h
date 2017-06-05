#ifndef NEWROUTE_STOPOVERWINDOW_H
#define NEWROUTE_STOPOVERWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QDateTime>
#include <QMessageBox>
#include "enhancetablewidget.h"

namespace Ui {
class NewRoute_StopoverWindow;
}

class NewRoute_StopoverWindow : public QDialog
{
    Q_OBJECT

public:
    QDateTime* parrival;
    QDateTime* pdepature;
    QString* coordinates;
    QString* notes;
    explicit NewRoute_StopoverWindow(QWidget *parent, QSqlDatabase db1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    ~NewRoute_StopoverWindow();

private slots:
    void on_newroute_newstopover_submit_buttonBox_accepted();

    void on_newroute_newstopover_submit_buttonBox_rejected();

    void on_newroute_newstopover_comboBox_currentIndexChanged(int index);

    void on_newroute_newstopover_comboBox_currentTextChanged(const QString &arg1);

    void on_newroute_newstopover_parrival_time_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);

private:
    QSqlDatabase db;
    Ui::NewRoute_StopoverWindow *ui;
};

#endif // NEWROUTE_STOPOVERWINDOW_H
