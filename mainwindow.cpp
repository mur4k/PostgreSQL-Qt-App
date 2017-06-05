#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    //qt basic config
    ui->setupUi(this);
    ip = "bus-stops-db.ccqa5cns6u8c.eu-central-1.rds.amazonaws.com";
    port = "5432";
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(ip);
    db.setPort(port.toInt());
    db.setDatabaseName("busstopsdb");
    this->create_db_connection();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::create_db_connection() {
    LoginWindow login_form(this);
    this->hide();
    login_form.show();

    if(login_form.exec() == QDialog::Accepted) {
        qDebug() << "Login Accepted";
        this->login = login_form.get_login();
        this->password = login_form.get_password();
        db.setUserName("masterStudent");
        db.setPassword("timursanek2017");
        db.open();
        role=getRole();
        db.close();
        db.setUserName(login);
        db.setPassword(password);

        if(db.open()){
            qDebug() << "connected to db";
            this->setWindowTitle(QString("Диспетчерська: '%1'").arg(role));
            if (role=="controller_assistant") change_observer_mode(false);

            set_output_format();
            //initial tables filling
            this->refresh_table_drivers();
            this->refresh_table_buses();
            this->refresh_table_routes();
            this->refresh_table_runs();
            config_default_behavior(ui->Drivers_general_tableWidget);
            config_default_behavior(ui->Buses_general_tableWidget);
            config_default_behavior(ui->Routes_general_tableWidget);
            config_default_behavior(ui->Route_stops_tableWidget);
            config_default_behavior(ui->Run_stops_tableWidget);
            config_default_behavior(ui->Runs_general_tableWidget);
            config_default_behavior(ui->Drivers_runs_registered_tableWidget);
            config_default_behavior(ui->Bus_runs_registered_tableWidget);
            ui->Route_stops_tableWidget->horizontalHeader()->setSectionHidden(0,true);
            ui->Drivers_general_tableWidget->horizontalHeader()->setSectionHidden(0,true);
            ui->Run_stops_tableWidget->horizontalHeader()->setSectionHidden(0,true);
            login_form.close();
            this->show();
        }
        else {
            qDebug() << "can not connected to db";
            qDebug() << db.lastError();
            prompt_error("Виникла помилка при підключенні до бази даних! Перевірте правильність логіну/пароля.",false);
            login_form.close();
            create_db_connection();
        }
    }
    else {
        qDebug() << "Login Rejected";
        QApplication::closeAllWindows();
        exit(0);
    }

}

//getting user's role
QString MainWindow::getRole() {
    QSqlQuery sq = db.exec(QString("select rolname from pg_user\
                            join pg_auth_members on (pg_user.usesysid=pg_auth_members.member)\
                            join pg_roles on (pg_roles.oid=pg_auth_members.roleid)\
                            where pg_user.usename='%1';").arg(login));
    sq.first();
    qDebug()<<sq.value(0).toString();
    return sq.value(0).toString();
}

void MainWindow::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"Виникла помилка",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int MainWindow::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}

void MainWindow::change_observer_mode(bool state) {
    ui->additional_stop_pushButton->setVisible(state);
    ui->new_route_pushButton->setVisible(state);
    ui->new_run_pushButton->setVisible(state);
}

void MainWindow::refresh_table_drivers() {
    QString query = QString("SELECT * FROM busrunsdb.\"Driver\";");
    if(!fill_table_with_query(ui->Drivers_general_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }
    config_default_behavior(ui->Drivers_general_tableWidget);
}

void MainWindow::refresh_table_buses() {
    QString query = QString("SELECT * FROM busrunsdb.buses_general;");
    if(!fill_table_with_query(ui->Buses_general_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }
    config_default_behavior(ui->Buses_general_tableWidget);
}

void MainWindow::refresh_table_runs_stops(int run_id)
{
    QString query = QString("SELECT * FROM busrunsdb.run_plan(%0);").arg(run_id);
    if(!fill_table_with_query(ui->Run_stops_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }

    query = QString("SELECT busrunsdb.stop_status(%0,%1);");
    QSqlQuery sql_query;
    int status;
    for (int i=0;i<ui->Run_stops_tableWidget->rowCount();i++){
        sql_query.exec(query.arg(QString::number(run_id),ui->Run_stops_tableWidget->item(i,0)->text()));
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
        sql_query.first();
        status=sql_query.value(0).toInt();
        switch(status)
        {
        case 0:
            for(int j = 0; j < ui->Run_stops_tableWidget->columnCount(); j++)
                ui->Run_stops_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::white)));
            break;
        case 1:
            for(int j = 0; j < ui->Run_stops_tableWidget->columnCount(); j++)
                ui->Run_stops_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::yellow)));
            break;
        case 2:
            for(int j = 0; j < ui->Run_stops_tableWidget->columnCount(); j++)
                ui->Run_stops_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::green)));
            break;
        }
    }
    config_default_behavior(ui->Run_stops_tableWidget);
}

void MainWindow::refresh_table_buses_registered(int bus_servicebook_id)
{
    QString query = QString("SELECT * FROM busrunsdb.buses_history(%0);").arg(QString::number(bus_servicebook_id));
    if(!fill_table_with_query(ui->Bus_runs_registered_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }

    query = QString("SELECT busrunsdb.run_status(%0);");
    QSqlQuery sql_query;
    int status;
    for (int i=0;i<ui->Bus_runs_registered_tableWidget->rowCount();i++){
        sql_query.exec(query.arg(ui->Bus_runs_registered_tableWidget->item(i,0)->text()));
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
        sql_query.first();
        status=sql_query.value(0).toInt();
        switch(status)
        {
        case 0:
            for(int j = 0; j < ui->Bus_runs_registered_tableWidget->columnCount(); j++)
                ui->Bus_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::white)));
            break;
        case 1:
            for(int j = 0; j < ui->Bus_runs_registered_tableWidget->columnCount(); j++)
                ui->Bus_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::yellow)));
            break;
        case 2:
            for(int j = 0; j < ui->Bus_runs_registered_tableWidget->columnCount(); j++)
                ui->Bus_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::green)));
            break;
        }
        config_default_behavior(ui->Bus_runs_registered_tableWidget);
    }
}

void MainWindow::refresh_table_drivers_registered(int driver_id)
{
    QString query = QString("SELECT * FROM busrunsdb.drivers_history(%0);").arg(QString::number(driver_id));
    if(!fill_table_with_query(ui->Drivers_runs_registered_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }

    query = QString("SELECT busrunsdb.run_status(%0);");
    QSqlQuery sql_query;
    int status;
    for (int i=0;i<ui->Drivers_runs_registered_tableWidget->rowCount();i++){
        sql_query.exec(query.arg(ui->Drivers_runs_registered_tableWidget->item(i,0)->text()));
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
        sql_query.first();
        status=sql_query.value(0).toInt();
        switch(status)
        {
        case 0:
            for(int j = 0; j < ui->Drivers_runs_registered_tableWidget->columnCount(); j++)
                ui->Drivers_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::white)));
            break;
        case 1:
            for(int j = 0; j < ui->Drivers_runs_registered_tableWidget->columnCount(); j++)
                ui->Drivers_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::yellow)));
            break;
        case 2:
            for(int j = 0; j < ui->Drivers_runs_registered_tableWidget->columnCount(); j++)
                ui->Drivers_runs_registered_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::green)));
            break;
        }
    }
    config_default_behavior(ui->Drivers_runs_registered_tableWidget);
}

void MainWindow::refresh_table_routes() {
    QString query = QString("SELECT * FROM busrunsdb.routes_general;");
    if(!fill_table_with_query(ui->Routes_general_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }
    config_default_behavior(ui->Routes_general_tableWidget);
}

void MainWindow::refresh_table_runs() {
    QString query = QString("SELECT * FROM busrunsdb.runs_general;");
    if(!fill_table_with_query(ui->Runs_general_tableWidget,query)) {
        prompt_error("Виникла помилка при завантаженні даних!");
    }

    query = QString("SELECT busrunsdb.run_status(%0);");
    QSqlQuery sql_query;
    int status;
    for (int i=0;i<ui->Runs_general_tableWidget->rowCount();i++){
        sql_query.exec(query.arg(ui->Runs_general_tableWidget->item(i,0)->text()));
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
        }
        sql_query.first();
        status=sql_query.value(0).toInt();
        switch(status)
        {
        case 0:
            for(int j = 0; j < ui->Runs_general_tableWidget->columnCount(); j++)
                ui->Runs_general_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::white)));
            break;
        case 1:
            for(int j = 0; j < ui->Runs_general_tableWidget->columnCount(); j++)
                ui->Runs_general_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::yellow)));
            break;
        case 2:
            for(int j = 0; j < ui->Runs_general_tableWidget->columnCount(); j++)
                ui->Runs_general_tableWidget->item(i, j)->setBackground(QBrush(QColor(Qt::green)));
            break;
        }
    }
    config_default_behavior(ui->Runs_general_tableWidget);
}


void MainWindow::set_output_format()
{
    QString query = QString("SET DATESTYLE TO ISO;");
    QSqlQuery sql_query;
    sql_query.exec(query);
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
    sql_query.exec(query);
    query = QString("SET INTERVALSTYLE TO ISO_8601");
    if(sql_query.lastError().isValid()) {
        qDebug() << sql_query.lastError();
    }
}

void MainWindow::on_exit_pushButton_clicked()
{
    db.close();
    create_db_connection();
}

void MainWindow::on_stop_info_hide_pushButton_clicked()
{
    ui->Run_info_groupBox->setVisible(false);

}

void MainWindow::on_route_info_hide_pushButton_clicked()
{
    ui->Route_info_groupBox->setVisible(false);
}

void MainWindow::on_new_route_pushButton_clicked()
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

void MainWindow::on_Routes_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->Route_info_groupBox->setVisible(true);
    fill_table_with_query(ui->Route_stops_tableWidget,QString("SELECT * FROM busrunsdb.route_plan(%0)").arg(ui->Routes_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->route_driver_expirience_num_lb,QString("SELECT \"BottomRestriction\" FROM busrunsdb.\"Route\" WHERE \"Id\"=%0").arg(ui->Routes_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->route_length_num_lb,QString("SELECT \"Length\" FROM busrunsdb.\"Route\" WHERE \"Id\"=%0").arg(ui->Routes_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->route_transport_type_name_lb,QString("SELECT vt.\"TypeName\" FROM busrunsdb.\"Route\" r LEFT JOIN busrunsdb.\"VehicleType\" vt\
                                                                         ON r.\"VehicleType_Id\"=vt.\"Id\"  WHERE r.\"Id\"=%0").arg(ui->Routes_general_tableWidget->item(row,0)->text().toInt()));
    config_default_behavior(ui->Route_stops_tableWidget);
}

void MainWindow::on_new_run_pushButton_clicked()
{
    NewRun_RouteWindow* route_form = new NewRun_RouteWindow(this, this->db);
    NewRun_BusWindow* bus_form = new NewRun_BusWindow(this, this->db);
    NewRun_DriversWindow* drivers_form = new NewRun_DriversWindow(this, this->db);
    route_form->show();

    connect(route_form,SIGNAL(valuesChanged(int,QDateTime)),bus_form,SLOT(setValues(int,QDateTime)));
    connect(route_form,SIGNAL(valuesChanged(int,QDateTime)),drivers_form,SLOT(setValues(int,QDateTime)));
    connect(bus_form,SIGNAL(busChanged(int)),route_form,SLOT(setBus(int)));
    connect(bus_form,SIGNAL(busChanged(int)),drivers_form,SLOT(setBus(int)));
    connect(drivers_form,SIGNAL(driversChanged(QList<int>)),bus_form,SLOT(setDrivers(QList<int>)));
    connect(drivers_form,SIGNAL(driversChanged(QList<int>)),route_form,SLOT(setDrivers(QList<int>)));


    connect(route_form,SIGNAL(accepted()),bus_form, SLOT(show()));
    connect(route_form,SIGNAL(rejected()),bus_form,SLOT(close()));
    connect(route_form,SIGNAL(rejected()),drivers_form,SLOT(close()));
    connect(bus_form,SIGNAL(accepted()),drivers_form,SLOT(show()));
    connect(bus_form,SIGNAL(rejected()),route_form,SLOT(show()));
    connect(drivers_form,SIGNAL(rejected()),bus_form,SLOT(show()));
    connect(drivers_form,SIGNAL(runCreated()),bus_form,SLOT(close()));
    connect(drivers_form,SIGNAL(runCreated()),route_form,SLOT(close()));
    connect(drivers_form,SIGNAL(runCreated()),this,SLOT(refresh_table_runs()));
}



void MainWindow::on_drivers_runs_registered_hide_pushButton_clicked()
{
    ui->Drivers_runs_registered_tableWidget->setVisible(false);
    ui->drivers_runs_registered_hide_pushButton->setVisible(false);
}



void MainWindow::on_bus_runs_registered_hide_pushButton_clicked()
{
    ui->Bus_runs_registered_tableWidget->setVisible(false);
    ui->bus_runs_registered_hide_pushButton->setVisible(false);
}

void MainWindow::on_Buses_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->Bus_runs_registered_tableWidget->setVisible(true);
    ui->bus_runs_registered_hide_pushButton->setVisible(true);
    if (ui->Buses_general_tableWidget->rowCount()!=0){
        refresh_table_buses_registered(ui->Buses_general_tableWidget->item(ui->Buses_general_tableWidget->currentRow(),0)->text().toInt());
    }
}

void MainWindow::on_Drivers_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->Drivers_runs_registered_tableWidget->setVisible(true);
    ui->drivers_runs_registered_hide_pushButton->setVisible(true);
    if (ui->Drivers_general_tableWidget->rowCount()!=0){
        refresh_table_drivers_registered(ui->Drivers_general_tableWidget->item(ui->Drivers_general_tableWidget->currentRow(),0)->text().toInt());
    }
}

void MainWindow::on_Runs_general_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->Run_info_groupBox->setVisible(true);
    current_run=ui->Runs_general_tableWidget->item(row,0)->text().toInt();
    refresh_table_runs_stops(ui->Runs_general_tableWidget->item(row,0)->text().toInt());
  //  refresh_table_runs();
    fill_lb_with_query(ui->run_pass_attended_num_lb,QString("SELECT \"PassangersAttended\" FROM busrunsdb.\"Run\" WHERE \"Id\"=%0").arg(ui->Runs_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->run_pass_registered_num_lb,QString("SELECT count(*) FROM busrunsdb.\"Ticket\" WHERE \"Run_Id\"=%0").arg(ui->Runs_general_tableWidget->item(row,0)->text().toInt()));
    fill_lb_with_query(ui->run_bus_on_run_num_lb,QString("SELECT sb.\"Id\" FROM busrunsdb.\"Run\" r LEFT JOIN busrunsdb.\"Vehicle\" v\
                                                                         ON r.\"Vehicle_Id\"=v.\"Id\" LEFT JOIN busrunsdb.\"ServiceBook\" sb\
                                                                         ON sb.\"Id\"=v.\"ServiceBook_Id\"   WHERE r.\"Id\"=%0").arg(ui->Runs_general_tableWidget->item(row,0)->text().toInt()));
    fill_combobox_with_query(ui->run_drivers_on_run_comboBox,QString("SELECT dr.\"Surname\" FROM busrunsdb.\"Driver\" dr LEFT JOIN busrunsdb.\"RunDriver\" rd\
                                                                                ON rd.\"Driver_Id\"=dr.\"Id\" WHERE rd.\"Run_Id\"=%0").arg(ui->Runs_general_tableWidget->item(row,0)->text().toInt()));
}

void MainWindow::on_stop_accept_pushButton_clicked()
{
    if (ui->Run_stops_tableWidget->rowCount()==0){
        prompt_error("Виберіть рейс з яким плануєте працювати");
    }
    else{
        if (ui->Runs_general_tableWidget->rowCount()==0){
            prompt_error("Немає жодного запланованого рейсу");
        }
        else{
            QString query = QString("SELECT busrunsdb.run_status(%0);");
            QSqlQuery sql_query;
            int status;
            sql_query.exec(query.arg(QString::number(current_run)));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            sql_query.first();
            status=sql_query.value(0).toInt();
            query = QString("SELECT busrunsdb.stop_status(%0,%1);");
            int stop_status;
            sql_query.exec(query.arg(QString::number(current_run),ui->Run_stops_tableWidget->item(ui->Run_stops_tableWidget->currentRow(),0)->text()));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            sql_query.first();
            stop_status=sql_query.value(0).toInt();
            switch(status)
            {
            case 0:
            {prompt_error("Цей рейс ще не розпочався.");
                break;}
            case 1:

            {switch(stop_status)
                {
                case 0:{
                    int current_stop;
                    query=QString("SELECT busrunsdb.current_stop(%0);");
                    sql_query.exec(query.arg(QString::number(current_run)));
                    if(sql_query.lastError().isValid()) {
                        qDebug() << sql_query.lastError();
                    }
                    sql_query.first();
                    current_stop=sql_query.value(0).toInt();
                    if (current_stop==ui->Run_stops_tableWidget->item(ui->Run_stops_tableWidget->currentRow(),0)->text().toInt())
                    {
                        query = QString("INSERT INTO busrunsdb.\"RealStopover\" (\"Run_Id\",\"RArrivalTime\",\"Stopover_Id\") VALUES (%0,'%1'::TIMESTAMP,%2);");
                        qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate);
                        sql_query.exec(query.arg(QString::number(current_run),QDateTime::currentDateTime().toString(Qt::ISODate),QString::number(current_stop)));
                        if(sql_query.lastError().isValid()) {
                            qDebug() << sql_query.lastError();
                        }

                        refresh_table_runs();
                        refresh_table_runs_stops(current_run);
                    }
                    else{
                        prompt_error("Ви пропустили одну з зупинок. Спробуйте ще раз.");
                    }
                    break;
                }
                case 1:{
                    prompt_error("Ця зупинка вже розпочалася");
                    break;
                }
                case 2:{
                    prompt_error("Ця зупинка вже закінчилася");
                    break;
                }
                }
                break;
            }
            case 2:
            {prompt_error("Цей рейс вже закінчився.");
                break;}
            }
        }
    }
}

void MainWindow::on_stop_end_pushButton_clicked()
{
    if (ui->Run_stops_tableWidget->rowCount()==0){
        prompt_error("Виберіть рейс з яким плануєте працювати");

    }
    else{
        if (ui->Runs_general_tableWidget->rowCount()==0){
            prompt_error("Немає жодного запланованого рейсу");
        }
        else{
            QString query = QString("SELECT busrunsdb.run_status(%0);");
            QSqlQuery sql_query;
            int status;
            sql_query.exec(query.arg(QString::number(current_run)));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            sql_query.first();
            status=sql_query.value(0).toInt();
            query = QString("SELECT busrunsdb.stop_status(%0,%1);");
            int stop_status;
            sql_query.exec(query.arg(QString::number(current_run),ui->Run_stops_tableWidget->item(ui->Run_stops_tableWidget->currentRow(),0)->text()));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            sql_query.first();
            stop_status=sql_query.value(0).toInt();
            switch(status)
            {
            case 0:
            {prompt_error("Цей рейс ще не розпочався.");
                break;}
            case 1:
            {switch(stop_status)
                {
                case 0:{
                    prompt_error("Ця зупинка ще не розпочалася");
                    break;
                }
                case 1:{
                    int current_stop;
                    query=QString("SELECT busrunsdb.current_stop(%0);");
                    sql_query.exec(query.arg(QString::number(current_run)));
                    if(sql_query.lastError().isValid()) {
                        qDebug() << sql_query.lastError();
                    }
                    sql_query.first();
                    current_stop=sql_query.value(0).toInt();
                    if (current_stop==ui->Run_stops_tableWidget->item(ui->Run_stops_tableWidget->currentRow(),0)->text().toInt())
                    {
                        query = QString("UPDATE busrunsdb.\"RealStopover\" SET \"RDepatureTime\"='%1'::TIMESTAMP WHERE \"Id\"=%2 AND \"Run_Id\"=%0;");
                        qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate);
                        sql_query.exec(query.arg(QString::number(current_run),QDateTime::currentDateTime().toString(Qt::ISODate),QString::number(current_stop)));
                        if(sql_query.lastError().isValid()) {
                            qDebug() << sql_query.lastError();
                        }

                        refresh_table_runs();
                        refresh_table_runs_stops(current_run);
                    }
                    else{
                        prompt_error("Ви пропустили одну з зупинок. Спробуйте ще раз.");
                    }
                    break;
                }
                case 2:{
                    prompt_error("Ця зупинка вже закінчилася");
                    break;
                }
                }
                break;
            }
            case 2:
            {prompt_error("Цей рейс вже закінчився.");
                break;}
            }
        }
    }
}

void MainWindow::on_additional_stop_pushButton_clicked()
{
    if (ui->Run_stops_tableWidget->rowCount()==0){
         prompt_error("Виберіть рейс з яким плануєте працювати");

    }
    else{
        if (ui->Runs_general_tableWidget->rowCount()==0){
            prompt_error("Немає жодного запланованого рейсу");
        }
        else{
            QString query = QString("SELECT busrunsdb.run_status(%0);");
            QSqlQuery sql_query;
            int status;
            sql_query.exec(query.arg(current_run));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            sql_query.first();
            status=sql_query.value(0).toInt();
            switch(status)
            {
            case 0:{
                prompt_error("Цей рейс ще не розпочався.");
                break;}
            case 1:{
                int current_stop_status;
                query=QString("SELECT busrunsdb.stop_status(%0,(SELECT busrunsdb.current_stop(%1)));").arg(QString::number(current_run),QString::number(current_run));
                sql_query.exec(query);
                if(sql_query.lastError().isValid()) {
                    qDebug() << sql_query.lastError();
                }
                sql_query.first();
                current_stop_status=sql_query.value(0).toInt();
                qDebug()<<query;
                switch (current_stop_status) {
                case 0:{
                    UnscheduledStop* form = new UnscheduledStop(this, this->db);
                    if (form->exec()==QDialog::Rejected){
                        form->close();
                        delete form;
                    }
                    else{
                        query = QString("SELECT busrunsdb.on_unscheduled_stopover_insert(%0,'%1','%2','%3'::TIMESTAMP,NULL);");
                        sql_query.exec(query.arg(QString::number(current_run),form->coordinates,form->notes,QDateTime::currentDateTime().toString(Qt::ISODate)));
                        if(sql_query.lastError().isValid()) {
                            qDebug() << sql_query.lastError();
                        }
                        refresh_table_runs();
                        refresh_table_runs_stops(ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text().toInt());
                        form->close();
                        delete form;
                    }
                    break;}
                case 1:{
                    prompt_error("Наразі цей рейс вже знаходиться на зупинці.");
                    break;}
                }
                break;}
            case 2:{
                prompt_error("Цей рейс вже закінчився.");
                break;}
            }
        }
    }
}

void MainWindow::on_start_run_pushButton_clicked()
{
    if (ui->Run_stops_tableWidget->rowCount()==0){
        prompt_error("Виберіть рейс з яким плануєте працювати");

    }
    else{
        if (ui->Runs_general_tableWidget->rowCount()==0){
            prompt_error("Немає жодного запланованого рейсу");
        }
        else{
            QString query = QString("SELECT busrunsdb.run_status(%0);");
            QSqlQuery sql_query;
            int status;
            sql_query.exec(query.arg(ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text()));
            if(sql_query.lastError().isValid()) {
                qDebug() << sql_query.lastError();
            }
            sql_query.first();
            status=sql_query.value(0).toInt();
            switch(status)
            {
            case 0:
            {StartRun* form = new StartRun(this, this->db,ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text().toInt());
                if (form->exec()==QDialog::Rejected){
                    form->close();
                    delete form;
                }
                else{
                    int pass_atnd=form->passangers_attended;
                    qDebug()<<pass_atnd;
                    query = QString("UPDATE busrunsdb.\"Run\" SET \"PassangersAttended\"=%0 WHERE \"Id\"=%1;");
                    sql_query.exec(query.arg(QString::number(pass_atnd),ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text()));
                    if(sql_query.lastError().isValid()) {
                        qDebug() << sql_query.lastError();
                    }

                    int current_stop;
                    query=QString("SELECT busrunsdb.current_stop(%0);");
                    sql_query.exec(query.arg(ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text()));
                    if(sql_query.lastError().isValid()) {
                        qDebug() << sql_query.lastError();
                    }
                    sql_query.first();
                    current_stop=sql_query.value(0).toInt();
                    qDebug()<<current_stop;
                    qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate);
                    query = QString("INSERT INTO busrunsdb.\"RealStopover\" (\"Run_Id\",\"RArrivalTime\",\"Stopover_Id\") VALUES (%0,'%1'::TIMESTAMP,%2);");
                    qDebug()<<QDateTime::currentDateTime().toString(Qt::ISODate);
                    sql_query.exec(query.arg(ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text(),QDateTime::currentDateTime().toString(Qt::ISODate),QString::number(current_stop)));
                    if(sql_query.lastError().isValid()) {
                        qDebug() << sql_query.lastError();
                    }

                    refresh_table_runs();
                    refresh_table_runs_stops(ui->Runs_general_tableWidget->item(ui->Runs_general_tableWidget->currentRow(),0)->text().toInt());
                    form->close();
                    delete form;
                }
                break;}
            case 1:
            {prompt_error("Цей рейс вже у дорозі.");
                break;}
            case 2:
            {prompt_error("Цей рейс вже закінчився.");
                break;}
            }
        }
    }
}


void MainWindow::on_report_pushButton_clicked()
{
    NCReport *report = new NCReport();
    report->setReportFile("/home/timurmirlas/studying_/DB/CourseProject/report.ncr"); //set the report filename fullpath or relative to dir
    report->runReportToShowPreview(); // run and show to preview output

    // error handling
    if( report->hasError())
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Report error: ") + report->lastErrorMsg());
        msgBox.exec();
    }
    delete report;
}
