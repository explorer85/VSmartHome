#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QDebug>
#include <QTimer>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    //включение при старте и выключение через интервал
    /*switchSonoff("on");
      QTimer::singleShot(timeToClose, this,[this](){
      switchSonoff("off");
      QTimer::singleShot(500, this,[](){
        QApplication::quit();
      });
    });*/

    t = new QTimer();
    int wakeInterval = 10000;
    t->setInterval(wakeInterval);
    t->start();

    connect(t, &QTimer::timeout, [this, &wakeInterval]() {
        qDebug() << "timeout" << power_;
        if (power_ == "off") {
            t->setInterval(5000);
            switchSonoff("on");
        } else {
            t->setInterval(wakeInterval);
            switchSonoff("off");
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    qDebug() << "MainWindow::on_pushButton_clicked()";

    if (power_ == "off")
        switchSonoff("on");
    else
        switchSonoff("off");
}

void MainWindow::onManagerFinished(QNetworkReply *reply)
{
    qDebug()<< reply->readAll();
}

void MainWindow::loadSettings() {
    QString m_sSettingsFile = QApplication::applicationDirPath() + "/settings.ini";
    qDebug() << m_sSettingsFile;
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    if(!settings.contains("ip"))
        settings.setValue("ip", "192.168.0.92");
    if(!settings.contains("timeclose"))
        settings.setValue("timeclose", 10000);

    sonoffIp = settings.value("ip", "").toString();
    timeToClose = settings.value("timeclose", "").toInt();
    qDebug() << sonoffIp;
}

void MainWindow::switchSonoff(QString state) {

    power_ = state;
    QNetworkRequest request(QUrl("http://" + sonoffIp + ":8081/zeroconf/switch"));
    request.setRawHeader("Content-Type", "text/plain;charset=UTF-8");



    QJsonDocument doc;
    QJsonObject obj;
    obj.insert("deviceid", "100000140e");
    QJsonObject objdata;

    objdata.insert("switch", state);
    obj.insert("data", objdata);
    doc.setObject(obj);
    qDebug() <<doc.toJson(QJsonDocument::Indented);
    ui->textEdit->setText(doc.toJson(QJsonDocument::Indented));



    QByteArray data = ui->textEdit->toPlainText().toUtf8();
    qDebug() << data;
    manager.post(request,doc.toJson());
}
