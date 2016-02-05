#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setStyleSheet("QWidget#Widget, QDialog { "
					"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2, stop: 0 #C0C0C0, stop: 1 #FFFFFF); "
					"}"
					"QGroupBox { "
					"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2, stop: 0 #99CCFF, stop: 1 #FFFFFF); "
					"border: 2px solid #3399FF; "
					"border-radius: 5px; "
					"margin-top: 2ex"
					"}"
					"QLineEdit, QTextEdit { "
					"border: 1px solid gray; "
					"border-radius: 3px; "
					"}"
					"QSplitter::handle {"
					"background: solid lightgrey; "
					"border: 5px; "
					"}"
					);

	Widget w;
	w.show();

	return a.exec();
}
