#include "../include/status_log.h"

Status_Log::Status_Log() {
    success_="✅ ";
    warning_="⚠️ ";
    error_="❌ ";
    refresh_="🌀 ";
}

void Status_Log::print_status(QLabel* target, const std::string type, const std::string message) noexcept{
    // Abbruch bei leerer Message
    if (message == ""){
        qDebug() << "Message is empty";
        return;
    }
    // Abruch bei nullptr
    QString out{};
    // Test für Message Typ
    if (type == "SUCCESS"){
        out = this->success_;
    }
    else if (type == "WARNING"){
        out = this->warning_;
    }
    else if (type == "ERROR"){
        out = this->error_;
    }
    else if (type == "REFRESH"){
        out = this->refresh_;
    }
    // Abbruch bei falschem Typ
    else {
        qDebug() << "No valid message type has been submitted. Options: ['SUCCESS','WARNING','ERROR','REFRESH'].";
        return;
    }
    out += QString::fromStdString(message);
    target->setText(out);
}
