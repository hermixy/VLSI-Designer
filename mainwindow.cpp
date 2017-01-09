#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    interfaces = QNetworkInterface::allInterfaces();
    showNetworkInterfaces();

    scanner.initIPv6Multicast(QHostAddress(ui->scanningAddressText->text()), interfaces.at(ui->interfacesBox->currentIndex()),
                              ui->scanningPortText->text().toInt(), ui->responsePortText->text().toInt());

    connect (&scanner, SIGNAL(sendLog(QString)), this, SLOT(on_sendLog(QString)));
    connect (&scanner, SIGNAL(sendAddress(QHostAddress, QString)), this, SLOT(on_sendAddress(QHostAddress, QString)));

    clientT.init(40001);
    serverT.init(40000);

    connect(&serverT, SIGNAL(dataReceived(QByteArray,QHostAddress,int)), this, SLOT(on_dataReceived(QByteArray,QHostAddress,int)));
}

void MainWindow::on_sendLog(QString data)
{
    log(data);
}

void MainWindow::on_sendAddress(QHostAddress senderHost, QString hostName)
{
    QTableWidgetItem* item = new QTableWidgetItem(hostName);
    QTableWidgetItem* item1 = new QTableWidgetItem(senderHost.toString());
    ui->nodeList->insertRow(ui->nodeList->rowCount());

    ui->nodeList->setItem(ui->nodeList->rowCount()-1, 0, item);
    ui->nodeList->setItem(ui->nodeList->rowCount()-1, 1, item1);
}

void MainWindow::showNetworkInterfaces()
{
    ui->interfacesBox->clear();
    foreach (QNetworkInterface i, interfaces)
    {
        ui->interfacesBox->addItem(i.humanReadableName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_scanButton_clicked()
{
    if (!scanner.isStopped())
    {
        scanner.stopScanning();
        ui->scanButton->setText("Scan");

        return;
    }

    ui->scanButton->setText("Stop");

    ui->nodeList->clearContents();
    ui->nodeList->setRowCount(0);

    scanner.scanNetwork();
}

void MainWindow::on_sendButton_clicked()
{
    QString ip = ui->nodeList->selectedItems()[1]->text();
    QHostAddress host(ip);

    clientT.sendData(ui->sendText->text().toUtf8(), host, 40000);
}

void MainWindow::log(QString text)
{
    ui->logText->append(text);
}

void MainWindow::on_saveButton_clicked()
{
    if (!scanner.isStopped())
    {
        scanner.stopScanning();
        ui->scanButton->setText("Scan");
    }

    if (ui->ipv6Button->isChecked())
        scanner.initIPv6Multicast(QHostAddress(ui->scanningAddressText->text()), interfaces.at(ui->interfacesBox->currentIndex()),
                                  ui->scanningPortText->text().toInt(), ui->responsePortText->text().toInt());
    else
        try
        {
            scanner.initIPv4Broadcast(interfaces.at(ui->interfacesBox->currentIndex()),
                                  ui->scanningPortText->text().toInt(), ui->responsePortText->text().toInt());
        }
        catch(NetworkException ex)
        {
           QMessageBox::critical(this, "Exception", ex.what());
        }
}

void MainWindow::on_dataReceived(QByteArray data, QHostAddress address, int port)
{
    log(QString("Got data %1 from %2:%3").arg(data, address.toString(), QString::number(port)));
}
