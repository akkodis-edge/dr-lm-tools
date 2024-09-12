#include "testgpo.h"
#include "testmodewrapper.h"
#include <QThread>
 
TestGPO::TestGPO(IOCtrlCommController * const controller,
                 quint16 channel,
                 quint16 expectedLow,
                 quint16 expectedHigh,
                 QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_channel(channel)
    , m_expectedLow(expectedLow)
    , m_expectedHigh(expectedHigh)
    , m_semaphore(0)
    , m_receivedValue(0)
{}
 
QString TestGPO::getName() const
{
    if (m_channel == 3) {
        return QString("Analog_in6-ECG");
    } else {
        return QString("Analog_in6-Manikin_GPO%1").arg(m_channel);
    }
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
 
bool TestGPO::closeEnough(quint16 input)
{
    return (input > m_expectedLow) && (input < m_expectedHigh);
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
    quint16 post;

    // Setting up the log header with expected range
    m_reporter->setLogTestHeader(getName() + QString(" (%1-%2),").arg(m_expectedLow).arg(m_expectedHigh));

    // Checking for signal-slot connection
    if (m_controller == nullptr || !connect(
            m_controller, SIGNAL(adcMessage(quint16, quint16)),
            this, SLOT(receivedAdcMessage(quint16, quint16)))
        )
    {
        m_reporter->testHasFailed("Failed to connect.");
        m_reporter->logResult("Error,");
        return;
    }

    // Enable test mode
    m_controller->sendTestModeCMD(1); // Test mode ON
    QThread::msleep(100); // Small delay to allow the mode change

    // Lambda to toggle ECG state
    auto setECGState = [](bool state) {
        if (state) {
            system("echo 1 > /sys/devices/platform/pwm-manikin@0/period"); // ECG on
        } else {
            system("echo 0 > /sys/devices/platform/pwm-manikin@0/period"); // ECG off
        }
    };

    // Lambda to measure ADC value and log results
    auto measureAndLog = [&]() {
        GET_ADC_VALUE;
        post = m_receivedValue;
        double measuredVoltage = static_cast<double>(m_receivedValue) / 4096.0 * 3.3;

        // Logging the result using qDebug
        qDebug("%s value: %u || %.2fV (Expected Range: %u-%u || %.2fV-%.2fV)",
               getName().toStdString().c_str(),
               post,
               measuredVoltage,
               m_expectedLow,
               m_expectedHigh,
               (((float)m_expectedLow * 3.3) / 4096),
               (((float)m_expectedHigh * 3.3) / 4096));

        // Checking if the received value is within the expected range
        if (!closeEnough(post)) {
            m_reporter->testHasFailed(
                QString("Read value outside range. Got %1, Expected range (%2-%3)")
                .arg(post).arg(m_expectedLow).arg(m_expectedHigh));
        } else {
            m_reporter->logResult(QString("%1,").arg(m_receivedValue));
        }
    };

    // Running tests based on channel
    if (m_channel == 1) {
        // Test 1
        setECGState(false); // ECG off
        m_controller->sendTestSetIOCMD(4, 1); // GPO1 on
        m_controller->sendTestSetIOCMD(5, 0); // GPO2 off
        QThread::msleep(100); // Wait 100ms
        measureAndLog();
    }
    else if (m_channel == 2) {
        // Test 2
        setECGState(false); // ECG off
        m_controller->sendTestSetIOCMD(4, 0); // GPO1 off
        m_controller->sendTestSetIOCMD(5, 1); // GPO2 on
        QThread::msleep(100); // Wait 100ms
        measureAndLog();
    }
    else if (m_channel == 3) {
        // Test 3
        setECGState(true); // ECG on
        m_controller->sendTestSetIOCMD(4, 0); // GPO1 off
        m_controller->sendTestSetIOCMD(5, 0); // GPO2 off
        QThread::msleep(100); // Wait 100ms
        measureAndLog();
    }

    // Reset pins to default state after tests
    setECGState(false); // ECG off
    m_controller->sendTestSetIOCMD(4, 0); // GPO1 off
    m_controller->sendTestSetIOCMD(5, 0); // GPO2 off

    // Turn off the test mode if needed
    m_controller->sendTestModeCMD(2); // Test mode OFF
}

