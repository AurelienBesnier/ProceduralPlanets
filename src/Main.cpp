#include <QApplication>
#include <filesystem>
#include "Window.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;
    std::filesystem::path fs;
    std::string icon = fs.string()+"./icon.svg";

    w.setWindowIcon(QIcon(QString(icon.c_str())));
    w.show();
    return a.exec();
}
