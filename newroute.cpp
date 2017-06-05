#include "newroute.h"
#include "ui_newroute.h"

NewRoute::NewRoute(QWidget *parent, QSqlDatabase db1):
    QDialog(parent),
    ui(new Ui::NewRoute)
{
    ui->setupUi(this);
    this->db = db1;
    fill_combobox_with_query(ui->newroute_route_transport_type_comboBox,"SELECT DISTINCT vt.\"TypeName\" FROM busrunsdb.\"VehicleType\" vt");
    config_default_behavior(ui->newroute_Route_stops_tableWidget);
    this->show();
}

NewRoute::~NewRoute()
{
    delete ui;
}

void NewRoute::on_newroute_submit_buttonBox_accepted()
{
    if (ui->newroute_Route_stops_tableWidget->rowCount()<2){
        prompt_error("Маршрут має складатись принаймні з двох зупинок. Спробуйте ще раз.");
    }
    else{
        QSqlQuery sql_query;
        sql_query.prepare(QString("SELECT busrunsdb.on_route_insert(:len,:min_resctrict,:vehicle_type);"));
        sql_query.bindValue(0,ui->newroute_route_length_spinBox->text().toInt());
        sql_query.bindValue(1,ui->newroute_route_driver_expirience_spinBox->text().toInt());
        sql_query.bindValue(2,ui->newroute_route_transport_type_comboBox->currentText());
        sql_query.exec();
        if(sql_query.lastError().isValid()) {
            qDebug() << sql_query.lastError();
            qDebug() << ui->newroute_route_length_spinBox->text().toInt();
            qDebug() << ui->newroute_route_driver_expirience_spinBox->text().toInt();
            qDebug() << ui->newroute_route_transport_type_comboBox->currentText();
        }
        else{
            sql_query.first();
            int route_id=sql_query.value(0).toInt();
            for(int i=0;i<ui->newroute_Route_stops_tableWidget->rowCount();i++){
                QSqlQuery sql_subquery;
                sql_subquery.prepare(QString("SELECT busrunsdb.on_stopover_insert(:coord,:parr,:pdep,:notes,:route_id);"));
                sql_subquery.bindValue(0,ui->newroute_Route_stops_tableWidget->item(i,0)->text());
                sql_subquery.bindValue(1,ui->newroute_Route_stops_tableWidget->item(i,1)->text());
                sql_subquery.bindValue(2,ui->newroute_Route_stops_tableWidget->item(i,2)->text());
                sql_subquery.bindValue(3,ui->newroute_Route_stops_tableWidget->item(i,3)->text());
                sql_subquery.bindValue(4,route_id);
                sql_subquery.exec();
                if(sql_subquery.lastError().isValid()) {
                    qDebug() << sql_subquery.lastError();
                }
                else{
                    this->accept();
                }
            }
        }
    }
}

void NewRoute::on_newroute_submit_buttonBox_rejected()
{

    this->reject();
}

void NewRoute::on_newroute_new_stopover_pushButton_clicked()
{
    NewRoute_StopoverWindow * form = new NewRoute_StopoverWindow(this, this->db);
    if (form->exec()==QDialog::Rejected){
        form->close();
        delete form;
    }
    else{
        if (insert_stopover(form->coordinates, form->notes, form->parrival, form->pdepature)){
            ui->newroute_Route_stops_tableWidget->sortByColumn(1,Qt::SortOrder::AscendingOrder);
            form->close();
            delete form;
        }
    }
}

int NewRoute::insert_stopover(QString* coordinates,QString* notes,QDateTime* parrival,QDateTime* pdepature){
    ui->newroute_Route_stops_tableWidget->sortByColumn(1,Qt::SortOrder::AscendingOrder);
    if (ui->newroute_Route_stops_tableWidget->rowCount()==0){
       if (parrival->toString("dd hh:mm")=="01 00:00") {
            ui->newroute_Route_stops_tableWidget->insertRow(0);
            ui->newroute_Route_stops_tableWidget->setItem(0, 1, new QTableWidgetItem(parrival->toString("dd hh:mm")));
            ui->newroute_Route_stops_tableWidget->setItem(0, 2, new QTableWidgetItem(pdepature->toString("dd hh:mm")));
            ui->newroute_Route_stops_tableWidget->setItem(0, 0, new QTableWidgetItem(*coordinates));
            ui->newroute_Route_stops_tableWidget->setItem(0, 3, new QTableWidgetItem(*notes));
            return 1;
        }
       else{
           prompt_error("Виникла помилка при узгоджені часів прибуття на зупинки та відправлення з зупинок. Відлік маршрута починається з мітки '01 00:00'. Спробуйте ще раз.");
           return 0;
       }
    }
    if(ui->newroute_Route_stops_tableWidget->item(ui->newroute_Route_stops_tableWidget->rowCount()-1,1)->text() < parrival->toString("dd hh:mm")\
            && ui->newroute_Route_stops_tableWidget->item(ui->newroute_Route_stops_tableWidget->rowCount()-1,2)->text() < parrival->toString("dd hh:mm")){
        if (ui->newroute_Route_stops_tableWidget->item(ui->newroute_Route_stops_tableWidget->rowCount()-1,0)->text()!=*coordinates){
            ui->newroute_Route_stops_tableWidget->insertRow(ui->newroute_Route_stops_tableWidget->rowCount());
            ui->newroute_Route_stops_tableWidget->setItem(ui->newroute_Route_stops_tableWidget->rowCount()-1, 1, new QTableWidgetItem(parrival->toString("dd hh:mm")));
            ui->newroute_Route_stops_tableWidget->setItem(ui->newroute_Route_stops_tableWidget->rowCount()-1, 2, new QTableWidgetItem(pdepature->toString("dd hh:mm")));
            ui->newroute_Route_stops_tableWidget->setItem(ui->newroute_Route_stops_tableWidget->rowCount()-1, 0, new QTableWidgetItem(*coordinates));
            ui->newroute_Route_stops_tableWidget->setItem(ui->newroute_Route_stops_tableWidget->rowCount()-1, 3, new QTableWidgetItem(*notes));
            return 1;
        }
        else{
            prompt_error("Сусідні зупинки мають бути в різних локаціях. Спробуйте ще раз.");
            return 0;
        }
    }
    for (int i = 0;i < ui->newroute_Route_stops_tableWidget->rowCount()-1;i++){
        if (ui->newroute_Route_stops_tableWidget->item(i,1)->text() < parrival->toString("dd hh:mm") && ui->newroute_Route_stops_tableWidget->item(i+1,1)->text() > parrival->toString("dd hh:mm") \
                && ui->newroute_Route_stops_tableWidget->item(i,2)->text() < parrival->toString("dd hh:mm") && ui->newroute_Route_stops_tableWidget->item(i+1,1)->text() > pdepature->toString("dd hh:mm")){
            if (ui->newroute_Route_stops_tableWidget->item(i,0)->text()!=*coordinates && ui->newroute_Route_stops_tableWidget->item(i+1,0)->text()!=*coordinates){
                ui->newroute_Route_stops_tableWidget->insertRow(i+1);
                ui->newroute_Route_stops_tableWidget->setItem(i+1, 1, new QTableWidgetItem(parrival->toString("dd hh:mm")));
                ui->newroute_Route_stops_tableWidget->setItem(i+1, 2, new QTableWidgetItem(pdepature->toString("dd hh:mm")));
                ui->newroute_Route_stops_tableWidget->setItem(i+1, 0, new QTableWidgetItem(*coordinates));
                ui->newroute_Route_stops_tableWidget->setItem(i+1, 3, new QTableWidgetItem(*notes));
                return 1;
            }
            else{
                prompt_error("Сусідні зупинки мають бути в різних локаціях. Спробуйте ще раз.");
                return 0;
           }
        }
    }
    prompt_error("Виникла помилка при узгоджені часів прибуття на зупинки та відправлення з зупинок. Спробуйте ще раз.");
    return 0;
}

void NewRoute::prompt_error(QString text, bool exit_flag) {
    QMessageBox messageBox;
    messageBox.critical(0,"An error occurred",text);
    messageBox.setFixedSize(500,200);
    if(exit_flag) {
        QApplication::closeAllWindows();
        exit(0);
    }
}

int NewRoute::prompt_question(QString text) {
    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);
    QMessageBox::StandardButton a = messageBox.question(0,"Ви впевнені, що бажаєте продовжити?",text);
    if (a==QMessageBox::Yes) return 1;
    else return 0;
}



void NewRoute::on_newroute_Route_stops_tableWidget_itemSelectionChanged()
{
    if (ui->newroute_Route_stops_tableWidget->currentRow() == 0 && ui->newroute_Route_stops_tableWidget->rowCount() > 1) ui->newroute_stopover_delete_pushButton->setVisible(false);
}

void NewRoute::on_newroute_stopover_delete_pushButton_clicked()
{
    ui->newroute_Route_stops_tableWidget->removeRow(ui->newroute_Route_stops_tableWidget->currentRow());
}
