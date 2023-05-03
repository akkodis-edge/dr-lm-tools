#include "testgpo.h"

#include "testmodewrapper.h"

TestGPO::TestGPO(IOCtrlCommController * const controller,
                 quint16 channel,
                 QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_channel(channel)
    , m_expectedLow(4096)
    , m_expectedHigh(0)
    , m_semaphore(0)
    , m_receivedValue(0)
{}

QString TestGPO::getName() const
{
    return QString("Analog_in6-Manikin_GPO%1").arg(m_channel);
}

void TestGPO::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestGPO::receivedAdcMessage(quint16 channel, quint16 value)
{
    if (channel == 9)           // Analog_in6 = 9
    {
        m_receivedValue = value;
        m_semaphore.release();
    }
}

bool TestGPO::closeEnough(quint16 input, quint16 target)
{
    return (input > 0.9*target)
        && (input < 1.1*target);
}

#define GET_ADC_VALUE \
    m_semaphore.release(m_semaphore.available());                       \
    m_controller->sendTestGetAdcCMD(9);                                 \
    if (not m_semaphore.tryAcquire(1, 1000))                            \
    {                                                                   \
        m_reporter->testHasFailed("Read analog value timed out.");      \
        return;                                                         \
    }

void TestGPO::runTest()
{
    quint16 pre;
    quint16 post;
    quint16 target = 1550;

    m_reporter->setLogTestHeader(getName() + QString(" (%1-%2),").arg(m_expectedLow).arg(m_expectedHigh));
    
    if (not connect(
            m_controller, SIGNAL(adcMessage(quint16, quint16)),
            this, SLOT(receivedAdcMessage(quint16, quint16)))
        )
    {
        m_reporter->testHasFailed("Failed to connect.");
        m_reporter->logResult("Error,");
        return;
    }

    // set ECG to 0
    system("echo 1 > /sys/devices/platform/pwm-manikin@0/period");

    m_controller->sendTestModeCMD(1); // on
    usleep(100000);
    m_controller->sendTestSetIOCMD(4, 1); // gpo1
    m_controller->sendTestSetIOCMD(5, 1); // gpo2
    usleep(100000);
    m_controller->sendTestSetIOCMD(3 + m_channel, 0);
    usleep(100000);
    GET_ADC_VALUE;
    post = m_receivedValue;
    m_reporter->logResult(QString("%1,").arg(m_receivedValue));
    if (not closeEnough(post, target)) // 0.332V±10%
    {
        m_reporter->testHasFailed(
            QString("Read value outside range. Got %1, Expected range (%2-%3)")
            .arg(post).arg(target*0.9).arg(target*1.1));
        return;
    }
    else
        qDebug("Analog_in6 with GPO%u: %u Expected range (%u-%u)", m_channel,
             m_receivedValue, int(target*0.9), int(target*1.1));

    //m_controller->sendTestModeCMD(2); // off
}
