#include "NativeMessagingHost.h"
#include "logging/Logger.h"
#include "app/UserSession.h"

#include <iostream>
#include <vector>
#include <cstdint>

#include <QJsonDocument>
#include <QJsonObject>

NativeMessagingHost::NativeMessagingHost(QObject* parent)
    : QObject(parent)
{
    Logger::log("NativeMessagingHost started with Debug Mode");
}


void NativeMessagingHost::run()
{
    while (true) {
        QJsonObject request;

        if (!readMessage(request)) {
            Logger::log("Chrome disconnected. Exiting native host.");
            break;
        }

        const QString type = request.value("type").toString();
        Logger::log("Received message type: " + type);

        if (type == "CHECK_CREDENTIALS") {
            handleCheckCredentials(request);
        }
        else if (type == "SAVE_CONFIRMED") {
            handleSaveConfirmed(request);
        }
        else {
            Logger::log("Unknown message type ignored");
        }
    }
}


bool NativeMessagingHost::readMessage(QJsonObject& outRequest)
{

    uint32_t length = 0;

    std::cin.read(reinterpret_cast<char*>(&length), sizeof(length));

    if (std::cin.eof() || std::cin.fail()) {
        return false;
    }

    if (length == 0) {
        Logger::log("Received empty message length");
        return true;
    }

    std::vector<char> buffer(length);
    std::cin.read(buffer.data(), length);

    if (std::cin.fail()) {
        Logger::log("Failed to read message payload");
        return false;
    }

    QJsonDocument doc =
        QJsonDocument::fromJson(QByteArray(buffer.data(), static_cast<int>(length)));

    if (!doc.isObject()) {
        Logger::log("Invalid JSON received");
        return true;
    }

    outRequest = doc.object();
    return true;
}


void NativeMessagingHost::sendMessage(const QJsonObject& response)
{
    QByteArray json =
        QJsonDocument(response).toJson(QJsonDocument::Compact);

    uint32_t length = static_cast<uint32_t>(json.size());

    std::cout.write(reinterpret_cast<char*>(&length), sizeof(length));
    std::cout.write(json.constData(), json.size());
    std::cout.flush();

    Logger::log("Sent response: " + QString::fromUtf8(json));
}

void NativeMessagingHost::handleCheckCredentials(const QJsonObject& request)
{
    const QString domain = request.value("domain").toString();
    const int userId = request.value("userId").toInt();

    Logger::log(QString("CHECK_CREDENTIALS | userId=%1 | domain=%2")
        .arg(userId)
        .arg(domain));

    QJsonObject response;
    response["type"] = "CHECK_RESULT";

     auto entry = m_passwordRepo.findByDomain(userId, domain);
     if (!entry.has_value()) {
         response["found"] = false;
     }
     else {
         response["username"] = entry->username();
         response["password"] = entry->password();
         response["found"] = true;
     }
        
    sendMessage(response);
}

void NativeMessagingHost::handleSaveConfirmed(const QJsonObject& request)
{
    const QString domain = request.value("domain").toString();
    const QString username = request.value("username").toString();
    const QString password = request.value("password").toString();
    const int userId = request.value("userId").toInt();

    /*Logger::log(QString("SAVE_CONFIRMED | userId=%1 | domain=%2 | username=%3 | password=%4")
        .arg(user->id())
        .arg(domain)
        .arg(username)
        .arg(password));*/

     PasswordEntry entry(0, domain, username, password);
     m_passwordRepo.addPassword(userId, entry);

    QJsonObject response;
    response["type"] = "SAVE_COMPLETE";

    sendMessage(response);
}
