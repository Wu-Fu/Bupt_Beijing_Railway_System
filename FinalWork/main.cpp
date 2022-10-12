#include "workwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WorkWindow w;
    w.show();
    return a.exec();
}
