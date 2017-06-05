#include "newrun_routewindow.h"
#include "ui_newrun_routewindow.h"

NewRun_RouteWindow::NewRun_RouteWindow(QWidget *parent, QSqlDatabase db1) :
    QDialog(parent),
    ui(new Ui::NewRun_RouteWindow)
{
    ui->setupUi(this);
    this->db = db1;
    refresh_table_routes();
    config_default_behavior(ui->newrun_Routes_general_tableWidget);
    config_default_behavior(ui->newrun_Route_stops_tableWidget);
    ui->newrun_Route_stops_tableWidget->horizontalHeader()->setSectionHidden(0,true);
    ui->dateTimeEdit->setMinimumDateTime(QDateTime::currentDateTime());
    connect(this,SIGNAL(accepted()),this,SLOT(hide()));
    connect(this,SIGNAL(rejected()),this,SLOT(close()));
    this->hide();
}

void NewRun_RouteWindow::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"An error occurred",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int NewRun_RouteWindow::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

NewRun_RouteWindow::~NewRun_RouteWindow()
{
    delete ui;
}

void NewRun_RouteWindow::setValues(int route_value, QDateTime start_value)
{
    if (start!=start_value || route_id!=route_value){
        if (ui->dateTimeEdit->dateTime()<QDateTime::currentDateTime()){
            prompt_error("Дата призначення рейсу задана некоректно. Спробуйте ще раз.");
        }
        else{
            if (ui->newrun_Routes_general_tableWidget->selectedItems().empty()){
                prompt_error("Виберіть маршрут слідування або створіть новий.");
            }
            else{
                start=start_value;
                route_id=route_value;
                emit valuesChanged(route_value,start_value);
                this->accept();
            }
        }
    }
    else{
        this->accept();
    }
}

void NewRun_RouteWindow::setBus(int bus_value)
{
    if (bus_value!=servicebook_id){
        servicebook_id=bus_value;
        emit busChanged(bus_value);
    }
}

void NewRun_RouteWindow::setDrivers(QList<int> drivers_values)
{
    if (drivers_values!=drivers_id){
        drivers_id=drivers_values;
        emit driversChanged(drivers_values);
    }
}

void NewRun_RouteWindow::refresh_table_routes()
{
    QString query = QString("SELECT * FROM busrunsdb.routes_general");
    if(!fill_table_with_query(ui->newrun_Routes_general_tableWidget,query)) {
        prompt_error("An error has occurred when uploading data to table!");
    }
}

void NewRun_RouteWindow::createRun()
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

void NewRun_RouteWindow::on_newrun_route_info_hide_pushButton_clicked()
{
    ui->newrun_Route_info_groupBox->setVisible(false);
}

void NewRun_RouteWindow::on_newrun_Routes_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->newrun_Route_info_groupBox->setVisible(true);
    fill_table_with_query(ui->newrun_Route_stops_tableWidget,QString("SELECT * FROM busrunsdb.route_plan(%0)").arg(ui->newrun_Routes_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->newrun_route_driver_expirience_num_lb,QString("SELECT \"BottomRestriction\" FROM busrunsdb.\"Route\" WHERE \"Id\"=%0").arg(ui->newrun_Routes_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->newrun_route_length_num_lb,QString("SELECT \"Length\" FROM busrunsdb.\"Route\" WHERE \"Id\"=%0").arg(ui->newrun_Routes_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->newrun_route_transport_type_name_lb,QString("SELECT vt.\"TypeName\" FROM busrunsdb.\"Route\" r LEFT JOIN busrunsdb.\"VehicleType\" vt\
                                                                         ON r.\"VehicleType_Id\"=vt.\"Id\"  WHERE r.\"Id\"=%0").arg(ui->newrun_Routes_general_tableWidget->item(row,0)->text().toInt()));
}

void NewRun_RouteWindow::on_newrun_new_route_pushButton_clicked()
{
    NewRoute* form = new NewRoute(this, this->db);
    if (form->exec()==QDialog::Rejected){
        form->close();
        delete form;
    }
    else{
       refresh_table_routes();
       form->close();
       delete form;
    }
}

void NewRun_RouteWindow::on_newrun_buttonBox_rejected()
{
    this->reject();
}

void NewRun_RouteWindow::on_newrun_buttonBox_accepted()
{
    if(ui->newrun_Routes_general_tableWidget->rowCount()==0){
        prompt_error("Нажаль наразі не існує жодного маршруту. Спробуйте ще раз.");
    }
    else{
     this->setValues(ui->newrun_Routes_general_tableWidget->item(ui->newrun_Routes_general_tableWidget->currentRow(),0)->text().toInt(),ui->dateTimeEdit->dateTime());
    }
}
