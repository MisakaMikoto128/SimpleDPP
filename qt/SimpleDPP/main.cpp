#include <QCoreApplication>
#include "iotest.hpp"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    iotest io(nullptr);
    return a.exec();
}
