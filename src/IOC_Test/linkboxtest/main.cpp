#include <QCoreApplication>
#include "linkboxtester.h"

//#include <QTimer>
//#include <QThread>
//#include <QTest>
//#include "ioctrlcommController.h"

//#include <QFile>
//#include <QStringList>
//#include <QTimer>


/*void quitApplication(QCoreApplication *app)
{
    QTimer *timer = new QTimer(app);
    QObject::connect(timer, SIGNAL(timeout()),
                     app, SLOT(quit()),
                     Qt::QueuedConnection);
    timer->start(0);            // ms
}*/

int main(int argc, char *argv[])
{
	/*QCoreApplication a(argc, argv);
	LinkBoxTester lbt(&a);
	return a.exec();*/

    LinkBoxTester app(argc, argv);
    return app.exec();
}
