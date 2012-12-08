#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	// ∑¿÷π÷–Œƒ¬“¬Î
	QTextCodec *codec = QTextCodec::codecForName("GB2312");   

	QTextCodec::setCodecForLocale(codec);   
	QTextCodec::setCodecForCStrings(codec);   
	QTextCodec::setCodecForTr(codec); 
    MainWindow w;
    //w.show();
	w.showMaximized(); 
    return a.exec();
}
