#include <QtCore/QCoreApplication>
#include <QSqlDatabase>

#include "src/native/NativeMessagingHost.h"
#include "database/UserRepository.h"
#include "logging/Logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (!DatabaseManager::instance().initialize()) {
        Logger::log("error occured while initializing the database");
        return 0;
    }

    /*UserRepository userRepo;
    userRepo.login("user2", "user2");*/

    NativeMessagingHost host;
    host.run();

    return 0;
}
