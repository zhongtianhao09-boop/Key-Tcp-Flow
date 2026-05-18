#include "ClientWindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);

    ClientWindow w;
    w.setWindowTitle("Flower - Key Agreement Client");
    w.resize(700, 500);
    w.show();

    return app.exec();
}
