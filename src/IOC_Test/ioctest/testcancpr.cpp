#include "testcancpr.h"

#include "testmodewrapper.h"

TestCanCpr::TestCanCpr(QObject *parent)
    : QObject(parent)
    , m_reporter(0)
    , m_writeData("")
    , m_semaphore(0)
{
    m_canSerialPort = new QextSerialPort("/dev/ttyO1", QextSerialPort::EventDriven);
    m_canSerialPort->setBaudRate(BAUD57600);
    m_canSerialPort->setFlowControl(FLOW_OFF);
    m_canSerialPort->open(QextSerialPort::ReadWrite);

    m_cprSerialPort = new QextSerialPort ("/dev/ttymxc0", QextSerialPort::EventDriven);
    m_cprSerialPort->setBaudRate(BAUD57600);
    m_cprSerialPort->setFlowControl(FLOW_OFF);
    m_cprSerialPort->open(QextSerialPort::ReadWrite);
}

QString TestCanCpr::getName() const
{
    return QString("canDriverAndCprMeter");
}

void TestCanCpr::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestCanCpr::receivedMessageCPR(void)
{
    QByteArray readData = m_cprSerialPort->readAll();
    if(!readData.contains(m_writeData))
    {
        m_reporter->testHasFailed(
            QString("CAN->CPR: Incoming data does not match. Received (%1), expected (%2)")
            .arg((QString)readData).arg((QString)m_writeData));
    }
    else {
        m_semaphore.release();
    }
}

void TestCanCpr::receivedMessageCAN(void)
{
    QByteArray readData = m_canSerialPort->readAll();
    if(!readData.contains(m_writeData))
    {
        m_reporter->testHasFailed(
            QString("CPR->CAN: Incoming data does not match. Received (%1), expected (%2)")
            .arg((QString)readData).arg((QString)m_writeData));
    }
    else {
        m_semaphore.release();
    }
}

void TestCanCpr::runTest()
{
    connect(m_canSerialPort, SIGNAL(readyRead()), this, SLOT(receivedMessageCAN()));
    connect(m_cprSerialPort, SIGNAL(readyRead()), this, SLOT(receivedMessageCPR()));

    m_writeData.append("0123456789abcdefloremipsumdolor");

    m_cprSerialPort->flush();
    m_canSerialPort->write(m_writeData);

    if (not m_semaphore.tryAcquire(1, 5000))
    {
        m_reporter->testHasFailed(
            QString("CAN->CPR: Didn't receive any data!"));
    }

    m_writeData.append("more");
    m_canSerialPort->flush();
    m_cprSerialPort->write(m_writeData);

    if (not m_semaphore.tryAcquire(1, 5000))
    {
        m_reporter->testHasFailed(
            QString("CPR->CAN: Didn't receive any data!"));
    }
}
