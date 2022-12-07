/*
 * linkboxtester.h
 *
 *  Created on: Oct 10, 2012
 *      Author: rune
 */

#ifndef LINKBOXTESTER_H_
#define LINKBOXTESTER_H_

#include <QCoreApplication>
#include <synchronousiocontroller.h>
#include "uart.h"

class LinkBoxTester : public QCoreApplication
{
	Q_OBJECT
public:
	explicit LinkBoxTester(int &argc, char *argv[]);
	//LinkBoxTester(QObject *parent);
	void run();
	virtual ~LinkBoxTester();

	qint16 audioQuality();
	qint16 analogInput(quint16);

private:
	SynchronousIoController *ioControl;
	Uart *uart;
};

#endif /* LINKBOXTESTER_H_ */
