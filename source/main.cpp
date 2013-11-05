#include <QMainWindow>
#include <QApplication>

#include "test/testmain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    w.show();
    
    // unit test entry
    test_main();
    
    return a.exec();
}