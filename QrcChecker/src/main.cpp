#include "QrcChecker.h"

#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QrcChecker w;
	w.show();

	return a.exec();
}
