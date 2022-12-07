#include "testgpoecg.h"
#include <unistd.h>
#include "testmodewrapper.h"

TestGPOECG::TestGPOECG(IOCtrlCommController * const controller,
                 QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_expectedLow(4096)
    , m_expectedHigh(0)
    , m_semaphore(0)
    , m_receivedValue(0)
{}

QString TestGPOECG::getName() const
{
    return QString("Analog_in6, Manikin_GPO1_GPO2_ECG");
}

void TestGPOECG::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestGPOECG::receivedAdcMessage(quint16 channel, quint16 value)
{
    if (channel == 9)           // Analog_in6 = 9
    {
        m_receivedValue = value;
        m_semaphore.release();
    }
}

bool TestGPOECG::closeEnough(quint16 input, quint16 target)
{
    return (input > 0.9*target)
        && (input < 1.1*target);
}

void manikinEcg(bool on)
{
    if(on) {
        system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable");
    }
    else {
        system("echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable");
    }
}

#define GET_ADC_VALUE \
    m_semaphore.release(m_semaphore.available());                       \
    m_controller->sendTestGetAdcCMD(9);                                 \
    if (not m_semaphore.tryAcquire(1, 1000))                            \
    {                                                                   \
        m_reporter->testHasFailed("Read analog value timed out.");      \
        return;                                                         \
    }

void TestGPOECG::runTest()
{
    quint16 pre;
    quint16 post;

    system("echo 0 > /sys/class/pwm/pwmchip0/export");
    manikinEcg(false);

    if (not connect(
            m_controller, SIGNAL(adcMessage(quint16, quint16)),
            this, SLOT(receivedAdcMessage(quint16, quint16)))
        )
    {
        m_reporter->testHasFailed("Failed to connect.");
        system("echo 0 > /sys/class/pwm/pwmchip0/unexport");
        return;
    }

    m_controller->sendTestModeCMD(1); // on
    m_controller->sendTestSetIOCMD(4, 1); // gpo1
    m_controller->sendTestSetIOCMD(5, 1); // gpo2

    GET_ADC_VALUE;
    pre = m_receivedValue;
    if (not closeEnough(pre, 1490)) // 1.2V±10%
    {
        m_reporter->testHasFailed(
            QString("GPO1=1, GPO2=1, ECG=OFF. Initial value outside range. Got %1, expected %2.")
            .arg(pre).arg(1490));
    }

    manikinEcg(true);
    usleep(10000);

    GET_ADC_VALUE;
    pre = m_receivedValue;
    if (not closeEnough(pre, 1580)) // 1.2V±10%
    {
        m_reporter->testHasFailed(
            QString("GPO1=1, GPO2=1, ECG=ON. Initial value outside range. Got %1, expected %2.")
            .arg(pre).arg(1580));
    }

    manikinEcg(false);
    usleep(10000);

    m_controller->sendTestSetIOCMD(4, 0);
    GET_ADC_VALUE;
    post = m_receivedValue;
    if (not closeEnough(post, 412)) // 0.332V±10%
    {
        m_reporter->testHasFailed(
            QString("GPO1=0, GPO2=1, ECG=OFF. Read value outside range. Got %1, expected %2.")
            .arg(post).arg(412));
    }

    m_controller->sendTestSetIOCMD(4, 1);
    m_controller->sendTestSetIOCMD(5, 0);

    GET_ADC_VALUE;
    post = m_receivedValue;
    if (not closeEnough(post, 412)) // 0.332V±10%
    {
        m_reporter->testHasFailed(
            QString("GPO1=1, GPO2=0, ECG=OFF. Read value outside range. Got %1, expected %2.")
            .arg(post).arg(412));
    }

    system("echo 0 > /sys/class/pwm/pwmchip0/unexport");
}
