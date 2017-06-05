#include "newroute_stopoverwindow.h"
#include "ui_newroute_stopoverwindow.h"

NewRoute_StopoverWindow::NewRoute_StopoverWindow(QWidget *parent,QSqlDatabase db1) :
    QDialog(parent),
    ui(new Ui::NewRoute_StopoverWindow)
{
    ui->setupUi(this);
    this->db = db1;
    fill_combobox_with_query(ui->newroute_newstopover_comboBox,"SELECT DISTINCT p.\"Coordinates\" FROM busrunsdb.\"Place\" p");
    this->show();
}

NewRoute_StopoverWindow::~NewRoute_StopoverWindow()
{
    delete ui;
}

void NewRoute_StopoverWindow::on_newroute_newstopover_submit_buttonBox_accepted()
{
    coordinates = new QString(ui->newroute_newstopover_comboBox->currentText().simplified());
    notes = new QString(ui->newroute_newstopover_notes_textEdit->toPlainText().simplified());
    parrival = new QDateTime(ui->newroute_newstopover_parrival_time_dateTimeEdit->dateTime());
    pdepature = new QDateTime(ui->newroute_newstopover_pdepature_time_dateTimeEdit->dateTime());
    if (*parrival >=*pdepature){
        this->prompt_error("Невірні значення часу прибуття та відправлення. Повторіть ще раз.");
    }
    else{
        if (*coordinates==""){
          prompt_error("Заповніть форму з координатами!");
        }
        else{
            QSqlQuery sql_query;
            sql_query.exec(QString("SELECT EXISTS(SELECT p.\"Coordinates\" FROM busrunsdb.\"Place\" p WHERE p.\"Coordinates\"='%0');").arg(*coordinates));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            else{
                sql_query.first();
                if (sql_query.value(0).toBool()==false){
                    if (prompt_question("Такої локації не існує. Ви впевнені, що бажаєте створити нову?")){
                        sql_query.exec(QString("SELECT busrunsdb.on_place_insert('%0','%1');").arg(*coordinates,*notes));
                        if(sql_query.lastError().isValid()) {
                            qDebug() << sql_query.lastError();
                        }
                        this->accept();
                    }
                }
                else{
                    this->accept();
                }
            }
        }
    }
}

void NewRoute_StopoverWindow::on_newroute_newstopover_submit_buttonBox_rejected()
{
    this->reject();
}

void NewRoute_StopoverWindow::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"Виникла помилка",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int NewRoute_StopoverWindow::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

void NewRoute_StopoverWindow::on_newroute_newstopover_comboBox_currentIndexChanged(int index)
{
    QSqlQuery sql_query;
    sql_query.prepare(QString("SELECT bs.\"Notes\" FROM busrunsdb.\"BusStop\" bs WHERE bs.\"Id\"=\
                                    (SELECT p.\"Id\" FROM busrunsdb.\"Place\" p WHERE p.\"Coordinates\"='%0');")\
                                .arg(ui->newroute_newstopover_comboBox->itemText(index)));
    sql_query.exec();
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    else{
        sql_query.first();
        ui->newroute_newstopover_notes_textEdit->setText(sql_query.value(0).toString());
    }
}

void NewRoute_StopoverWindow::on_newroute_newstopover_comboBox_currentTextChanged(const QString &arg1)
{
    QSqlQuery sql_query;
    sql_query.prepare(QString("SELECT bs.\"Notes\" FROM busrunsdb.\"BusStop\" bs WHERE bs.\"Id\"=\
                                    (SELECT p.\"Id\" FROM busrunsdb.\"Place\" p WHERE p.\"Coordinates\"='%0');")\
                                .arg(arg1));
    sql_query.exec();
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    else{
        sql_query.first();
        ui->newroute_newstopover_notes_textEdit->setText(sql_query.value(0).toString());
    }
}

void NewRoute_StopoverWindow::on_newroute_newstopover_parrival_time_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    ui->newroute_newstopover_pdepature_time_dateTimeEdit->setDateTime(dateTime);}
