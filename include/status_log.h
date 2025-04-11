#ifndef STATUS_LOG_H
#define STATUS_LOG_H
#include <string>
#include <QString>
#include <QLabel>
#include <QDebug>

class Status_Log
{
private:
    QString error_;
    QString warning_;
    QString success_;
    QString refresh_;
public:
    Status_Log();
    void print_status(QLabel* target, const std::string type, const std::string message) noexcept;
};

#endif // STATUS_LOG_H
