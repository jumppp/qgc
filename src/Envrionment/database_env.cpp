#include "database_env.h"
#include "QGCApplication.h"
#include "database_env.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QErrorMessage>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <Vehicle.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include<QSqlQueryModel>
#include<QDateTime>
#include <QSqlError>
#include <QThread>

QTimer *timer=new QTimer();
Database_Env::Database_Env(QObject *parent) : QObject(parent)
{

}

bool Database_Env::connect_Database()
{

        if (QSqlDatabase::contains(dbName)) {
            db = QSqlDatabase::database(dbName);
        } else {
            db = QSqlDatabase::addDatabase("QMYSQL", dbName);
        }
        db.setDatabaseName(dbName);
        db.setHostName("localhost");
        db.setUserName("root");
        db.setPassword("123456");
        _dbopen = db.open();
        if (_dbopen)
        {
            qDebug()<<"databese open succeed";
            QMessageBox::information(NULL,"Success","databese open succeed");
            timer_Record();
            return true;
        }
        else{
            qDebug()<<"databese open failed";
            QMessageBox::warning(NULL,"ERROR","databese open failed");
            qDebug()<<db.lastError().text();
            return false;
        }
}

bool Database_Env::disconnect_Database()
{
    db.close();
    timer->stop();
    if(_dbopen)
    {
        QMessageBox::warning(NULL,"Error","database close failed");
        return false;
    }
    else
    {
        QMessageBox::information(NULL,"Success","database close succeed");
        return true;
    }

}

void Database_Env::timer_Record()
{

    QSqlQuery query(db);
    QStringList table = db.tables();
    if(table.contains(tableName1))
    {
         query.prepare(creatTable1);
         qDebug()<<"table already exsists";
    }
    else {
        query.prepare(creatTable1);
        query.exec(creatTable1);
        qDebug()<<"create new table succeed";
        if(query.exec())
        {
            qDebug()<<"new table created success";
        }
        else
        {
            qDebug()<<"table created failed";
            qDebug()<<db.lastError().text();
        }
    }

    timer->setInterval(msecInterval);
    connect(timer,SIGNAL(timeout()),this,SLOT(startSql()));
    timer->start();

}

void Database_Env::startSql()
{
    QSqlQuery query(db);
    QDateTime cur_date_time = QDateTime::currentDateTime();
    QString cur_time = cur_date_time.toString("yyyy-MM-dd hh:mm:ss");
    QString cur_time_time = cur_date_time.toString("hh:mm:ss");
    qDebug()<<cur_time;
    double longitude = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gps()->lon()->rawValue().toDouble();
    double latitude = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gps()->lat()->rawValue().toDouble();
    double altitude = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->altitudeRelative()->rawValue().toDouble();
    float gastempreture = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->gasTempreture()->rawValue().toDouble();
    int humidity = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->humidity()->rawValue().toInt();
    int gaspressure = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->gasPressure()->rawValue().toInt();
    int pm25 = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->pm25()->rawValue().toInt();
    int pm10 = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->pm10()->rawValue().toInt();
    int SO2 = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->so2()->rawValue().toInt();
    int NO2 = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->no2()->rawValue().toInt();
    int CO = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->co()->rawValue().toInt();
    int O3 = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->gasSensor()->o3()->rawValue().toInt();

    query.prepare(inserttoTable1);
    query.bindValue(":t_time",cur_time);
    query.bindValue(":t_time_time",cur_time_time);
    query.bindValue(":longitude",longitude);
    query.bindValue(":latitude",latitude);
    query.bindValue(":altitude",altitude);
    query.bindValue(":tempreture",gastempreture);
    query.bindValue(":humidity",humidity);
    query.bindValue(":pressure",gaspressure);
    query.bindValue(":pm25",pm25);
    query.bindValue(":pm10",pm10);
    query.bindValue(":SO2",SO2);
    query.bindValue(":NO2",NO2);
    query.bindValue(":CO",CO);
    query.bindValue(":O3",O3);

    bool success = query.exec();
    if(success)
    {
        qDebug()<<"insert success";
    }
    else
    {
        qDebug()<<"insert failed";
        qDebug()<<db.lastError().text();
    }

    qDebug()<<"storing data";
}