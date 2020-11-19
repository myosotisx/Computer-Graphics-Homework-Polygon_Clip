#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menubar->hide();

    setWindowTitle("Clip");
    connect(ui->out, &QRadioButton::toggled, this, &MainWindow::switchInOut);
    connect(ui->clearButton, &QPushButton::clicked, ui->clipWidget, &ClipWidget::clear);
    connect(ui->clipButton, &QPushButton::clicked, ui->clipWidget, &ClipWidget::clip);
    connect(ui->finishButton, &QPushButton::clicked, ui->clipWidget, &ClipWidget::finish);

    connect(ui->clipWidget, &ClipWidget::warning, this, &MainWindow::showStatus);
    connect(ui->clipWidget, &ClipWidget::status, this, &MainWindow::showStatus);
    connect(ui->clipWidget, &ClipWidget::inputOut, ui->out, &QRadioButton::toggle);
    connect(ui->resultOnly, &QCheckBox::stateChanged, ui->clipWidget, &ClipWidget::showResultsMode);
    connect(ui->clipWidget, &ClipWidget::shiftInOut, this, &MainWindow::shiftInOut);
    ui->out->toggle();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showStatus(QString msg) {
    statusBar()->showMessage(msg, 2000);
}

void MainWindow::switchInOut(bool isOut) {
    ui->clipWidget->switchInOut(isOut);
}

void MainWindow::shiftInOut(bool isOut) {
    if(isOut) ui->out->toggle();
    else ui->in->toggle();
}
