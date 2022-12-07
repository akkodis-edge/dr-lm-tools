/*
 * uart.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: rune
 */

#include <qextserialport.h>
#include <QDebug>
#include <stdio.h>

#include "uart.h"

Uart::Uart()
{
	// Open port for UART communication
	port = new QextSerialPort("/dev/ttymxc0", QextSerialPort::Polling);
	port->setBaudRate(BAUD9600);
	port->setFlowControl(FLOW_OFF);
	port->open(QextSerialPort::ReadWrite);

	port->flush();

	_isConnected = false;
}


/*
 * Waiting for incoming connection, then proceeds with a handshake
 */
int Uart::connect()
{
	int cmd = receive(); // syn
	if (cmd != CMD_SYN)
		return 0;

	send(CMD_SYNACK);

	cmd = receive(); // ack
	if (cmd != CMD_ACK)
		return 0;

	_isConnected = true;
	return 1;
}


int Uart::disconnect(bool ext)
{
	if (!ext) {
		// SEND FIN / ACK

		// return 1 if this fails
	}

	_isConnected = false;
	port->flush();

	return 0;
}


bool Uart::isConnected(void)
{
	return _isConnected;
}


int Uart::send(int msg)
{
	QByteArray data;
	data.append(CMD_FIRST_BYTE);
	data.append(msg);
	data.append(CMD_LAST_BYTE);
	port->write(data);

	return 0;
}


/*
 * TODO: Add timeout...?
 */
int Uart::receive()
{
	int msg = -1;
	int result = 0;

	while (result > -1 && result < 3) {
		result = port->bytesAvailable();
	}

	if (result >= 3) { // potentially a reply. Filter out unwanted messages
		QByteArray readData = port->readAll();
		if ((int)readData.at(0) == CMD_FIRST_BYTE && (int)readData.at(2) == CMD_LAST_BYTE)
			msg = (int)readData.at(1);
	}

	else if (result < 1) {
		qDebug() << "An error occurred while receiving command. debug=" << result;
		exit(0);
		//qFatal("An error occurred. Did not receive any data.");
	}

	port->flush();
	return msg;
}


int Uart::write(const char *str)
{
	return port->write(str);
}


int Uart::write(int num)
{
	char str[16] = "";
	sprintf(str, "%d", num);
	return port->write(str);
}


