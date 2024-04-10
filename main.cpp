#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	
	MainWindow mW;
	mW.show();

	return a.exec();
}
