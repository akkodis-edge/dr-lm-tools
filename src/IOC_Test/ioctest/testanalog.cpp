#include "testanalog.h"

#include "testmodewrapper.h"

TestAnalog::TestAnalog(IOCtrlCommController * controller,
                       quint16 channel,
                       quint16 expectedLow,
                       quint16 expectedHigh,
                       QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_channel(channel)
    , m_expectedLow(expectedLow)
    , m_expectedHigh(expectedHigh)
    , m_semaphore(0)
    , m_receivedValue(0)
{ }

QString TestAnalog::getName() const
{
    return QString("Analog_in%1").arg(m_channel);
}

quint16 TestAnalog::actualChannel(quint16 ch)
{
    switch (ch)
    {
    case 1: return 11;
    case 2: return 12;
    case 3: return 4;
    case 4: return 15;
    case 5: return 10;
    case 6: return 9;
    default:
        return 0;
    }
}

void TestAnalog::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestAnalog::runTest(void)
{
    // Precondition
    if (m_expectedLow > m_expectedHigh)
    {
        m_reporter->testHasFailed(
            QString("Invalid input range: %1-%2")
            .arg(m_expectedLow)
            .arg(m_expectedHigh));
        return;
    }

    m_reporter->setLogTestHeader(getName() + QString(" (%1-%2),").arg(m_expectedLow).arg(m_expectedHigh));

    connect(m_controller, SIGNAL(adcMessage(quint16, quint16))
            , this, SLOT(receivedAdcMessage(quint16, quint16)));

    TestModeWrapper testmode(m_controller);

    m_semaphore.release(m_semaphore.available());

    m_controller->sendTestGetAdcCMD(actualChannel(m_channel));
    bool got_signal = m_semaphore.tryAcquire(1, 1000); // ms
    if (not got_signal)
    {
        m_reporter->testHasFailed("Timed out.");
        m_reporter->logResult("ERROR,");
        return;
    }

    qDebug("Analog_in%u Value %u (%fV) Expected range (%u-%u)",
        m_channel,  m_receivedValue, ((float)m_receivedValue * 3.3 / 4096),
        m_expectedLow, m_expectedHigh);


    m_reporter->logResult(QString("%1,").arg(m_receivedValue));
    if ((m_receivedValue < m_expectedLow)
        || (m_receivedValue > m_expectedHigh))
    {
        m_reporter->testHasFailed(
            QString("Value %1 (%4V) not in range (%2-%3)")
            .arg(m_receivedValue)
            .arg(m_expectedLow)
            .arg(m_expectedHigh)
            .arg((float)m_receivedValue * 3.3 / 4096)
            );
        return;
    }
}

void TestAnalog::receivedAdcMessage(quint16 channel, quint16 value)
{
    if (channel == actualChannel(m_channel))
    {
        m_receivedValue = value;
        m_semaphore.release();
    }
}
