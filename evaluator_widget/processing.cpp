#include "processing.h"
#include <string>
#include <QString>

Processing::Processing(QObject *parent) :
    QObject(parent)
{
}

Processing::~Processing() {
    qDeleteAll(calls);
}

void Processing::receiveData(QString data) {    

    QStringList fields = data.split(" ");

    if(fields[0] == "e") {

        // set main function address
        if(main_function.size() == 0) {
            main_function = fields[1];
        }

        // aliases for exclusive time spent in function computation
        if(!function_aliases.contains(fields[2]) && fields[2] != prev_function) {
            if(prev_function.size() != 0) {
                if(calls[prev_function]->getDepth() == 0) {
                    function_aliases.insert(fields[2], main_function);
                    calls[main_function]->countTotalTimeExclusive(fields[3].toInt(), fields[4].toInt());
                } else {
                    function_aliases.insert(fields[2], prev_function);
                }
            } else {
                function_aliases.insert(fields[2], prev_function);
            }

            qDebug() << function_aliases;
        }

        // this is for exclusive time spent in function computation
        // if previous function is correctly set and previous function is not the same
        if(prev_function.size() != 0 && fields[1] != function_aliases[fields[2]] && calls[prev_function]->getDepth() > 0) {
            // we count total time, but we are not lowering the depth
            calls[prev_function]->countTotalTimeExclusive(fields[3].toInt(), fields[4].toInt());
        }

        if(!calls.contains(fields[1])) {

            FunctionCall* temp = new FunctionCall();
            temp->setStartedTimestamp(fields[3].toInt(), fields[4].toInt());
            temp->setStartedTimestampExclusive(fields[3].toInt(), fields[4].toInt());
            calls.insert(fields[1], temp);

        } else {

            if(calls[fields[1]]->getDepth() == 0) {
                calls[fields[1]]->setStartedTimestamp(fields[3].toInt(), fields[4].toInt());
                calls[fields[1]]->setStartedTimestampExclusive(fields[3].toInt(), fields[4].toInt());
            }
            calls[fields[1]]->addDepth();

        }

    } else if(fields[0] == "x") {

        // this is for exclusive time spent in function computation
        // if the previous function is not the same as current call
        if(fields[1] != function_aliases[fields[2]]) {
            // we set back the starting time
            if(function_aliases[fields[2]].size() != 0) {
                calls[function_aliases[fields[2]]]->setStartedTimestampExclusive(fields[3].toInt(), fields[4].toInt());
                qDebug() << function_aliases[fields[2]];
            } else {

            }
        }

        if(calls[fields[1]]->getDepth() == 1) {

            calls[fields[1]]->removeDepth();            
            calls[fields[1]]->countTotalTime(fields[3].toInt(), fields[4].toInt());
            calls[fields[1]]->countTotalTimeExclusive(fields[3].toInt(), fields[4].toInt());

        } else {
            calls[fields[1]]->removeDepth();
        }

    }

    prev_function = fields[1];
    prev_sec = fields[3].toInt();
    prev_nsec = fields[4].toInt();
}

void Processing::wipeData() {

    qDeleteAll(calls);

    QMap<QString, FunctionCall*>::iterator iter;
    for (iter = calls.begin(); iter != calls.end(); ++iter ) {
        calls.erase(iter);
    }

    QMap<QString, QString>::iterator iter2;
    for (iter2 = function_names.begin(); iter2 != function_names.end(); ++iter2 ) {
        function_names.erase(iter2);
    }

    QMap<QString, QString>::iterator iter3;
    for (iter3 = function_aliases.begin(); iter3 != function_aliases.end(); ++iter3 ) {
        function_aliases.erase(iter3);
    }

}

void Processing::resolveFunctionNames(QString pathToBinary) {
    system("cat /dev/null > /tmp/prof.out"); // clears previous results

    for(QString function_address : calls.keys()) {
        QString command = "echo -n '" + function_address + " ' >> /tmp/prof.out; addr2line -f -e " + pathToBinary + " " + function_address + " | head -1 >> /tmp/prof.out";
        std::string temp = command.toStdString();
        char *temp2 = (char*)temp.c_str();
        system(temp2);
    }

    loadResolvedFunctionNames();

}

void Processing::loadResolvedFunctionNames() {
    QFile file( "/tmp/prof.out" );
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        function_names.insert(fields[0], fields[1]);
    }

    file.close();
}

void Processing::scanForFunctionsInBinary(QString pathToBinary) {

    QMap<QString, QString>::iterator iter;
    for (iter = functions_binary.begin(); iter != functions_binary.end(); ++iter ) {
        functions_binary.erase(iter);
    }

    QString command = "nm -r -t x " + pathToBinary + " > /tmp/functions_in_binary.out";
    std::string temp = command.toStdString();
    char *temp2 = (char*)temp.c_str();
    system(temp2);

    QFile file( "/tmp/functions_in_binary.out" );
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        parseAddressFunctionFromBinary(line);
    }

    file.close();

}

void Processing::parseAddressFunctionFromBinary(QString line) {

    QStringList fields = line.split(" ");

    if(fields[1] == "T") {
        emit addFunction(fields[2]);
        bool ok;
        QString transformed_address = "0x" + QString::number( fields[0].toInt(&ok, 16), 16);
        functions_binary.insert(fields[2], transformed_address);
    }

}

void Processing::showData() {
    for(QString function_address : calls.keys()) {
        //bool ok;
        //emit addFunctionData( functions_binary.value("0x" + QString::number( function_name.toInt(&ok, 16), 16)), QString::number( calls.value(function_name)->getTotalTime(), 'f', 9) );
        emit addFunctionData( function_names[function_address], QString::number( calls.value(function_address)->getTotalTime(), 'f', 9), QString::number( calls.value(function_address)->getTotalTimeExclusive(), 'f', 9) );
    }
}

void Processing::generateProfilerCode(QList<QString> functions) {

    int i = 0;
    QString out_function_pointers;
    QString out_functions_condition;

    foreach(QString function, functions) {
        out_function_pointers.append( QString("static void\* function_preset_\%1 = (void\*)\%2;\\n").arg(i).arg(functions_binary[function]) );
        if(i > 0) {
            out_functions_condition.append(" || ");
        }
        out_functions_condition.append( QString("func == function_preset_\%1").arg(i++) );
    }

    QString command = "cat ~/grid/profiler/bachelor/code_templates/trace_specific_template.c > /tmp/acroprof_temp.c;";
    command.append("sed -i 's/<function_pointer>/" + out_function_pointers + "/g' /tmp/acroprof_temp.c;");
    command.append("sed -i 's/<functions_condition>/" + out_functions_condition + "/g' /tmp/acroprof_temp.c;");
    const char *temp = command.toStdString().c_str();
    system(temp);

}
