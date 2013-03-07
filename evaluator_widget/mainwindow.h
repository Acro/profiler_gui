#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QListWidget>

#include "processing.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadFile(QString& name);
    
private slots:
    void addRowToFunctionListWidget(QString line);
    void addRowToFunctionTableWidget(QString function_name, QString function_duration, QString function_duration_exclusive);
    void on_functionTableHorizontalHeaderClicked(int index);

    void on_loadDataButton_clicked();

    void on_loadBinaryButton_clicked();

    void on_processButton_clicked();

    void on_scanBinaryButton_clicked();    

    void on_functionListWidget_itemChanged(QListWidgetItem *item);

    void on_generateCodeButton_clicked();

private:
    Ui::MainWindow *ui;
    Processing processing;

    QString dataFileName;
    QString binaryName;
};

#endif // MAINWINDOW_H
