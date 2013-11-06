//#include <QMainWindow>
//#include <QApplication>

#include "test/testmain.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    //QCoreApplication a(argc, argv);
    
    // unit test entry
    test_main();
    
    return 0;//a.exec();
}