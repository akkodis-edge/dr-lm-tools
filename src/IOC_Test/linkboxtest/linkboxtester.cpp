/*
 * linkboxtester.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: rune
 */

#include <HALLib.h>
#include <Power.h>
#include <PWM.h>
#include <qextserialport.h>
#include <QCoreApplication>
#include <QDebug>
#include <QTest>
#include <QObject>
#include <QSignalSpy>
#include <QProcess>

#include "linkboxtester.h"
#include "uart.h"
#include "synchronousiocontroller.h"

static const int ANALOG_CHANNELS[] =
{
	0,
    AnalogIn1,
    AnalogIn2,
    AnalogIn3,
    AnalogIn4,
    AnalogIn5,
    AnalogIn6,
};

LinkBoxTester::LinkBoxTester(int &argc, char *argv[])
	: QCoreApplication(argc, argv)
{

	//LM_VSHAL::Power *powerControl;
	bool didSetPower = false;

    system("echo 1 >/sys/class/power_supply/MANIKIN_5V/online");
    system("echo 1 >/sys/class/power_supply/MANIKIN_12V/online");
    didSetPower = true;

	/*try {
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
		qFatal("HAL exception");
	}*/


	ioControl = new SynchronousIoController("/dev/ttymxc3");
	ioControl->enterTestMode();


	qDebug() << "Power initialized.";


	uart = new Uart();

	run();


	//QByteArray writeData("ack");
	//uartPort.write(writeData);
	//uartPort.write("test", 4);

	//qDebug() << "Finished writing data... (initial)";

	//qDebug() << "Waiting to receive data...";
	//qint64 result = 0;

	/*while (true)
	{
		while (!result) {
			result = uartPort.bytesAvailable();
		}
		if (result < 1) {
			qDebug() << "An error occurred: result=" << result;
			exit(0);
			//qFatal("An error occurred. Did not receive any data.");
		}

		QByteArray readData = uartPort.readAll();

		qDebug() << "Received " << result << " data: " << readData.toHex();
		result = 0;

		// Transmit data as well
		uartPort.write(writeData);
		//uartPort.write("test", 4);

	}*/
	/*qDebug() << "Waiting to receive data...";

	qint64 result = 0;
	while (!result) {
		result = uartPort.bytesAvailable();
	}

	if (result < 1)
		qDebug() << "An error occurred: result=" << result;
		//qFatal("An error occurred. Did not receive any data.");

	QByteArray readData = uartPort.readAll();

	qDebug() << "Received data: " << readData;
	*/



}


/*
 * TODO: This function should be replaced, by utilizing much more of the QT framework, such as signals/slots
 */
void LinkBoxTester::run()
{
	while (1)
	{

		qDebug() << "Waiting for Test Card connection...";

		while (!uart->connect());

		qDebug() << "Successfully connected to Test Card.";

		while (uart->isConnected())
		{
			qDebug() << "Waiting to receive a command...";

			quint16 cmd = uart->receive();

			qDebug() << "Received cmd:" << cmd;

			switch (cmd)
			{

			case CMD_SYN: // Other side has restarted without telling us. Finishing the handshake anyway.
				uart->send(CMD_SYNACK);
				cmd = uart->receive(); // should be: ack
				if (cmd != CMD_ACK)
					uart->disconnect(true); // something wrong happened
				break;

			case CMD_FINISH:
				uart->send(CMD_ACK);
				uart->disconnect(true);
				break;

			case CMD_AUDIO_START:
				audioQuality();
				break;

			case CMD_ANALOG_INPUT1:
			case CMD_ANALOG_INPUT2:
			case CMD_ANALOG_INPUT3:
			case CMD_ANALOG_INPUT4:
			case CMD_ANALOG_INPUT5:
			case CMD_ANALOG_INPUT6:
				analogInput( (cmd & 0x000F) );
				break;

			default:
				break;

			}
		}

		qDebug() << "Disconnected!";
	}
}



qint16 LinkBoxTester::audioQuality()
{
	qDebug() << "Starting Speaker output";
	qint16 reply = 0;

	QProcess *proc = new QProcess();
	proc->start("aplay", QStringList() << "-Dhw:0,0" << "/root/sine_8ch_16khz.wav");

	uart->send(CMD_AUDIO_START_ACK);

	if (uart->receive() == CMD_AUDIO_STOP) {
		reply = 0;
		uart->send(CMD_AUDIO_STOP_ACK);
		qDebug() << "Stopping Speaker output";
	}
	else {
		reply = -1;
		qDebug() << "Something went wrong when waiting for CMD_AUDIO_STOP...";
	}

	proc->close();

	return reply;
}



qint16 LinkBoxTester::analogInput(quint16 channel)
{
	qDebug() << "Testing analog input on channel" << channel;

	qint16 result = ioControl->analogIn(ANALOG_CHANNELS[channel]);
	uart->write(result);

	return 0; // improve
}




LinkBoxTester::~LinkBoxTester()
{
	uart->port->close();

	ioControl->exitTestMode();
	ioControl->exit();
}

