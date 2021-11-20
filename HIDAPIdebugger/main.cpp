#include "hidapidebugger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HIDAPIdebugger w;
    w.show();
    return a.exec();
}
