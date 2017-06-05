#include "unscheduledstop.h"
#include "ui_unscheduledstop.h"

UnscheduledStop::UnscheduledStop(QWidget *parent, QSqlDatabase db1) :
    QDialog(parent),
    ui(new Ui::UnscheduledStop)
{
    ui->setupUi(this);
    this->db = db1;
    fill_combobox_with_query(ui->unscheduled_stop_coordinates_comboBox,"SELECT DISTINCT p.\"Coordinates\" FROM busrunsdb.\"Place\" p");
    this->show();
}

void UnscheduledStop::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"An error occurred",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int UnscheduledStop::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

UnscheduledStop::~UnscheduledStop()
{
    delete ui;
}

void UnscheduledStop::on_unscheduled_stop_submit_buttonBox_accepted()
{
    coordinates = ui->unscheduled_stop_coordinates_comboBox->currentText().simplified();
    notes = ui->unscheduled_stop_notes_textEdit->toPlainText().simplified();

    if (coordinates==""){
      prompt_error("Заповніть форму з координатами!");
    }
    else{
      QSqlQuery sql_query;
      sql_query.exec(QString("SELECT EXISTS(SELECT p.\"Coordinates\" FROM busrunsdb.\"Place\" p WHERE p.\"Coordinates\"='%0');").arg(coordinates));
      if(sql_query.lastError().isValid()) {
          qDebug() << sql_query.lastError();
      }
      else{
          sql_query.first();
          if (sql_query.value(0).toBool()==false){
             if (prompt_question("Такої локації не існує. Ви впевнені, що бажаєте створити нову?")){
                  sql_query.exec(QString("SELECT busrunsdb.on_place_insert('%0','%1');").arg(coordinates,notes));
                  if(sql_query.lastError().isValid()){
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

void UnscheduledStop::on_unscheduled_stop_submit_buttonBox_rejected()
{
    this->reject();
}

void UnscheduledStop::on_unscheduled_stop_coordinates_comboBox_currentIndexChanged(int index)
{
    QSqlQuery sql_query;
    sql_query.prepare(QString("SELECT bs.\"Notes\" FROM busrunsdb.\"BusStop\" bs WHERE bs.\"Id\"=\
                                    (SELECT p.\"Id\" FROM busrunsdb.\"Place\" p WHERE p.\"Coordinates\"='%0');")\
                                .arg(ui->unscheduled_stop_coordinates_comboBox->itemText(index)));
    sql_query.exec();
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    else{
        sql_query.first();
        ui->unscheduled_stop_notes_textEdit->setText(sql_query.value(0).toString());
    }
}

void UnscheduledStop::on_unscheduled_stop_coordinates_comboBox_currentTextChanged(const QString &arg1)
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
        ui->unscheduled_stop_notes_textEdit->setText(sql_query.value(0).toString());
    }
}
