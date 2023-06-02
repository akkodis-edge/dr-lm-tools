#include "testcancpr.h"
#include "testmodewrapper.h"
#include <QTest>
#include <QDebug>

TestCanCpr::TestCanCpr(QObject *parent)
    : QObject(parent)
    , m_reporter(0)
    , m_writeUARTData("")
    , m_writeUSBData("")
    , m_semaphore(0)
{
   m_canSerialPort = new QextSerialPort("/dev/ttymxc2", QextSerialPort::EventDriven);
    m_canSerialPort->setBaudRate(BAUD115200);
    m_canSerialPort->setFlowControl(FLOW_OFF);
    m_canSerialPort->setDataBits(DATA_8);
    m_canSerialPort->setParity(PAR_NONE);
    m_canSerialPort->setStopBits(STOP_1);
    m_canSerialPort->open(QextSerialPort::ReadWrite);

    m_cprUSBPort = new QextSerialPort ("/dev/ttyUSB0", QextSerialPort::EventDriven);
    m_cprUSBPort->setBaudRate(BAUD115200);
    m_cprUSBPort->setFlowControl(FLOW_OFF);
    m_cprUSBPort->setDataBits(DATA_8);
    m_cprUSBPort->setParity(PAR_NONE);
    m_cprUSBPort->setStopBits(STOP_1);
    m_cprUSBPort->open(QextSerialPort::ReadWrite);
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
    m_readUSBData.append(m_cprUSBPort->readAll());

    if(m_readUSBData.contains(m_writeUARTData)) {
        m_semaphore.release();
        receivedUSB = true;
        qDebug("USB(CPR) Received from UART(CAN):\t %s", ((QString)m_readUSBData).toStdString().c_str());
    }
}

void TestCanCpr::receivedMessageCAN(void)
{
    m_readUARTData.append(m_canSerialPort->readAll());
    
    if(m_readUARTData.contains(m_writeUSBData)) {
        m_semaphore.release();
        receivedUART = true;
        qDebug("UART(CAN) Received from USB(CPR): \t %s", ((QString)m_readUARTData).toStdString().c_str());
    }
}

void TestCanCpr::runTest()
{
    receivedUSB = false;
    receivedUART = false;
    
    connect(m_cprUSBPort, SIGNAL(readyRead()), this, SLOT(receivedMessageCPR()));
    m_writeUARTData.append("0123456789abcdefloremipsumdolor");

    m_cprUSBPort->flush();
    m_readUSBData.clear();
    qDebug("Sending Data from UART(CAN) to USB(CPR): %s", ((QString)m_writeUARTData).toStdString().c_str());

    if (m_canSerialPort->isOpen() && m_canSerialPort->isWritable()) {
        m_canSerialPort->write(m_writeUARTData);
    } else {
        qDebug() << "Failed to send UART data. Serial port not open or not writable.";
    }

    if (not m_semaphore.tryAcquire(1, 5000))
    {
        m_reporter->testHasFailed(
            QString("CAN->CPR: Didn't receive any data!"));
    }

    m_canSerialPort->flush();
    m_cprUSBPort->flush();

    if(!receivedUSB) {
        m_reporter->testHasFailed(
            QString("CAN->CPR: Incoming data does not match. Received (%1), expected (%2)")
            .arg((QString)m_readUSBData).arg((QString)m_writeUARTData));
    }

    disconnect(m_cprUSBPort, SIGNAL(readyRead()), this, SLOT(receivedMessageCPR()));
    connect(m_canSerialPort, SIGNAL(readyRead()), this, SLOT(receivedMessageCAN()));

    m_readUARTData.clear();
    m_writeUSBData.clear();
    m_writeUSBData.append("sitamet,consecteturadipiscingelit");
    qDebug("Sending Data from USB(CPR) to UART(CAN): %s", ((QString)m_writeUSBData).toStdString().c_str());

    if (m_cprUSBPort->isOpen() && m_cprUSBPort->isWritable()) {
        m_cprUSBPort->write(m_writeUSBData);
    } else {
        qDebug() << "Failed to send USB data. Serial port not open or not writable.";
    }

    if (not m_semaphore.tryAcquire(1, 5000))
    {
        m_reporter->testHasFailed(
            QString("CPR->CAN: Didn't receive any data!"));
    }

    m_cprUSBPort->flush();

    if(!receivedUART) {
        m_reporter->testHasFailed(
            QString("CPR->CAN: Incoming data does not match. Received (%1), expected (%2)")
            .arg((QString)m_readUARTData).arg((QString)m_writeUSBData));
    }

    disconnect(m_canSerialPort, SIGNAL(readyRead()), this, SLOT(receivedMessageCAN()));
    m_readUARTData.clear();
    m_readUSBData.clear();
}
