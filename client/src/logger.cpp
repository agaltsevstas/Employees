#include "logger.h"
#include "utils.h"

#include <QDir>
#include <QThread>

constinit const auto DIRECTORY = "../log/";

QString Logger::_infoBuffer;
QString Logger::_warningBuffer;
QString Logger::_errorBuffer;
QString Logger::_allMessagesBuffer;
QScopedPointer<QFile> Logger::_file;
std::unique_ptr<Logger> Logger::_logger = nullptr;


void Logger::Instance()
{
    _logger.reset(new Logger());
    _debugLevel = DEBUG_LEVEL_INFO;

    const QString fileName = Utils::LocalTime() + ".log";

    QDir directory(DIRECTORY);
    if (!directory.exists())
        directory.mkpath(".");

    const QString filePath = QString(DIRECTORY) + fileName;
    _file.reset(new QFile(std::move(filePath)));
    _file->open(QFile::Append | QFile::Text);
    if (!_file->isOpen())
        qWarning() << "Logger file is not open";
}

void Logger::messageHandler(QtMsgType iMessageType, const QMessageLogContext&, const QString& iMessage)
{
    switch (iMessageType)
    {
        case QtInfoMsg:
        case QtDebugMsg:
            WriteInfo(iMessage);
            break;
        case QtWarningMsg:
            WriteWarning(iMessage);
            break;
        case QtCriticalMsg:
            WriteCritical(iMessage);
            break;
        case QtFatalMsg:
        default:
            abort();
    }
}

void Logger::SetDebugLevel(Logger::DebugLevel iDebugLevel) noexcept
{
    _debugLevel = iDebugLevel;
}

Logger::DebugLevel Logger::GetDebugLevel() noexcept
{
    return _debugLevel;
}

void Logger::WriteInfo(const QString& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_INFO)
    {
        const QString str = QString("[%1] %2\n").arg(Utils::LocalTime()).arg(iMessage);
        std::thread thread = std::thread([&str]() { WriteToFile(str); });
        WriteToBuffer(QtMsgType::QtInfoMsg, str);
        thread.join();
    }
}

void Logger::WriteWarning(const QString& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_WARNING)
    {
        const QString str = QString("[%1] [Warning] %2\n").arg(Utils::LocalTime()).arg(iMessage);
        std::thread thread = std::thread([&str]() { WriteToFile(str); });
        WriteToBuffer(QtMsgType::QtWarningMsg, str);
        thread.join();
    }
}

void Logger::WriteCritical(const QString& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_ERROR)
    {
        const QString str = QString("[%1] [Error] %2\n").arg(Utils::LocalTime()).arg(iMessage);
        std::thread thread = std::thread([&str]() { WriteToFile(str); });
        WriteToBuffer(QtMsgType::QtCriticalMsg, str) ;
        thread.join();
    }
}

void Logger::WriteToBuffer(QtMsgType iMessageType, const QString& message)
{
    _allMessagesBuffer += message;
    switch (static_cast<int>(iMessageType))
    {
        case QtInfoMsg:
            _infoBuffer += message;
            break;

        case QtWarningMsg:
            _warningBuffer += message;
            break;

        case QtCriticalMsg:
            _errorBuffer += message;
            break;
    }
}

void Logger::WriteToFile(const QString& iMessage)
{
    QTextStream(_file.get()) << iMessage;
}

void Logger::PrintInfo()
{
    qInfo() << (_infoBuffer.isEmpty() ? "Cообщения отсутствуют" : _infoBuffer);
}

void Logger::PrintWarning()
{
    qInfo() << (_warningBuffer.isEmpty() ? "Предупреждения отсутствуют" : _warningBuffer);
}

void Logger::PrintError()
{
    qInfo() << (_errorBuffer.isEmpty() ? "Ошибки отсутствуют" : _errorBuffer);
}

void Logger::PrintAllMessages()
{
    qInfo() << _allMessagesBuffer;
}

Logger::~Logger()
{
    _file->flush();
    _file->close();
}
