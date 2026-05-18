#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("flower-server");

    MainWindow w;
    w.setWindowTitle("Flower - Key Management Console");
    w.resize(960, 640);
    w.show();

    return app.exec();
}
