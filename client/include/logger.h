#ifndef LOGGER_H
#define LOGGER_H

#include <QScopedPointer>


class QFile;
class QMutex;

/*!
 * @brief Класс-синглтон, предназначенный для сбора и сохранения информации о работе программы.
 * Информация записывается в буфер и файлы
 */
class Logger
{
    Q_DISABLE_COPY_MOVE(Logger);
    friend std::default_delete<Logger>;
    friend class Session;

public:

    /*!
     * @brief Четыре уровня подробности лога:
     * DEBUG_LEVEL_DISABLED - Полное отключение лога
     * DEBUG_LEVEL_ERROR - Вывод только ошибок
     * DEBUG_LEVEL_WARNING - Вывод предупреждений и ошибок
     * DEBUG_LEVEL_INFO - Вывод сообщений
     */
    enum DebugLevel
    {
        DEBUG_LEVEL_DISABLED = 0,
        DEBUG_LEVEL_ERROR = 1,
        DEBUG_LEVEL_WARNING = 2,
        DEBUG_LEVEL_INFO = 3,
    };

    /*!
     * @brief Обработчик выводимых сообщения
     * @param iMessageType - тип сообщения
     * @param iContext - контекст
     * @param iMessage - записываемое сообщение
     */
    static void messageHandler(QtMsgType iMessageType, const QMessageLogContext& iContext, const QString& iMessage);

    /*!
     * Установление уровня подробности лога
     * @param iLevel - Уровень лога
     */
    static void SetDebugLevel(DebugLevel iLevel) noexcept;

    /*!
     * Получение уровня подробности лога
     * @return Уровень лога
     */
    static DebugLevel GetDebugLevel() noexcept;

private:
    static void Instance();

    Logger() {}
    ~Logger();

private:
    inline static DebugLevel _debugLevel = Logger::DebugLevel::DEBUG_LEVEL_DISABLED; // Уровень подробности лога
    static QString _infoBuffer;        // Буфер для хранения информационных сообщений
    static QString _warningBuffer;     // Буфер для хранения предупреждений
    static QString _errorBuffer;       // Буфер для хранения ошибок
    static QString _allMessagesBuffer; // Буфер для хранения всех видов сообщений
    static QScopedPointer<QFile> _file; // Выходной файловый поток
    static std::unique_ptr<Logger> _logger; // Объект-одиночка
    static QMutex _mutex;

    /*!
     * @brief Запись информационных сообщений.
     * Запись производится одновременно в файл и буфер в двух потоках
     * @param iMessage - Записываемое сообщение
     */
    static void WriteInfo(const QString& iMessage);

    /*!
     * @brief Запись предупреждений.
     * Запись производится одновременно в файл, на сервер и буфер в трех потоках
     * @param iMessage - Записываемое сообщение
     */
    static void WriteWarning(const QString& iMessage);

    /*!
     * @brief Запись ошибок.
     * Запись производится одновременно в файл, на сервер и буфер в трех потоках
     * @param iMessage - Записываемое сообщение
     */
    static void WriteCritical(const QString& iMessage);

    /*!
     * @brief Запись падений.
     * Запись производится только на сервер в отдельном потоке
     * @param iMessage - Записываемое сообщение
     */
    static void WriteFatal(const QString& iMessage);

    /*!
     * @brief Запись в буфер.
     * Запись производится в определенный буфер в зависимости от уровня вида сообщения
     * @param iMessageType - Тип сообщения
     * @param iMessage - Записываемое сообщение
     */
    static void WriteToBuffer(QtMsgType iMessageType, const QString& iMessage);

    /*!
     * @brief Запись в файл
     * @param iMessage - Записываемое сообщение
     */
    static void WriteToFile(const QString& iMessage);

    /*!
     * @brief Вывод информационных сообщений на экран
     */
    static void PrintInfo();

    /*!
     * @brief Вывод предупреждений на экран
     */
    static void PrintWarning();

    /*!
     * @brief Вывод ошибок на экран
     */
    static void PrintError();

    /*!
     * @brief Вывод всех сообщений (информационные сообщений, предупреждения, ошибки) на экран
     */
    static void PrintAllMessages();
};

#endif // LOGGER_H
