#include "textreporter.h"

#include <iostream>
#include <QDebug>
#include <QFile>
#include <QHashIterator>
#include <QDateTime>

TextReporter::TextReporter(QCoreApplication *app,
                           bool colorized,
                           bool fileOutput,
                           QObject *parent)
    : QObject(parent)
    , m_currentTest()
    , m_application(app)
    , m_current(0)
    , m_tests()
    , m_colorized(colorized)
    , m_fileOutput(fileOutput)
{
}

void TextReporter::setCurrentTestName(QString name)
{
    // if (m_okTable.contains(name))
    // {
    //     qDebug() << QString("The test %1 is already run").arg(name);
    //     return;
    // }
    m_current = new TestData();
    m_current->name = name;
    m_current->ok = true;
    m_current->why = QString();
    m_current->csvResult = QString();
    // m_tests.append();
    // m_currentTest = name;
    // m_okTable[name] = true;     // assume good
    // m_errorTable[name] = "";
    m_current->csvHeader = QString();
}


void TextReporter::testHasFailed(QString why)
{
    m_current->ok = false;
    m_current->why = why;
    // m_okTable[m_currentTest] = false;

    qDebug("%s failed:\n    %s",
           m_current->name.toUtf8().constData(),
           why.toUtf8().constData());
    // m_errorTable[m_currentTest] = why;
}

void TextReporter::setLogTestHeader(QString header)
{
    m_current->csvHeader = header;
}

void TextReporter::logResult(QString result)
{
    m_current->csvResult += result;
}

void TextReporter::testHasRun()
{
    // m_okTable[m_currentTest] = true;
    m_tests.append(m_current);
    // m_currentTest = "NULL";
}

bool TextReporter::allTestsPassed()
{
    return getFailedCount() == 0;
}

quint32 TextReporter::getFailedCount()
{
    quint32 count = 0;
    QList<TestData*>::const_iterator it;
    for (it = m_tests.begin(); it != m_tests.end(); ++it)
    {
        if (not (*it)->ok)
            count++;
    }
    return count;
}

QString TextReporter::systemCmdExec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    size_t pos = std::string::npos;

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // remove trailing new line
    pos = result.find_last_of('\n');
    
    // If the newline character was found, erase it and everything after it
    if (pos != std::string::npos) {
        result.erase(pos);
    }
    
    return QString(result.c_str());
}

void TextReporter::printTestResults(bool verbose)
{
    const char * red    = m_colorized ? "\033[31;1m" : "";
    const char * green  = m_colorized ? "\033[32;1m" : "";
    const char * normal = m_colorized ? "\033[0m"    : "";
    //const char * bold   = m_colorized ? "\033[1m"    : "";

    quint32 failedCount = getFailedCount();

    qDebug("\n=====\n\nTest summary:");  // TODO bold
    qDebug("    Tests executed: %3d", m_tests.count());
    qDebug("    Tests passed:   %3d", (m_tests.count() - failedCount));

    if (failedCount == 0)     // ok
    {
        qDebug("    Tests failed:   %s%3d%s", green, failedCount, normal);
        qDebug("\n    %sAll tests passed!%s", green, normal);
    }
    else
    {
        qDebug("    Tests failed:   %s%3d%s", red, failedCount, normal);
        // qDebug("%sSome tests failed:%s", red, normal);
        // QHashIterator<QString, bool> it(m_okTable);
        // while (it.findNext(false))
        // {
        //     qDebug("    %s", it.key().toUtf8().data());
        // }
    }

    if (verbose)
    {
        qDebug("\nTest overview:"); // TODO bold
    }
    else
    {
        qDebug("\nFailed tests:");
    }
    QList<TestData*>::const_iterator it;
    for (it = m_tests.begin();
         it != m_tests.end();
         ++it)
    {
        if ((*it)->ok)
        {
            if (verbose)
            {
                qDebug("    %s: %s%s%s",
                       (*it)->name.toUtf8().constData(),
                       green, "OK", normal);
            }
        }
        else
        {
            qDebug("    %s: %s%s%s",
                   (*it)->name.toUtf8().constData(),
                   red, "FAILED", normal);
        }
    }
}

void TextReporter::saveResultsToFile()
{
    if(!m_fileOutput)
        return;
    
    // Get the current date and time
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("yyyy-MM-dd_hh-mm-ss");

    // Construct the file name with the serial number and current date and time
    QString serialCmd = "nvram --get SYS_LM_PRODUCT_SERIAL";
    QString serialNumber = systemCmdExec(serialCmd.toStdString().c_str());
    QString fileLocation = "/home/root/";
    QString fileName = fileLocation + "log_ioctest_" + serialNumber + ".csv";

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    QTextStream out(&file);

    // Write the text to the file
    out << dateTimeString << " " << serialNumber << "\n";

    QList<TestData*>::const_iterator it;
    for (it = m_tests.begin();
         it != m_tests.end();
         ++it)
    {
        out << (*it)->csvHeader;
    }

    out << "\n";

    for (it = m_tests.begin();
         it != m_tests.end();
         ++it)
    {
        out << (*it)->csvResult;
    }
    out << "\n\n";

    // Close the file
    file.close();

    std::cout << "Test output file:" << fileName.toStdString() << std::endl;
}
