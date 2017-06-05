#include "newrun_buswindow.h"
#include "ui_newrun_buswindow.h"

NewRun_BusWindow::NewRun_BusWindow(QWidget *parent, QSqlDatabase db1) :
    QDialog(parent),
    ui(new Ui::NewRun_BusWindow)
{
    db=db1;
    ui->setupUi(this);
    config_default_behavior(ui->newrun_Buses_general_tableWidget);
    connect(this,SIGNAL(accepted()),this,SLOT(hide()));
    connect(this,SIGNAL(rejected()),this,SLOT(hide()));
    this->hide();
}

void NewRun_BusWindow::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"An error occurred",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int NewRun_BusWindow::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

NewRun_BusWindow::~NewRun_BusWindow()
{
    delete ui;
}

void NewRun_BusWindow::setValues(int route_value, QDateTime start_value)
{
    if (start_value!=start || route_value!=route_id){
        start=start_value;
        route_id=route_value;
        refresh_table_buses_avaliable_on_route_at_moment();
        config_default_behavior(ui->newrun_Buses_general_tableWidget);
        emit valuesChanged(route_value,start_value);
    }
}

void NewRun_BusWindow::setBus(int bus_value)
{
    if (ui->newrun_Buses_general_tableWidget->rowCount()==0){
        prompt_error("Нажаль наразі не існує жодного автобуса, що відповідає необхідним вимогам. Спробуйте ще раз.");
    }
    else{
        if (bus_value!=servicebook_id){
            servicebook_id=bus_value;
            emit busChanged(bus_value);
            this->accept();
        }
        else{
            this->accept();
        }
    }
}

void NewRun_BusWindow::setDrivers(QList<int> drivers_values)
{
    if (drivers_values!=drivers_id){
        drivers_id=drivers_values;
        emit driversChanged(drivers_values);
    }
}


void NewRun_BusWindow::on_newrun_bus_runs_registered_hide_pushButton_clicked()
{
    ui->newrun_Bus_runs_registered_tableWidget->setVisible(false);
    ui->newrun_bus_runs_registered_hide_pushButton->setVisible(false);
}

void NewRun_BusWindow::on_newrun_bussubmit_buttonBox_accepted()
{
    if (ui->newrun_Buses_general_tableWidget->rowCount()==0){
        prompt_error("Нажаль наразі не існує жодного автобуса, що відповідає необхідним вимогам. Спробуйте ще раз.");
    }
    else{
     setBus(ui->newrun_Buses_general_tableWidget->item(ui->newrun_Buses_general_tableWidget->currentRow(),0)->text().toInt());
    }
}

void NewRun_BusWindow::on_newrun_bussubmit_buttonBox_rejected()
{
    this->reject();
}

void NewRun_BusWindow::refresh_table_buses_avaliable_on_route_at_moment()
{
    QString query = QString("SELECT * FROM busrunsdb.buses_avaliable_on_route_at_moment(%0,'%1'::timestamp);").arg(QString::number(route_id),start.toString(Qt::ISODate));
    if(!fill_table_with_query(ui->newrun_Buses_general_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }
    config_default_behavior(ui->newrun_Buses_general_tableWidget);
}

void NewRun_BusWindow::createRun()
{
    QString query = QString("SELECT \"Id\" FROM busrunsdb.\"Vehicle\" WHERE \"ServiceBook_Id\"=%0;");
    QSqlQuery sql_query;
    sql_query.exec(query.arg(servicebook_id));
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    sql_query.first();
    int vehicle_id=sql_query.value(0).toInt();

    query = QString("INSERT INTO busrunsdb.\"Run\" (\"Route_Id\",\"Vehicle_Id\",\"PStart\") VALUES (%0,%1,'%2');").arg(QString::number(route_id),QString::number(vehicle_id),start.toString(Qt::ISODate));
    sql_query.exec(query);
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }

    for (int i=0; i<drivers_id.size();i++){
        query = QString("INSERT INTO busrunsdb.\"RunDriver\" (\"Driver_Id\") VALUES (%0);").arg(QString::number(drivers_id[i]));
        sql_query.exec(query);
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
    }

    emit runCreated();
}

void NewRun_BusWindow::on_newrun_Buses_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->newrun_Bus_runs_registered_tableWidget->setVisible(true);
    ui->newrun_bus_runs_registered_hide_pushButton->setVisible(true);
    if (ui->newrun_Buses_general_tableWidget->rowCount()!=0){
        refresh_table_buses_registered(ui->newrun_Buses_general_tableWidget->item(ui->newrun_Buses_general_tableWidget->currentRow(),0)->text().toInt());
    }
}

void NewRun_BusWindow::refresh_table_buses_registered(int bus_servicebook_id)
{
    QString query = QString("SELECT * FROM busrunsdb.buses_history(%0);").arg(QString::number(bus_servicebook_id));
    if(!fill_table_with_query(ui->newrun_Bus_runs_registered_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }

    query = QString("SELECT busrunsdb.run_status(%0);");
    QSqlQuery sql_query;
    int status;
    for (int i=0;i<ui->newrun_Bus_runs_registered_tableWidget->rowCount();i++){
        sql_query.exec(query.arg(ui->newrun_Bus_runs_registered_tableWidget->item(i,0)->text()));
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
        sql_query.first();
        status=sql_query.value(0).toInt();
        switch(status)
        {
        case 0:
            for(int j = 0; j < ui->newrun_Bus_runs_registered_tableWidget->columnCount(); j++)
                ui->newrun_Bus_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::white)));
            break;
        case 1:
            for(int j = 0; j < ui->newrun_Bus_runs_registered_tableWidget->columnCount(); j++)
                ui->newrun_Bus_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::yellow)));
            break;
        case 2:
            for(int j = 0; j < ui->newrun_Bus_runs_registered_tableWidget->columnCount(); j++)
                ui->newrun_Bus_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::green)));
            break;
        }
        config_default_behavior(ui->newrun_Bus_runs_registered_tableWidget);
    }
}
