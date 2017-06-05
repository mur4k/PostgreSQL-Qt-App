#include "startrun.h"
#include "ui_startrun.h"

StartRun::StartRun(QWidget *parent, QSqlDatabase db1, int run_id1) :
    QDialog(parent),
    ui(new Ui::StartRun)
{
    ui->setupUi(this);
    this->db=db1;
    this->run_id=run_id1;
    QString query = QString("SELECT count(*) FROM busrunsdb.\"Ticket\" WHERE \"Run_Id\"=%0;");
    QSqlQuery sql_query;
    int tickets_sold;
    sql_query.exec(query.arg(run_id));
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    sql_query.first();
    tickets_sold=sql_query.value(0).toInt();
    ui->passangers_attended_num_spinBox->setMaximum(tickets_sold);
}

void StartRun::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"An error occurred",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int StartRun::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

StartRun::~StartRun()
{
    delete ui;
}

void StartRun::on_start_run_submit_buttonBox_accepted()
{
    passangers_attended=ui->passangers_attended_num_spinBox->value();
//  qDebug()<<passangers_attended;
    this->accept();
}

void StartRun::on_start_run_submit_buttonBox_rejected()
{
    this->reject();
}
