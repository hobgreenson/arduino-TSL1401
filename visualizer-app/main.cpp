#ifndef MAIN_CPP
#define MAIN_CPP

#include <QApplication>
#include "plotwindow.h"


int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	PlotWindow plotWin;	

	plotWin.show();

	return app.exec();

}

#endif // MAIN_CPP
