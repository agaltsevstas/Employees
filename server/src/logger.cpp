#include "logger.h"
#include "utils.h"

#include <QDir>
#include <QMutex>
#include <QThread>


namespace
{
    constexpr auto SERVER_DIRECTORY = "../log/server/";
    constexpr auto CLIENT_DIRECTORY = "../log/client/";
}

QString Logger::_infoBuffer;
QString Logger::_warningBuffer;
QString Logger::_errorBuffer;
QString Logger::_allMessagesBuffer;
QScopedPointer<QFile> Logger::_serverFile;
QScopedPointer<QFile> Logger::_clientFile;
std::unique_ptr<Logger> Logger::_logger = nullptr;
QMutex Logger::_mutex;


void Logger::Instance()
{
    _logger.reset(new Logger());
    _debugLevel = DEBUG_LEVEL_INFO;

    const QString fileName = Utils::LocalTime() + ".log";

    QDir directory(SERVER_DIRECTORY);
    if (!directory.exists())
        directory.mkpath(".");

    const QString filePath = QString(SERVER_DIRECTORY) + fileName;
    _serverFile.reset(new QFile(std::move(filePath)));
    _serverFile->open(QFile::Append | QFile::Text);
    if (!_serverFile->isOpen())
        qWarning() << "Logger Server file is not open";
}

void Logger::messageHandler(QtMsgType iMessageType, const QMessageLogContext&, const QString& iMessage)
{
    QMutexLocker lock(&_mutex);
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

void Logger::WriteToClientFile(const QString& iMessage)
{
    if (iMessage.isEmpty())
        return;

    if (!_clientFile)
    {
        QDir directory(CLIENT_DIRECTORY);
        if (!directory.exists())
            directory.mkpath(".");

        const QString fileName = Utils::LocalTime() + ".log";
        const QString filePath = QString(CLIENT_DIRECTORY) + fileName;
        _clientFile.reset(new QFile(std::move(filePath)));
        _clientFile->open(QFile::Append | QFile::Text);
        if (_clientFile->isOpen())
            qWarning() << "Logger file is not open";
    }

    QTextStream(_clientFile.get()) << iMessage;
}

void Logger::WriteInfo(const QString& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_INFO)
    {
        const QString str = QString("[%1] %2\n").arg(Utils::LocalTime()).arg(iMessage);
        std::thread thread = std::thread([&str]() { WriteToServerFile(str); });
        WriteToBuffer(QtMsgType::QtInfoMsg, str);
        thread.join();
    }
}

void Logger::WriteWarning(const QString& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_WARNING)
    {
        const QString str = QString("[%1] [Warning] %2\n").arg(Utils::LocalTime()).arg(iMessage);
        std::thread thread = std::thread([&str]() { WriteToServerFile(str); });
        WriteToBuffer(QtMsgType::QtWarningMsg, str);
        thread.join();
    }
}

void Logger::WriteCritical(const QString& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_ERROR)
    {
        const QString str = QString("[%1] [Error] %2\n").arg(Utils::LocalTime()).arg(iMessage);
        std::thread thread = std::thread([&str]() { WriteToServerFile(str); });
        WriteToBuffer(QtMsgType::QtCriticalMsg, str);
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

void Logger::WriteToServerFile(const QString& iMessage)
{
    if (iMessage.isEmpty())
        return;

    QTextStream(_serverFile.get()) << iMessage;
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
    _serverFile->flush();
    _serverFile->close();

    _clientFile->flush();
    _clientFile->close();
}
