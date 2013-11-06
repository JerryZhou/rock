#include <QMainWindow>
#include <QApplication>

#include "test/testmain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // unit test entry
    test_main();
    
    return a.exec();
}