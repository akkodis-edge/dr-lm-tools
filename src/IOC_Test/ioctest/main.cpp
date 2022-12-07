#include <QCoreApplication>
#include "ioctrlcommController.h"

#include <QFile>
#include <QStringList>
#include <QTimer>

// framework
#include "itestcase.h"
#include "testrunner.h"
#include "textreporter.h"

// tests
#include "testanalog.h"
#include "testcuff.h"
#include "testgpio.h"
#include "testgpoecg.h"
#include "testpso.h"
#include "testpulsedriver.h"
#include "testcancpr.h"

// Guard against accidentally triggering power
#include "safeguardpower.h"

// more
#include "pollanalog.h"
#include "pollgpio.h"
#include "pollpso.h"

void quitApplication(QCoreApplication *app)
{
    QTimer *timer = new QTimer(app);
    QObject::connect(timer, SIGNAL(timeout()),
                     app, SLOT(quit()),
                     Qt::QueuedConnection);
    timer->start(0);            // ms
}

#if 0
#ifdef VS_UNIX
extern "C" {
#include <signal.h>
void signal_handler(__attribute__((unused)) int a_signal)
{
    qDebug("Signal %d received!",a_signal);
    qApp->exit(1);
}
}
#endif
#endif
int main(int argc, char *argv[])
{
#if 0
#ifdef VS_UNIX
    // Register signal handler
    signal(SIGINT, &signal_handler);   // Ctrl-C
    signal(SIGTERM, &signal_handler);  // kill
#endif
#endif
    QCoreApplication a(argc, argv);
    a.setOrganizationName("DataRepons");
    a.setApplicationName("VitalSim2 BaseUnit Production Test");

    // Serial port to IOC
    IOCtrlCommController ioControl("/dev/ttymxc3");

    // parse cmd line arguments
    QStringList args = a.arguments();
    const char * usage = "\
\nUsage: ioctest [options]                                              \
\nOptions:                                                              \
\n  -r, --run            Run all tests.                                 \
\n  --fw                 Show current IO Controller firmware version.   \
\n  --poll-gpio=<ch>     Poll given GPIO channel.                       \
\n  --poll-pso=<ch>      Poll given PSO channel.                        \
\n  --poll-analog=<ch>   Poll given Analog channel.                     \
\n  -c, --color          Add colors to output.                          \
\n  -h, --help           Print this message and exit.\n";
    bool option_forceExit = false;
    bool option_help = args.size() == 1; // default yes if no args
    bool option_run = false;
    bool option_firmware = false;
    bool option_colors = false;
    bool option_pollgpio = false;
    quint16 option_pollgpio_channel = 0;
    bool option_pollpso = false;
    quint16 option_pollpso_channel = 0;
    bool option_pollanalog = false;
    quint16 option_pollanalog_channel = 0;
    QString option_flashFile = "";

    int _idx = 1;               // first in argv
    while (_idx < args.size()) {
        QString arg = args.at(_idx);
        if (arg == "-h" || arg == "--help") {
            option_help = true;
        } else if (arg == "-r" || arg == "--run") {
            option_run = true;
        } else if (arg == "--fw") {
            option_firmware = true;
        } else if (arg == "-c" || arg == "--color") {
            option_colors = true;
        }
        // else if (arg == "--test") {
        //     ioControl.sendTestModeCMD(0x01);              //Enter test mode
        //     ioControl.sendTestGetAdcCMD(11);              //Get An1
        //     ioControl.sendTestGetAdcCMD(12);              //Get An2
        //     ioControl.sendTestGetAdcCMD(4);               //Get An3
        //     ioControl.sendTestGetAdcCMD(15);              //Get An4
        //     ioControl.sendTestGetAdcCMD(8);               //Get An5
        //     ioControl.sendTestGetAdcCMD(9);               //Get An6
        //     ioControl.sendTestModeCMD(0x02);              //Exit test mode
        // }
        else if (arg.startsWith("--poll-gpio=")) {
            QString ch = arg.mid(12);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok) {
                option_pollgpio_channel = channel;
                option_pollgpio = true;
            } else {
                qDebug() << "Invalid channel:" << ch;
            }
        } else if (arg.startsWith("--poll-pso=")) {
            QString ch = arg.mid(11);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok) {
                option_pollpso_channel = channel;
                option_pollpso = true;
            } else {
                qDebug() << "Invalid PSO channel:" << ch;
            }
        } else if (arg.startsWith("--poll-analog=")) {
            QString ch = arg.mid(14);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok) {
                option_pollanalog_channel = channel;
                option_pollanalog = true;
            } else {
                qDebug() << "Invalid Analog channel:" << ch;
            }
        } else {
            qDebug() << "Unknown argument: " << arg;
        }
        _idx += 1;              // next arg
    }

    if (option_help) {
        qDebug() << usage;
        quitApplication(&a);
    }

    if (option_forceExit) {
        // Explicitly turn off all actions. This will enforce that we
        // do nothing, even after the Qt message loop is activated.
        option_run = false;
        option_firmware = false;
        quitApplication(&a);
    }

    // actions
    if (option_firmware) {
    	ioControl.sendReqUserSWver();
    }

    if (option_pollgpio) {
        qDebug() << "Polling GPIO channel" << option_pollgpio_channel;
        PollGPIO *pollgpio = new PollGPIO(&ioControl, option_pollgpio_channel, &a);
    }

    if (option_pollpso){
        qDebug() << "Polling PSO channel" << option_pollpso_channel;
        PollPSO *pollpso = new PollPSO(&ioControl, option_pollpso_channel, &a);
    }

    if (option_pollanalog) {
        qDebug() << "Polling ANALOG channel" << option_pollanalog_channel;
        PollAnalog *pollanalog = new PollAnalog(
            &ioControl, option_pollanalog_channel, &a);
    }

    if (option_run) {
        //SafeguardPower *safeguard = new SafeguardPower(&ioControl);
        //safeguard->start();

        TextReporter *reporter = new TextReporter(&a, option_colors);
        TestRunner *tests = new TestRunner(&a, reporter, &a);

        // Pulse Sense Oscillators
        tests->addTest(new TestPSO(&ioControl, 1, 5100)); // +/- 10%
        tests->addTest(new TestPSO(&ioControl, 2, 5100));
        tests->addTest(new TestPSO(&ioControl, 3, 5100));
        tests->addTest(new TestPSO(&ioControl, 4, 5100));
        tests->addTest(new TestPSO(&ioControl, 5, 5100));
        tests->addTest(new TestPSO(&ioControl, 6, 5100));

        // Pulse Drivers
        tests->addTest(new TestPulseDriver(&ioControl, 1, 50, 55));
        tests->addTest(new TestPulseDriver(&ioControl, 2, 50, 55));
        tests->addTest(new TestPulseDriver(&ioControl, 3, 50, 55));
        tests->addTest(new TestPulseDriver(&ioControl, 4, 50, 55));
        tests->addTest(new TestPulseDriver(&ioControl, 5, 50, 55));
        tests->addTest(new TestPulseDriver(&ioControl, 6, 50, 55));

        // Analog 2, 3, 4, and 5
        tests->addTest(new TestAnalog(&ioControl, 2, 2407, 2557)); // VBUS/11?
        tests->addTest(new TestAnalog(&ioControl, 3, 1204, 1278)); // VBUS/22?
        tests->addTest(new TestAnalog(&ioControl, 4, 2358, 2606)); // ~2V
        tests->addTest(new TestAnalog(&ioControl, 5, 2978, 3227)); // ~2.5V

        // GPIO 0 + 1
        tests->addTest(new TestGPIO(&ioControl, 0, 1)); // set gpio0, read gpio1
        tests->addTest(new TestGPIO(&ioControl, 1, 0)); // set gpio1, read gpio0

        // GPO-ECG (and analog_in6)
        tests->addTest(new TestGPOECG(&ioControl)); // gpo1, gpo2, ecg

        // Cuff sphygmamometer
        tests->addTest(new TestCuff(&ioControl, 40)); // above 40mmHg

        // CAN-CPR test
        tests->addTest(new TestCanCpr());

        // Run all tests. Controlled by own QThead, to ensure that the
        // Qt message pump is running.
        tests->start();
    }

    // qDebug() << "exec";
    return a.exec();
}
