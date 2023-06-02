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
    QextSerialPort *m_cprUSBPort;
    ITestReporter *m_reporter;
    QByteArray m_writeUARTData;
    QByteArray m_writeUSBData;
    bool receivedUSB;
    bool receivedUART;
    QByteArray m_readUARTData;
    QByteArray m_readUSBData;
    QSemaphore m_semaphore;
};

#endif // TESTCANCPR_H
