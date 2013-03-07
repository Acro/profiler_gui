#include "functioncall.h"

FunctionCall::FunctionCall(QObject *parent) :
    QObject(parent)
{
    currentDepth = 1;
    secLength = 0;
    nsecLength = 0;
    secLengthEx = 0;
    nsecLengthEx = 0;
    callCount = 1;
}

void FunctionCall::countTotalTime(unsigned sec, unsigned nsec) {
    if(sec - secStartedTimestamp == 0) {
        nsecLength += nsec - nsecStartedTimestamp;
    } else {
        qDebug() << nsecLength << nsec << nsecStartedTimestamp;
        nsecLength += nsec + (1000*1000*1000 - nsecStartedTimestamp) + (sec - secStartedTimestamp == 1 ? 0 : (sec - secStartedTimestamp - 1)*1000*1000*1000);
    }

    if(nsecLength >= 1000*1000*1000) {
        secLength += nsecLength / (1000*1000*1000);
        nsecLength = nsecLength % (1000*1000*1000);
    }

}

double FunctionCall::getTotalTime() {
    return secLength + (double)nsecLength / (1000*1000*1000);
}

void FunctionCall::countTotalTimeExclusive(unsigned sec, unsigned nsec) {
    if(sec - secStartedTimestampEx == 0) {
        nsecLengthEx += nsec - nsecStartedTimestampEx;
    } else {
        nsecLengthEx += nsec + (1000*1000*1000 - nsecStartedTimestampEx) + (sec - secStartedTimestampEx == 1 ? 0 : (sec - secStartedTimestampEx - 1)*1000*1000*1000);
    }

    if(nsecLengthEx >= 1000*1000*1000) {
        secLengthEx += nsecLengthEx / (1000*1000*1000);
        nsecLengthEx = nsecLengthEx % (1000*1000*1000);
    }

}

double FunctionCall::getTotalTimeExclusive() {
    return secLengthEx + (double)nsecLengthEx / (1000*1000*1000);
}
