#ifndef UNSCHEDULEDSTOP_H
#define UNSCHEDULEDSTOP_H

#include <QDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QtSql>
#include <QtDebug>
#include <QMessageBox>
#include "enhancetablewidget.h"

namespace Ui {
class UnscheduledStop;
}

class UnscheduledStop : public QDialog
{
    Q_OBJECT

public:
    QString coordinates;
    QString notes;
    explicit UnscheduledStop(QWidget *parent, QSqlDatabase db1);
    void prompt_error(QString text, bool exit_flag = false);
    int prompt_question(QString text);
    ~UnscheduledStop();

private slots:
    void on_unscheduled_stop_submit_buttonBox_accepted();

    void on_unscheduled_stop_submit_buttonBox_rejected();

    void on_unscheduled_stop_coordinates_comboBox_currentIndexChanged(int index);

    void on_unscheduled_stop_coordinates_comboBox_currentTextChanged(const QString &arg1);

private:
    QSqlDatabase db;
    Ui::UnscheduledStop *ui;
};

#endif // UNSCHEDULEDSTOP_H
