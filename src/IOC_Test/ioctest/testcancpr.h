#ifndef TESTCANCPR_H
#define TESTCANCPR_H

#include <QObject>
#include <QSemaphore>
#include <qextserialport.h>

#include "itestcase.h"

class TestCanCpr
    : public QObject,
      public ITestCase
{
    Q_OBJECT
public:
    explicit TestCanCpr(QObject *parent = 0);

    virtual QString getName() const;
    virtual void setReporter(ITestReporter *reporter);
    virtual void runTest();

private slots:
    void receivedMessageCPR(void);
    void receivedMessageCAN(void);

private:
    QextSerialPort *m_canSerialPort;
    QextSerialPort *m_cprSerialPort;
    ITestReporter *m_reporter;
    QByteArray m_writeData;

    QSemaphore m_semaphore;
    quint16 m_receivedValue;
};

#endif // TESTCANCPR_H
