#pragma once

#include <QObject>
#include <QJsonObject>

#include "database/PasswordRepository.h"

class NativeMessagingHost : public QObject
{
    Q_OBJECT

public:
    NativeMessagingHost(QObject* parent = nullptr);
    void run();

private:
    
    PasswordRepository m_passwordRepo;
    bool m_testingEnabled = false;

    // Native Messaging IO
    bool readMessage(QJsonObject& outRequest);
    void sendMessage(const QJsonObject& response);

    // Message handlers
    void handleCheckCredentials(const QJsonObject& request);
    void handleSaveConfirmed(const QJsonObject& request);

};
