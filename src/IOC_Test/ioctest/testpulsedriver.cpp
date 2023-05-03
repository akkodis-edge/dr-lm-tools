#include "testpulsedriver.h"

#include "testmodewrapper.h"

TestPulseDriver::TestPulseDriver(IOCtrlCommController *controller,
                                 quint16 channel,
                                 quint16 expectedLow,
                                 quint16 expectedHigh,
                                 QObject *parent)
    : QObject(parent)
    , m_controller(controller)
    , m_reporter(0)
    , m_pdChannel(channel)
    , m_adcChannel(11)          // Analog_in1 = 11
    , m_expectedLow(expectedLow)
    , m_expectedHigh(expectedHigh)
    , m_semaphore(0)
{ }

QString TestPulseDriver::getName() const
{
    return QString("Pulse_driver_out%1").arg(m_pdChannel);
}

void TestPulseDriver::setReporter(ITestReporter *reporter)
{
    m_reporter = reporter;
}

void TestPulseDriver::receivedAdcMessage(quint16 channel, quint16 value)
{
    if (m_adcChannel == channel) {
        m_receivedValue = value;
        m_semaphore.release();
    }
}

quint16 TestPulseDriver::actualChannel(quint16 channel)
{
    if (channel >= 1 && channel <= 6) {
        return channel - 1;
    } else {
        return 0xFFFF;
    }
}

void TestPulseDriver::setPwmValue(quint16 channel, quint16 value)
{
    for (quint16 ch = 0; ch < 6; ch++) {
        quint16 pwmValue = 0x0000;
        if (ch == channel) {
            pwmValue = value;
        }
        m_controller->sendTestSetPwmCMD(ch, pwmValue);
    }
}

void TestPulseDriver::runTest()
{
    m_reporter->setLogTestHeader(getName() + QString(" (%1-%2),").arg(m_expectedLow).arg(m_expectedHigh));

    bool didConnect = connect(
                m_controller, SIGNAL(adcMessage(quint16, quint16)),
                this, SLOT(receivedAdcMessage(quint16, quint16)));

    if (not didConnect) {
        m_reporter->testHasFailed("Couldn't connect");
        
        return;
    }

    TestModeWrapper testmode(m_controller); // raii

    // Set pulse driver value to 0 and measure
    setPwmValue(actualChannel(m_pdChannel), 0);

    m_semaphore.release(m_semaphore.available()); // dangerous
    m_controller->sendTestGetAdcCMD(m_adcChannel);

    bool got_signal = m_semaphore.tryAcquire(1, 1000); // ms

    if (not got_signal) {
        m_reporter->testHasFailed("Read analog value timed out.");
        m_reporter->logResult("ERROR,");
        return;
    }

    qDebug("Analog value with 0 input: \t\t%u (%fV) Expected range (%u-%u)",
        m_receivedValue, ((float)m_receivedValue * 3.3 / 4096),
        m_expectedLow, m_expectedHigh);

    // Set pulse driver to value different from 0 and measure
    setPwmValue(actualChannel(m_pdChannel), 0x1000);

    m_controller->sendTestGetAdcCMD(m_adcChannel);

    got_signal = m_semaphore.tryAcquire(1, 1000); // ms

    if (not got_signal) {
        m_reporter->testHasFailed("Read analog value timed out.");
        m_reporter->logResult("ERROR,");
        return;
    }

    qDebug("Analog value with 0x1000 input: \t%u (%fV) Expected range (%u-%u)",
        m_receivedValue, ((float)m_receivedValue * 3.3 / 4096),
        m_expectedLow, m_expectedHigh);

    m_reporter->logResult(QString("%1,").arg(m_receivedValue));
    if (m_receivedValue >= m_expectedLow && m_receivedValue <= m_expectedHigh) {
        // ok!
    } else {
        m_reporter->testHasFailed(
            QString("Value %1 not in range (%2-%3)")
            .arg(m_receivedValue)
            .arg(m_expectedLow)
            .arg(m_expectedHigh)
            // .arg((float) m_receivedValue * 3.3 / 4096)
            );
        return;
    }
}
