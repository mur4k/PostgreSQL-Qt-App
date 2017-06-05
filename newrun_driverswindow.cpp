#include "newrun_driverswindow.h"
#include "ui_newrun_driverswindow.h"

NewRun_DriversWindow::NewRun_DriversWindow(QWidget *parent, QSqlDatabase db1) :
    QDialog(parent),
    ui(new Ui::NewRun_DriversWindow)
{
    db=db1;
    ui->setupUi(this);
    config_default_behavior(ui->newrun_Drivers_general_tableWidget);
    ui->newrun_Drivers_general_tableWidget->horizontalHeader()->setSectionHidden(0,true);
    connect(this,SIGNAL(accepted()),this,SLOT(hide()));
    connect(this,SIGNAL(accepted()),this,SLOT(createRun()));
    connect(this,SIGNAL(rejected()),this,SLOT(hide()));
    connect(this,SIGNAL(runCreated()),this,SLOT(close()));
    this->hide();
}

void NewRun_DriversWindow::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"An error occurred",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int NewRun_DriversWindow::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

NewRun_DriversWindow::~NewRun_DriversWindow()
{
    delete ui;
}

void NewRun_DriversWindow::setValues(int route_value, QDateTime start_value)
{
    if (start_value!=start || route_value!=route_id){
        start=start_value;
        route_id=route_value;
        refresh_table_drivers_avaliable_on_route_at_moment();
        config_default_behavior(ui->newrun_Drivers_general_tableWidget);
        emit valuesChanged(route_value,start_value);
    }
}

void NewRun_DriversWindow::setBus(int bus_value)
{
    if (bus_value!=servicebook_id){
        servicebook_id=bus_value;
        emit busChanged(bus_value);
    }
}

void NewRun_DriversWindow::setDrivers(QList<int> drivers_values)
{
    if(ui->newrun_Drivers_general_tableWidget->rowCount()==0){
        prompt_error("Нажаль наразі не існує жодного водія, що відповідає необхідним вимогам. Спробуйте ще раз.");
    }
    else{
        if (drivers_id.isEmpty()) prompt_error("Ви не обрали жодного водія. Спробуйте ще раз.");
        else {
            if (drivers_values!=drivers_id){
                drivers_id=drivers_values;
                emit driversChanged(drivers_values);
                this->accept();
            }
            else{
                this->accept();
            }
        }
    }
}

void NewRun_DriversWindow::on_newrun_submitdrivers_buttonBox_accepted()
{
    if(ui->newrun_Drivers_general_tableWidget->rowCount()==0){
        prompt_error("Нажаль наразі не існує жодного водія, що відповідає необхідним вимогам. Спробуйте ще раз.");
    }
    else{
        setDrivers(drivers_id);
    }

}

void NewRun_DriversWindow::on_newrun_submitdrivers_buttonBox_rejected()
{
    this->reject();
}

void NewRun_DriversWindow::on_newrun_add_driver_pushButton_clicked()
{
    if(ui->newrun_Drivers_general_tableWidget->rowCount()==0){
        prompt_error("Нажаль наразі не існує жодного водія, що відповідає необхідним вимогам. Спробуйте ще раз.");
    }
    else{
        if (!ui->newrun_Drivers_general_tableWidget->isRowHidden(ui->newrun_Drivers_general_tableWidget->currentRow()))
        {
            drivers_id.append(ui->newrun_Drivers_general_tableWidget->item(ui->newrun_Drivers_general_tableWidget->currentRow(),0)->text().toInt());
            ui->newrun_selected_drivers_comboBox->addItem(ui->newrun_Drivers_general_tableWidget->item(ui->newrun_Drivers_general_tableWidget->currentRow(),1)->text());
            ui->newrun_Drivers_general_tableWidget->hideRow(ui->newrun_Drivers_general_tableWidget->currentRow());
        }
    }
}

void NewRun_DriversWindow::refresh_table_drivers_avaliable_on_route_at_moment()
{
    QString query = QString("SELECT * FROM busrunsdb.drivers_avaliable_on_route_at_moment(%0,'%1'::TIMESTAMP);").arg(QString::number(route_id),start.toString(Qt::ISODate));
    if(!fill_table_with_query(ui->newrun_Drivers_general_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }
}

void NewRun_DriversWindow::createRun()
{
    QString query = QString("SELECT \"Id\" FROM busrunsdb.\"Vehicle\" WHERE \"ServiceBook_Id\"=%0;");
    QSqlQuery sql_query;
    sql_query.exec(query.arg(servicebook_id));
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    sql_query.first();
    int vehicle_id=sql_query.value(0).toInt();
    qDebug()<<vehicle_id;

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

void NewRun_DriversWindow::on_newrun_remove_driver_pushButton_clicked()
{
    if (!drivers_id.isEmpty())
    {
        int driver_id=drivers_id[ui->newrun_selected_drivers_comboBox->currentIndex()];
        for (int i=0; i<ui->newrun_Drivers_general_tableWidget->rowCount(); i++){
            if (ui->newrun_Drivers_general_tableWidget->item(i,0)->text().toInt()==driver_id){
                ui->newrun_Drivers_general_tableWidget->showRow(i);
                drivers_id.removeAt(ui->newrun_selected_drivers_comboBox->currentIndex());
                ui->newrun_selected_drivers_comboBox->removeItem(ui->newrun_selected_drivers_comboBox->currentIndex());
            }
        }
    }
}

void NewRun_DriversWindow::on_newrun_Drivers_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->newrun_Drivers_runs_registered_tableWidget->setVisible(true);
    ui->newrun_drivers_runs_registered_hide_pushButton->setVisible(true);
    if (ui->newrun_Drivers_general_tableWidget->rowCount()!=0){
        refresh_table_drivers_registered(ui->newrun_Drivers_general_tableWidget->item(ui->newrun_Drivers_general_tableWidget->currentRow(),0)->text().toInt());
    }
}

void NewRun_DriversWindow::refresh_table_drivers_registered(int driver_id)
{
    QString query = QString("SELECT * FROM busrunsdb.drivers_history(%0);").arg(QString::number(driver_id));
    if(!fill_table_with_query(ui->newrun_Drivers_runs_registered_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }

    query = QString("SELECT busrunsdb.run_status(%0);");
    QSqlQuery sql_query;
    int status;
    for (int i=0;i<ui->newrun_Drivers_runs_registered_tableWidget->rowCount();i++){
        sql_query.exec(query.arg(ui->newrun_Drivers_runs_registered_tableWidget->item(i,0)->text()));
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
        sql_query.first();
        status=sql_query.value(0).toInt();
        switch(status)
        {
        case 0:
            for(int j = 0; j < ui->newrun_Drivers_runs_registered_tableWidget->columnCount(); j++)
                ui->newrun_Drivers_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::white)));
            break;
        case 1:
            for(int j = 0; j < ui->newrun_Drivers_runs_registered_tableWidget->columnCount(); j++)
                ui->newrun_Drivers_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::yellow)));
            break;
        case 2:
            for(int j = 0; j < ui->newrun_Drivers_runs_registered_tableWidget->columnCount(); j++)
                ui->newrun_Drivers_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::green)));
            break;
        }
    }
    config_default_behavior(ui->newrun_Drivers_runs_registered_tableWidget);
}

void NewRun_DriversWindow::on_newrun_drivers_runs_registered_hide_pushButton_clicked()
{
    ui->newrun_Drivers_runs_registered_tableWidget->setVisible(false);
    ui->newrun_drivers_runs_registered_hide_pushButton->setVisible(false);
}

