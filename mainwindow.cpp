#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    qDebug() << "MainWindow::on_pushButton_clicked()";

    QNetworkRequest request(QUrl("http://192.168.0.92:8081/zeroconf/switch"));
    request.setRawHeader("Content-Type", "text/plain;charset=UTF-8");



    QJsonDocument doc;
    QJsonObject obj;
    obj.insert("deviceid", "100000140e");
    QJsonObject objdata;

    if (power == "off")
        power = "on";
    else
        power = "off";
    objdata.insert("switch", power);
    obj.insert("data", objdata);
    doc.setObject(obj);
    qDebug() <<doc.toJson(QJsonDocument::Indented);
    ui->textEdit->setText(doc.toJson(QJsonDocument::Indented));



    QByteArray data = ui->textEdit->toPlainText().toUtf8();
    qDebug() << data;
    manager.post(request,doc.toJson());

}

void MainWindow::onManagerFinished(QNetworkReply *reply)
{
    qDebug()<< reply->readAll();
}

