#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   // Remove the current titlebar to implement a custom one
    isWindowMaximized = false;
    ui->mainBodyContainer->setCurrentIndex(0);              // Set first page as Home Page

    HomePage* homePage = new HomePage(ui->homePage, ui->timeLabel, ui->dateLabel, this);
    homePage->updateDateAndTime();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Quit button
void MainWindow::on_quitButton_clicked()
{
    QCoreApplication::quit();
}

// Maximize button
void MainWindow::on_maximizeButton_clicked()
{
    if(isWindowMaximized)
    {
        showNormal();
        isWindowMaximized = false;
    }
    else
    {
        showMaximized();
        isWindowMaximized = true;
    }
}

// Minimize button
void MainWindow::on_minimizeButton_clicked()
{
    showMinimized();
}

// Shows home page
void MainWindow::on_pushButton_2_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(0);
}

// Shows telemetry page
void MainWindow::on_pushButton_3_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(1);
}

