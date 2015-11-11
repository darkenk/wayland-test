#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "wayland");
    QApplication app(argc, argv);
    QWidget window;
    window.resize(200, 300);
    window.setStyleSheet("background-color:green;");
    window.setWindowTitle("qt_window");
    window.show();
    return app.exec();
}
