#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include <QObject>
#include <QDebug>

class FunctionCall : public QObject
{
    Q_OBJECT
public:
    explicit FunctionCall(QObject *parent = 0);

    void setStartedTimestamp(unsigned sec, unsigned nsec) { secStartedTimestamp = sec; nsecStartedTimestamp = nsec; }
    void setStartedTimestampExclusive(unsigned sec, unsigned nsec) { secStartedTimestampEx = sec; nsecStartedTimestampEx = nsec; }

    void addDepth() { currentDepth++; }
    unsigned getDepth() { return currentDepth; }
    void removeDepth() { currentDepth--; }

    void countTotalTime(unsigned sec, unsigned nsec);
    void countTotalTimeExclusive(unsigned sec, unsigned nsec);
    double getTotalTime();
    double getTotalTimeExclusive();

signals:
    
public slots:

private:
    int callCount;
    unsigned secLength;
    unsigned nsecLength;
    unsigned secLengthEx;
    unsigned nsecLengthEx;

    unsigned secStartedTimestamp;
    unsigned nsecStartedTimestamp;
    unsigned secStartedTimestampEx;
    unsigned nsecStartedTimestampEx;

    unsigned currentDepth;
};

#endif // FUNCTIONCALL_H
