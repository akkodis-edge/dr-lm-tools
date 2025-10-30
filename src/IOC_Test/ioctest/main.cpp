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
#include "testgpo.h"
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
    QString commPort = "/dev/ttymxc3";  // Default port
    a.setOrganizationName("DataRepons");
    a.setApplicationName("VitalSim2 BaseUnit Production Test");

    // parse cmd line arguments
    QStringList args = a.arguments();
    const char * usage = "\
\nUsage: ioctest [options]                                              						\
\nOptions:                                                              						\
\n  -r, --run            Run all tests except cuff. Cuff needs to be run with --cuff			\ 
\n  --fw                 Show current IO Controller firmware version.   						\
\n  -f --file            Save results in a file                         						\
\n  --poll-gpio=<ch>     Poll given GPIO channel.                       						\
\n  --poll-pso=<ch>      Poll given PSO channel.                        						\
\n  --poll-analog=<ch>   Poll given Analog channel.                     						\
\n  --pulse-driver=<ch>  Run the Pulse Driver test for given channel (1-6).   						\
\n  --pso=<ch>           Run the PSO test for given channel (1-6).      						\
\n  --analog=<ch>        Run the Analog test for given channel (2-5).   						\
\n  --gpo-ecg=<ch>       Run the AnalogIn6-GPO test for given channel (1-2) and AnalogIn6-ECG test for channel 3. 	\
\n  --gpio=<ch>          Run the GPIO test for given channel (0-1).     						\
\n  --can-cpr            Run the CAN-CPR test.                          						\
\n  --cuff               Run the Cuff test.                            							\
\n  -c, --color          Add colors to output.                          						\
\n  --com-port=DEV       Set serial port (default /dev/ttymxc3)                                                         \
\n  -h, --help           Print this message and exit.\n";
    bool option_forceExit = false;
    bool option_help = args.size() == 1; // default yes if no args
    bool option_run = false;
    bool option_firmware = false;
    bool option_colors = false;
    bool option_file_out = false;
    bool option_pollgpio = false;
    bool option_pulsedriver = false;
    quint16 option_pulsedriver_channel = 0;
    bool option_pso = false;
    quint16 option_pso_channel = 0;
    quint16 option_pollgpio_channel = 0;
    bool option_pollpso = false;
    quint16 option_pollpso_channel = 0;
    bool option_pollanalog = false;
    quint16 option_pollanalog_channel = 0;
    bool option_analog = false;
    quint16 option_analog_channel = 0;
    bool option_gpo_ecg = false;
    quint16 option_gpo_ecg_channel = 0;
    bool option_gpio = false;
    quint16 option_gpio_channel = 0;
    bool option_cancpr = false;
    bool option_cuff = false;
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
        } else if (arg == "-f" || arg == "--file") {
            option_file_out = true;
        } else if (arg.startsWith("--com-port=")) {
            commPort = arg.mid(11);
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
        } else if (arg.startsWith("--pulse-driver=")) {
            QString ch = arg.mid(15);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok && channel >= 1 && channel <= 6) {
                option_pulsedriver_channel = channel;
                option_pulsedriver = true;
            } else {
                qDebug() << "Invalid Pulse Driver channel:" << ch  << "Channel must be 1-6";
            }
        } else if (arg.startsWith("--pso=")) {
            QString ch = arg.mid(6);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok && channel >= 1 && channel <= 6) {
                option_pso_channel = channel;
                option_pso = true;
            } else {
                qDebug() << "Invalid PSO channel:" << ch << "Channel must be 1-6";
            }
        } else if (arg.startsWith("--analog=")) {
            QString ch = arg.mid(9);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok && channel >= 2 && channel <= 5) {
                option_analog_channel = channel;
                option_analog = true;
            } else {
                qDebug() << "Invalid Analog channel:" << ch  << "Channel must be 2-5";
            }
        } else if (arg.startsWith("--gpo-ecg=")) {
            QString ch = arg.mid(10);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok && channel >= 1 && channel <= 3) {
                option_gpo_ecg_channel = channel;
                option_gpo_ecg = true;
            } else {
                qDebug() << "Invalid GPO-ECG channel:" << ch << "Channel must be 1-3";
            }
        } else if (arg.startsWith("--gpio=")) {
            QString ch = arg.mid(7);
            bool ok;
            quint16 channel = ch.toUInt(&ok);
            if (ok && (channel == 0 || channel == 1)) {
                option_gpio_channel = channel;
                option_gpio = true;
            } else {
                qDebug() << "Invalid GPIO channel:" << ch << "Channel must be 0-1";
            }
        } else if (arg == "--can-cpr") {
            option_cancpr = true;
        } else if (arg == "--cuff") {
            option_cuff = true;
        }
        
        else {
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

    // Serial port to IOC
    IOCtrlCommController ioControl(commPort);

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

    if (option_pulsedriver) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        tests->addTest(new TestPulseDriver(&ioControl, option_pulsedriver_channel, 2000, 2500));
        tests->start();
    }

    if (option_pso) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        tests->addTest(new TestPSO(&ioControl, option_pso_channel, 54000));
        tests->start();
    }

    if (option_analog) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        // Set the voltage limits based on channel
        quint16 lowerLimit, upperLimit;
        if (option_analog_channel == 2) {
            lowerLimit = 2375; upperLimit = 2750; // ~2.06V
        } else if (option_analog_channel == 3) {
            lowerLimit = 1170; upperLimit = 1430; // ~1.05V
        } else if (option_analog_channel == 4) {
            lowerLimit = 2234; upperLimit = 2730; // ~2V
        } else if (option_analog_channel == 5) {
            lowerLimit = 2792; upperLimit = 3413; // ~2.5V
        }
        tests->addTest(new TestAnalog(&ioControl, option_analog_channel, lowerLimit, upperLimit));
        tests->start();
    }

    if (option_gpo_ecg) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        // Set the voltage limits based on channel
        quint16 lowerLimit, upperLimit;
        if (option_gpo_ecg_channel == 1 || option_gpo_ecg_channel == 2) {
            lowerLimit = 2520; upperLimit = 3080; // ~2.25V
        } else if (option_gpo_ecg_channel == 3) {
            lowerLimit = 1845; upperLimit = 2255; // ~1.65V (ECG)
        }
        tests->addTest(new TestGPO(&ioControl, option_gpo_ecg_channel, lowerLimit, upperLimit));
        tests->start();
    }

    if (option_gpio) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        if (option_gpio_channel == 0) {
            tests->addTest(new TestGPIO(&ioControl, 0, 1)); // set gpio0, read gpio1
        } else {
            tests->addTest(new TestGPIO(&ioControl, 1, 0)); // set gpio1, read gpio0
        }
        tests->start();
    }

    if (option_cancpr) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        tests->addTest(new TestCanCpr());
        tests->start();
    }

    if (option_cuff) {
        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);
        tests->addTest(new TestCuff(&ioControl, 40)); // above 40mmHg
        tests->start();
    }

    if (option_run) {
        //SafeguardPower *safeguard = new SafeguardPower(&ioControl);
        //safeguard->start();

        TextReporter *reporter = new TextReporter(&a, option_colors, option_file_out);
        TestRunner *tests = new TestRunner(&a, reporter, &a);

        // Pulse Sense Oscillators - +/- 10%
        tests->addTest(new TestPSO(&ioControl, 1, 54000));
        tests->addTest(new TestPSO(&ioControl, 2, 54000));
        tests->addTest(new TestPSO(&ioControl, 3, 54000));
        tests->addTest(new TestPSO(&ioControl, 4, 54000));
        tests->addTest(new TestPSO(&ioControl, 5, 54000));
        tests->addTest(new TestPSO(&ioControl, 6, 54000));

         // Pulse Drivers - +/- 10%
        tests->addTest(new TestPulseDriver(&ioControl, 1, 2000, 2500)); // ~1.81V
        tests->addTest(new TestPulseDriver(&ioControl, 2, 2000, 2500)); // ~1.81V
        tests->addTest(new TestPulseDriver(&ioControl, 3, 2000, 2500)); // ~1.81V
        tests->addTest(new TestPulseDriver(&ioControl, 4, 2000, 2500)); // ~1.81V
        tests->addTest(new TestPulseDriver(&ioControl, 5, 2000, 2500)); // ~1.81V
        tests->addTest(new TestPulseDriver(&ioControl, 6, 2000, 2500)); // ~1.81V

        // Analog 2, 3, 4, and 5 - +/- 10%
        tests->addTest(new TestAnalog(&ioControl, 2, 2375, 2750)); // ~2.06V
        tests->addTest(new TestAnalog(&ioControl, 3, 1170, 1430)); // ~1.05V
        tests->addTest(new TestAnalog(&ioControl, 4, 2234, 2730)); // ~2V
        tests->addTest(new TestAnalog(&ioControl, 5, 2792, 3413)); // ~2.5V

        // GPO, ECG (and analog_in6) - +/- 10%
        tests->addTest(new TestGPO(&ioControl, 1, 2520, 3080)); // send on gpo1 - ~2.25V
        tests->addTest(new TestGPO(&ioControl, 2, 2520, 3080)); // send on gpo2 - ~2.25V
        tests->addTest(new TestGPO(&ioControl, 3, 1845, 2255)); // send on ECG  - ~1.65V

	// GPIO 0 + 1
        tests->addTest(new TestGPIO(&ioControl, 0, 1)); // set gpio0, read gpio1
        tests->addTest(new TestGPIO(&ioControl, 1, 0)); // set gpio1, read gpio0

        // CAN-CPR test
        tests->addTest(new TestCanCpr());

        // Run all tests. Controlled by own QThead, to ensure that the
        // Qt message pump is running.
        tests->start();
    }

    // qDebug() << "exec";
    return a.exec();
}
