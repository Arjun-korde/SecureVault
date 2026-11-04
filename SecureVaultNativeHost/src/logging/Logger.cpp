#include "Logger.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>

namespace Logger {

    void log(const QString& message)
    {
        QFile file("native_host.log");
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString(Qt::ISODate)
            << " | " << message << '\n';
    }

}
