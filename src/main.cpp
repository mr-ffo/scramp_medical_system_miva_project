#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    // Initializes the Qt Event Loop for Event-Driven Programming[cite: 164]
    QApplication app(argc, argv);

    GUI::MainWindow window;
    window.show();

    return app.exec();
}