#include <iostream>
#include <QWindow>
#include <QApplication>
//#include <QMainWindow>

int main(int argc, char *argv[]){
	QApplication app(argc, argv);

	QWindow window;
	window.resize(256,256);
	window.show();
	return app.exec();
}
