#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "commons.h"

#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include <fstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( &processing, SIGNAL(addFunction(QString)), this, SLOT(addRowToFunctionListWidget(QString)) );
    connect( &processing, SIGNAL(addFunctionData(QString,QString,QString)), this, SLOT(addRowToFunctionTableWidget(QString,QString,QString)) );
    connect( ui->functionCallsTableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_functionTableHorizontalHeaderClicked(int)) );

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadDataButton_clicked()
{
    dataFileName = QFileDialog::getOpenFileName(this, tr("Load Data"), "/home/acro/grid/profiler/bachelor");    
    ui->statusBar->showMessage("Data file " + dataFileName + " set.");
}

void MainWindow::loadFile(QString &name) {

    processing.wipeData();
    for(int i = ui->functionCallsTableWidget->rowCount() - 1; i >= 0; i--){
        ui->functionCallsTableWidget->removeRow(i);
    }

    QFile file( name );
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        processing.receiveData(line);
    }

    file.close();

}

void MainWindow::loadFileBinary(QString &name) {

    processing.wipeData();
    for(int i = ui->functionCallsTableWidget->rowCount() - 1; i >= 0; i--){
        ui->functionCallsTableWidget->removeRow(i);
    }

    std::ifstream ifs(dataFileName.toStdString(), std::ios::binary);
    Data temp;

    while ( ifs.good() ) {

        ifs.read( reinterpret_cast<char*>(&temp), sizeof temp );
        if ( ifs.good() ) {
            processing.receiveDataBinary(temp);
        }

    }

}

void MainWindow::on_loadBinaryButton_clicked()
{
    binaryName = QFileDialog::getOpenFileName(this, tr("Load Binary"), "/home/acro/grid/profiler/bachelor");
    ui->statusBar->showMessage("Binary file " + binaryName + " set.");
}

void MainWindow::on_processButton_clicked()
{

    loadFileBinary(dataFileName);
    ui->statusBar->showMessage("Function calls loaded and counted.");

    if(binaryName.size() != 0) {
        processing.resolveFunctionNamesBinary(binaryName);
        ui->statusBar->showMessage("Function names resolved.");
    }

    processing.showDataBinary();

/*
    loadFile(dataFileName);
    ui->statusBar->showMessage("Function calls loaded and counted.");

    if(binaryName.size() != 0) {
        processing.resolveFunctionNames(binaryName);
        ui->statusBar->showMessage("Function names resolved.");
    }

    processing.showData();
*/
}

void MainWindow::on_scanBinaryButton_clicked()
{
    QString binary_temp = QFileDialog::getOpenFileName(this, tr("Load Binary"), "/home/acro/grid/profiler/bachelor");
    ui->functionListWidget->clear();
    processing.scanForFunctionsInBinary( binary_temp );
    ui->statusBar->showMessage("Binary file " + binary_temp + " scanned.");
}

void MainWindow::addRowToFunctionListWidget(QString line) {
    QListWidgetItem* item = new QListWidgetItem(line);
    item->setCheckState(Qt::Unchecked);
    ui->functionListWidget->addItem( item );
}

void MainWindow::on_functionListWidget_itemChanged(QListWidgetItem *item)
{
    qDebug() << "item changed";
}

void MainWindow::on_generateCodeButton_clicked()
{    
    QList<QString> functions;
    for(int row = 0; row < ui->functionListWidget->count(); row++) {
        if(ui->functionListWidget->item(row)->checkState() == Qt::Checked) {
            //qDebug() << ui->functionListWidget->item(row)->text();
            functions.push_back(ui->functionListWidget->item(row)->text());
        }
    }

    QString saveFilename = QFileDialog::getSaveFileName(this, tr("Enter filename"), "/home/acro/grid/profiler/bachelor");
    if(!saveFilename.isNull()) {
        processing.generateProfilerCode(functions, saveFilename);
        ui->statusBar->showMessage("Profiler code generated.");
    }

}

void MainWindow::addRowToFunctionTableWidget(QString function_name, QString function_duration, QString function_duration_exclusive) {

    const int currentRow = ui->functionCallsTableWidget->rowCount();
    ui->functionCallsTableWidget->setRowCount(currentRow + 1);
    ui->functionCallsTableWidget->setItem(currentRow, 0, new QTableWidgetItem(function_name));
    ui->functionCallsTableWidget->setItem(currentRow, 1, new QTableWidgetItem(function_duration));
    ui->functionCallsTableWidget->setItem(currentRow, 2, new QTableWidgetItem(function_duration_exclusive));

}

void MainWindow::on_functionTableHorizontalHeaderClicked(int index) {
    ui->functionCallsTableWidget->sortByColumn(index);
}
