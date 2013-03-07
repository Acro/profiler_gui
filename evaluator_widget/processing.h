#ifndef PROCESSING_H
#define PROCESSING_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

#include "functioncall.h"

class Processing : public QObject
{
    Q_OBJECT
public:
    explicit Processing(QObject *parent = 0);
    ~Processing();

    // evaluator
    void receiveData(QString data);
    void wipeData();
    void resolveFunctionNames(QString pathToBinary);
    void loadResolvedFunctionNames();
    void showData();

    // profiler
    void scanForFunctionsInBinary(QString pathToBinary);
    void parseAddressFunctionFromBinary(QString line);
    void generateProfilerCode(QList<QString> functions);

signals:
    // for communication with GUI
    void addFunction(QString name);
    void addFunctionData(QString function_name, QString function_duration, QString function_duration_exclusive);

public slots:
    
private:
    // evaluator
    QMap<QString, FunctionCall*> calls; // key = function address, value = object containing calculated data
    QMap<QString, QString> function_names; // key = function address, value = name
    QMap<QString, QString> function_aliases;    

    QString prev_function, main_function;
    unsigned prev_sec, prev_nsec;

    // profiler
    QMap<QString, QString> functions_binary; // key = function name, value = hexadecimal address
};

#endif // PROCESSING_H
