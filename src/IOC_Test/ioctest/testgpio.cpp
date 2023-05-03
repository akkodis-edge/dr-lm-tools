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
    return QString("MCU_GPIO%1/%2").arg(m_readChannel).arg(m_writeChannel);
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
    if (channel == channel)
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
    // qDebug(">>> GPIO: write %u, read %u", m_writeChannel, m_readChannel);
    m_reporter->setLogTestHeader(getName() + QString(" (%1-%2),").arg(m_writeChannel).arg(m_readChannel));

    bool connected = connect(
        m_controller, SIGNAL(gpioMessage(quint16, quint16)),
        this, SLOT(receivedGpioMessage(quint16, quint16)));
    if (not connected)
    {
        m_reporter->testHasFailed("Failed to connect callback.");
        m_reporter->logResult("Error,");
        return;
    }

    TestModeWrapper testmode(m_controller);

    m_controller->sendTestSetIOCMD(m_writeChannel, 0);
    m_controller->sendTestSetIOCMD(m_readChannel, 0);

    READ_VALUE;
    if (m_receivedValue != 0)
    {
        m_reporter->testHasFailed("Value from read channel not reset at start.");
        m_reporter->logResult("Error,");
        return;
    }

    m_semaphore.release(m_semaphore.available()); // dangerous, but eh.
    m_controller->sendTestSetIOCMD(m_writeChannel, 1);

    READ_VALUE;
    m_reporter->logResult(QString("%1,").arg(m_receivedValue));
    if (m_receivedValue != 1)
    {
        m_reporter->testHasFailed(
            QString("Value from read channel not set. Received value: %1").arg(m_receivedValue));
        return;
    }
}
