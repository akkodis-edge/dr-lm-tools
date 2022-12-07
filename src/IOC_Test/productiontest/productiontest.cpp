#include "synchronousiocontroller.h"

#include <HALLib.h>
#include <Power.h>
#include <PWM.h>
#include <qextserialport.h>

#include <QtTest>

//! Expected values and deviations
struct AnalogInExpectedEntry
{
	unsigned expextedVal;		//! Nominal value
	float tolerance;			//! Allowed deviation
};

/*! Production test for VitalSim2 base unit

  This test case covers all the test that can run automatically with the
  manikin connector test fixture. The cuff is tested with a separate tool.
 */
class ProductionTest: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void pulseDriverOut();
    void pulseDetectIn();
    void analogIn2();
    void analogIn3();
    void analogIn4();
    void analogIn5();
    void mcuGpio0And1();
    void mcuGpio2ToPower();
    void manikinGposAndEcg();
    void canDriverAndCprMeter();
    void cleanupTestCase();

private:
    void manikinEcg(bool on);
    LM_VSHAL::Power *powerControl;
    SynchronousIoController *ioControl;
    QSocketNotifier *eventNotifier;
    int eventFileDescriptor;

};

static const AnalogInExpectedEntry analogExpectedTable[] =
{
    { 2256, 0.07 },
    { 2234, 0.07 },
    { 2234, 0.07 },
    { 2234, 0.07 },
    { 2234, 0.07 },
    { 2234, 0.07 },
};

void ProductionTest::initTestCase()
{
    bool didSetPower = false;

    try {
        powerControl = new LM_VSHAL::Power("/sys/class/gpio");

        if (powerControl->getManikinPower() == false) {
            powerControl->setManikinPower(true);
            didSetPower = true;
        }

        if (powerControl->getBoost12V() == false) {
            powerControl->setBoost12V(true);
            didSetPower = true;
        }
    } catch(LM_VSHAL::HALException e) {
        QFAIL("HAL exception");
    }

    ioControl = new SynchronousIoController("/dev/ttymcx3");
    ioControl->enterTestMode();

    eventFileDescriptor = open("/dev/input/event0", O_RDONLY);

    if (eventFileDescriptor < 0) {
        QFAIL("Could not connect to event0");
    }

    eventNotifier = new QSocketNotifier(eventFileDescriptor, QSocketNotifier::Read, this);

    if (didSetPower) {
        QTest::qWait(1000);  // Wait for voltages to stabilise
    }
	//system("echo 4 >/sys/kernel/debug/omap_mux/uart2_rts");
	//system("echo 145 >/sys/class/gpio/export");
	//system("echo out >/sys/class/gpio/gpio145/direction");
    system("echo 0 > /sys/class/pwm/pwmchip0/export");
    //system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable");

}

/*! Tests the pulse drivers

  This test will cycle through the pulse drivers 1-6 and turn them off. Then
  Pulse Drivers will be turned on one by one with full duty cycle and the
  resulting voltage will be measured on Analog In 1.

  Since the signals are inverted turning them off will deliver a voltage to the
  ADC. When pulse drivers are turned it will result in a voltage drop which is
  measured.

  \note The pulse driver channels are zero-indexed with regards to the function
  for setting the PWM.
*/
void ProductionTest::pulseDriverOut()
{
    bool pulseDriverTestsOk = true;
    const quint16 pulseDrvsCnt = 6;
    QVector<quint16> adcValues(pulseDrvsCnt);
    adcValues.fill(0);

    // All drivers off - initial state
    for (quint16 ch = 0; ch != pulseDrvsCnt; ++ch) {
        ioControl->setPwmValue(ch, 0);
    }

    QTest::qWait(10);

    unsigned offVal = ioControl->analogIn(AnalogIn1);
    QVERIFY2(  3232 <= offVal && offVal <= 3719, "Testing off value on Analog1");
    qDebug("All off, Analog In 1: %d", offVal);


    // To debug all measured values are printed
    for (quint16 ch = 0; ch != pulseDrvsCnt; ++ch) {
        ioControl->setPwmValue(ch, 0x03FF);  // Max duty cycle
        QTest::qWait(10);
        adcValues[ch] = ioControl->analogIn(AnalogIn1);
        qDebug("Pulse driver %d, Analog In 1: %d", (ch + 1), adcValues.at(ch));
        ioControl->setPwmValue(ch, 0);  // Turn off again
    }

    // Time for the verdict
    for (quint16 ch = 0; ch != pulseDrvsCnt; ++ch) {
    	unsigned deviation = analogExpectedTable[ch].tolerance * (float)analogExpectedTable[ch].expextedVal;
    	unsigned lowLimit = analogExpectedTable[ch].expextedVal - deviation;
    	unsigned highLimit = analogExpectedTable[ch].expextedVal + deviation;
        QString errMsg = QString("Failed while testing pulse driver %1 for value %2 <= %3 >= %4").arg(ch+1).arg(lowLimit).arg(adcValues.at(ch)).arg(highLimit);
        if(!(lowLimit <= adcValues.at(ch) && adcValues.at(ch) <= highLimit)) {
            QWARN(errMsg.toUtf8());
            pulseDriverTestsOk = false;
        }
    }
    QVERIFY2(pulseDriverTestsOk, QString("Test of all pulse drivers").toUtf8());
}

/*! Tests the pulse sense oscillators

  This test will cycle through the pulse sense oscillators (1-6) and get the
  their value. If the value is within acceptable range the test passes.

  \note The pulse sense oscillator channels are zero-indexed with regards to
  the function that reads the value.
*/
void ProductionTest::pulseDetectIn()
{
    bool pulseDetectTestsOk = true;
    const quint16 pulseSensorsCnt = 6;
    QVector<quint16> pulseValues(pulseSensorsCnt);
    pulseValues.fill(0);

    // To debug all measured values are printed
    for (quint16 ch = 0; ch != 6; ++ch) {
        pulseValues[ch] = ioControl->pulseDetectIn(ch);
        qDebug("Pulse value on %d: %d", (ch + 1), pulseValues.at(ch));
    }

    // Time for the verdict
    for (quint16 ch = 0; ch != 6; ++ch) {
        QString errMsg = QString("Failed while testing Pulse Detect %1").arg(ch + 1);
        if(!(45000 <= pulseValues.at(ch) && pulseValues.at(ch) <= 55000)) {
            QWARN(errMsg.toUtf8());
            pulseDetectTestsOk = false;
        }
    }
    QVERIFY2(pulseDetectTestsOk, QString("Test of all pulse detectIn").toUtf8());
}

/*! Tests analog in 2 */
void ProductionTest::analogIn2()
{
    quint16 adcValue = ioControl->analogIn(AnalogIn2);
    qDebug("ADC value from Analog In 2: %d", adcValue);
    QVERIFY(2234 < adcValue && adcValue < 2731);
}

/*! Tests analog in 3 */
void ProductionTest::analogIn3()
{
    quint16 adcValue = ioControl->analogIn(AnalogIn3);
    qDebug("ADC value from Analog In 3: %d", adcValue);
    QVERIFY(1117 <= adcValue && adcValue <= 1365);
}

/*! Tests analog in 4 */
void ProductionTest::analogIn4()
{
    quint16 adcValue = ioControl->analogIn(AnalogIn4);
    qDebug("ADC value from Analog In 4: %d", adcValue);
    QVERIFY(2405 <= adcValue && adcValue <= 2658);
}

/*! Tests analog in 5

\note This test requires the CPR cable to be plugged.
*/
void ProductionTest::analogIn5()
{
    quint16 adcValue = ioControl->analogIn(AnalogIn5);
    qDebug("ADC value from Analog In 5: %d", adcValue);
    QVERIFY(3006 <= adcValue && adcValue <= 3323);
}

/*! Tests MCU_GPIO0 and MCU_GPIO1

  The test checks both directions, GPIO0 to GPIO1 and vice versa. Both high and
  low levels are checked.
 */
void ProductionTest::mcuGpio0And1()
{
    quint16 input = 2; // Different from 0 and 1

    // Output on MCU GPIO 1, reading on MCU GPIO 0
    ioControl->setGpio(McuGpio0, 0);

    ioControl->setGpio(McuGpio1, 0);
    input = ioControl->gpio(McuGpio0);
    QVERIFY2(input == 0, "MCU GPIO0 not 0");

    ioControl->setGpio(McuGpio1, 1);
    input = ioControl->gpio(McuGpio0);
    QVERIFY2(input == 1, "MCU GPIO0 not 1");


    // Output on MCU GPIO 0, reading on MCU GPIO 1
    ioControl->setGpio(McuGpio1, 0);

    ioControl->setGpio(McuGpio0, 0);
    input = ioControl->gpio(McuGpio1);
    QVERIFY2(input == 0, "MCU GPIO1 not 0");

    ioControl->setGpio(McuGpio0, 1);
    input = ioControl->gpio(McuGpio1);
    QVERIFY2(input == 1, "MCU GPIO1 not 1");
}


void ProductionTest::mcuGpio2ToPower()
{
    QSignalSpy spy(eventNotifier, SIGNAL(activated(int)));

    eventNotifier->setEnabled(true);
    ioControl->setGpio(McuGpio2, 0);  // Inverted signal to power button

    QTest::qWait(100);

    ioControl->setGpio(McuGpio2, 1);
    eventNotifier->setEnabled(false);

    QVERIFY2(spy.count(), "Didn't receive the event.");

    if (spy.count()) {
        input_event ev;
        read(eventFileDescriptor, &ev, sizeof(input_event));
        QVERIFY2(ev.type == EV_KEY, "Incorrect event type received.");
        QVERIFY2(ev.value == 1, "Incorrect value received.");
    }
}

void ProductionTest::manikinGposAndEcg()
{
    bool mankinGposAndEcgTestsOk = true;
    quint16 adcValue = 0;
    manikinEcg(false);
    ioControl->setGpio(ManikinGpo1, 0);
    ioControl->setGpio(ManikinGpo2, 0);
    QTest::qSleep(10);
    adcValue = ioControl->analogIn(AnalogIn6);

    QString errMsg = QString("GPO1 and GPO2 to 0 ADC = %1").arg(adcValue);
    qDebug() << errMsg;
    if(!(3679 <= adcValue && adcValue <= 4233)) {
        QWARN(QString("Failed, wrong ADC value when GPO1 and GPO2 to 0").toUtf8());
        mankinGposAndEcgTestsOk = false;
    }

    ioControl->setGpio(ManikinGpo1, 1);
    QTest::qSleep(10);
    adcValue = ioControl->analogIn(AnalogIn6);

    errMsg = QString("GPO1 set ADC = %1").arg(adcValue);
    qDebug() << errMsg;
    if(!(2676 <= adcValue && adcValue <= 3079)) {
        QWARN(QString("Failed, wrong ADC value when GP01 set").toUtf8());
        mankinGposAndEcgTestsOk = false;
    }

    ioControl->setGpio(ManikinGpo2, 1);
    QTest::qSleep(10);
    adcValue = ioControl->analogIn(AnalogIn6);

    errMsg = QString("GPO1 and GPO2 set ADC = %1").arg(adcValue);
    qDebug() << errMsg;
    if(!(2102 <= adcValue && adcValue <= 2419)) {
        QWARN(QString("Failed, wrong ADC value when GPO1 and GPO2 set").toUtf8());
        mankinGposAndEcgTestsOk = false;
    }

    manikinEcg(true);
    QTest::qSleep(10);
    adcValue = ioControl->analogIn(AnalogIn6);

    errMsg = QString("GPO1 and GPO2 to 1, ECO to 1, ADC = %1").arg(adcValue);
    qDebug() << errMsg;
    if(!(2102 <= adcValue && adcValue <= 2418)) {
        QWARN(QString("Failed, wrong ADC value when GPO1, GPO2 and EC0 is set").toUtf8());
        mankinGposAndEcgTestsOk = false;
    }

    manikinEcg(false);
    ioControl->setGpio(ManikinGpo1, 0);
    ioControl->setGpio(ManikinGpo2, 1);
    QTest::qSleep(10);

    QVERIFY2(mankinGposAndEcgTestsOk, QString("Testing mankinGposAndEcg tests").toUtf8());

}

void ProductionTest::manikinEcg(bool on)
{
	if (on)
		//system("echo 1 >/sys/class/gpio/gpio145/value");
        system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable");
	else
		//system("echo 0 >/sys/class/gpio/gpio145/value");
        system("echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable");
}

void ProductionTest::canDriverAndCprMeter()
{
    // TODO: Find the right tty port for 
    QextSerialPort canPort("/dev/ttyO1", QextSerialPort::Polling);
    canPort.setBaudRate(BAUD57600);
    canPort.setFlowControl(FLOW_OFF);
    canPort.open(QextSerialPort::ReadWrite);

    QextSerialPort cprPort("/dev/ttymxc0", QextSerialPort::Polling);
    cprPort.setBaudRate(BAUD57600);
    cprPort.setFlowControl(FLOW_OFF);
    cprPort.open(QextSerialPort::ReadWrite);

    QByteArray writeData("0123456789abcdefloremipsumdolor");

    cprPort.flush();
    canPort.write(writeData);

    const int maxAttempts = 20;
    int attempts = 0;

    while (cprPort.bytesAvailable() == 0 && attempts != maxAttempts) {
        QTest::qWait(20);
        attempts++;
    }

    if (attempts == maxAttempts)
        QFAIL("Didn't receive any data!");

    QByteArray readData = cprPort.readAll();
    QVERIFY(readData.contains(writeData));

    writeData.append("more");
    canPort.flush();
    cprPort.write(writeData);

    attempts = 0;

    while (canPort.bytesAvailable() == 0 && attempts != maxAttempts) {
        QTest::qWait(20);
        attempts++;
    }

    if (attempts == maxAttempts)
        QFAIL("Didn't receive any data!");

    readData = canPort.readAll();
    QVERIFY(readData.contains(writeData));

    canPort.close();
    cprPort.close();
}

void ProductionTest::cleanupTestCase()
{
    ioControl->exitTestMode();
    ioControl->exit();

    close(eventFileDescriptor);
    //system("echo 2 >/sys/kernel/debug/omap_mux/uart2_rts");
    //system("echo in >/sys/class/gpio/gpio145/direction");
    //system("echo 145 >/sys/class/gpio/unexport");

    system("echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable");
    system("echo 0 > /sys/class/pwm/pwmchip0/unexport");

}




QTEST_MAIN(ProductionTest)
#include "productiontest.moc"

