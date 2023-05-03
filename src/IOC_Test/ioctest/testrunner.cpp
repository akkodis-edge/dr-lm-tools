#include "testrunner.h"
#include <QTimer>

#include <QDebug>
#include <textreporter.h>


TestRunner::TestRunner(QCoreApplication *app,
                       TextReporter *reporter,
                       QObject *parent)
    : QThread(parent)
    , m_application(app)
    , m_reporter(reporter)
    , m_tests()
{
    connect(this, SIGNAL(finished()),
            app, SLOT(quit()));
}

void TestRunner::addTest(ITestCase *test)
{
    m_tests.append(test);
}

void TestRunner::add(const QList<ITestCase *> &tests)
{
    m_tests.append(tests);
}

void TestRunner::runTests()
{
    QList<ITestCase *>::iterator it;
    for (it = m_tests.begin();
         it != m_tests.end();
         ++it)
    {
        ITestCase *test = (*it);
        m_reporter->setCurrentTestName(test->getName());
        test->setReporter(m_reporter);
        test->runTest();
        m_reporter->testHasRun();
        delete test;
    }
    m_reporter->printTestResults(true);
    m_reporter->saveResultsToFile();
}

void TestRunner::run()
{
    runTests();
}
