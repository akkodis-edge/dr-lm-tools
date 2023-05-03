#ifndef TEXTREPORTER_H
#define TEXTREPORTER_H

#include <QCoreApplication>
#include <QHash>
#include "itestreporter.h"

class TextReporter
    : public QObject,
      public ITestReporter
{
    Q_OBJECT
public:
    explicit TextReporter(QCoreApplication *app,
                          bool colorized = false,
                          bool fileOutput = false,
                          QObject *parent = 0);

public slots:
    virtual void setCurrentTestName(QString name);
    virtual void testHasFailed(QString why);
    virtual void logResult(QString value);
    virtual void setLogTestHeader(QString header);
    virtual void testHasRun();
    virtual bool allTestsPassed();

    QString systemCmdExec(const char* cmd);
    void printTestResults(bool verbose);
    void saveResultsToFile();

private slots:
    quint32 getFailedCount();

private:
    QString m_currentTest;
    QCoreApplication *m_application;
    struct TestData
    {
        QString name;
        bool ok;
        QString why;
        QString csvHeader;
        QString csvResult;
    };
    TestData *m_current;
    QList<TestData*> m_tests;
    // QHash<QString, bool> m_okTable;
    // QHash<QString, QString> m_errorTable;
    bool m_colorized;
    bool m_fileOutput;
};

#endif /* TEXTREPORTER_H */
