#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("CodeHolmes"));
    QApplication::setApplicationVersion(QStringLiteral("0.0.1"));

    codeclean::MainWindow w;
    w.show();
    return app.exec();
}
