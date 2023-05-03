#ifndef ITESTREPORTER_H
#define ITESTREPORTER_H

#include <QString>

class ITestReporter
{
public:
    virtual ~ITestReporter() {}

    virtual void setCurrentTestName(QString name) = 0;
    virtual void testHasFailed(QString why) = 0;
    virtual void logResult(QString value) = 0;
    virtual void setLogTestHeader(QString header) = 0;
    virtual void testHasRun() = 0;

    virtual bool allTestsPassed() = 0;

    // Usage by an test runner:
    // 1. setCurrentTestName(test->getName());
    // 2. test->setReporter(this);
    // 3. test->runTest(); //reports with testHas*
    // 4. implementation-specific calls to data
};

#endif /* ITESTREPORTER_H */
