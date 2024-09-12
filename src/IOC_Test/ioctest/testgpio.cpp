#include "testgpio.h"

#include "testmodewrapper.h"

TestGPIO::TestGPIO(IOCtrlCommController * controller,
                   quint16 readChannel,
                   quint16 writeChannel,
                   QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_semaphore(0)
    , m_writeChannel(actualChannel(writeChannel))
    , m_readChannel(actualChannel(readChannel))
{ }

QString TestGPIO::getName() const
{
    return QString("MCU_GPIO%1").arg(m_readChannel-1);
}

void TestGPIO::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

quint16 TestGPIO::actualChannel(quint16 ch)
{
       return ch+1;
}

void TestGPIO::receivedGpioMessage(quint16 channel, quint16 value)
{
    // qDebug("%s - Ch:%u Val:%u", __func__, channel, value);
    if (channel == m_readChannel)
    {
        m_receivedValue = value;
        m_semaphore.release();
    }
    else
    {
        // qDebug("  not corrent read-channel");
    }
}

#define READ_VALUE \
    m_controller->sendTestGetIOCMD(m_readChannel);                      \
    if (not m_semaphore.tryAcquire(1, 1000))                            \
    {                                                                   \
        m_reporter->testHasFailed("Reply from IO controller timed out."); \
        return;                                                         \
    }

void TestGPIO::runTest(void)
{
     m_reporter->setLogTestHeader(getName() + QString(" (%1-%2),").arg(m_writeChannel).arg(m_readChannel));

    bool connected = connect(
        m_controller, SIGNAL(gpioMessage(quint16, quint16)),
        this, SLOT(receivedGpioMessage(quint16, quint16)));
    
    if (!connected)
    {
        m_reporter->testHasFailed("Failed to connect callback.");
        m_reporter->logResult("Error,");
        return;
    }

    TestModeWrapper testmode(m_controller);
    qDebug("<<Sending 0 to MCU_GPI0%u...>>", m_writeChannel-1);
    m_controller->sendTestSetIOCMD(m_writeChannel, 0);
    m_controller->sendTestSetIOCMD(m_readChannel, 0);

    // Read the initial value from the read channel
    READ_VALUE;

    qDebug() << getName().toStdString().c_str() << " read value: " << m_receivedValue;
    qDebug(" ");

    if (m_receivedValue != 0)
    {
        m_reporter->testHasFailed("Value from read channel not reset at start.");
        m_reporter->logResult("Error,");
        return;
    }

    // Release the semaphore and set the write channel to 1
    m_semaphore.release(m_semaphore.available());  // Reset semaphore
    qDebug("<<Sending 1 to MCU_GPI0%u...>>", m_writeChannel-1);
    m_controller->sendTestSetIOCMD(m_writeChannel, 1);

    // Read the value after setting the write channel to 1
    READ_VALUE;
    
    qDebug() << getName().toStdString().c_str() << " read value: " << m_receivedValue;
    // Log the actual value from the read channel
    m_reporter->logResult(QString("%1,").arg(m_receivedValue));
    
    if (m_receivedValue != 1)
    {
        m_reporter->testHasFailed(
            QString("Value from read channel not set. Received value: %1").arg(m_receivedValue));
        return;
    }
    qDebug(" ");
}
