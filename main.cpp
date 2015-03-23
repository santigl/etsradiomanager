#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Setting default system font for the whole application:
    /*QFont font;
    font.setFamily(font.defaultFamily());
    font.setHintingPreference(QFont::PreferDefaultHinting);
    a.setFont(font);*/

    MainWindow w;
    w.show();

    return a.exec();
}
