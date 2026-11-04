#include <QtWidgets/QApplication>
#include <QDebug>

#include "mainwindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    if (!DatabaseManager::instance().initialize()) {
        qDebug() << "Error occured while initializing the database";
        return 0;
    }

    qDebug() << "Database Initilized successfully";

    MainWindow window;
    window.show();
    return app.exec();
}
