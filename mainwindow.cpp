#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secdialog.h"
#include "hostdialog.h"

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


void MainWindow::on_Start_clicked()
{
    hide();
    secDialog secDialog;
    secDialog.setWindowTitle("Guest");
    secDialog.setModal(true);
    secDialog.exec();
}

void MainWindow::on_pushButton_clicked()
{
    hide();
    hostDialog hostDialog;
    hostDialog.setWindowTitle("Host");
    hostDialog.setModal(true);
    hostDialog.exec();

}
