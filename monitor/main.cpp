#include "monitor.h"
#include <QApplication>
int main(int argc, char* argv[])
{

    QApplication app(argc,argv);
    Monitor monitor;
    monitor.show();
    app.exec();
    return 0;
}
