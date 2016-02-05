#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setStyleSheet("QDialog { "
					"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2, stop: 0 #D0D0D0, stop: 1 #FFFFFF); "
					"}"
					"QGroupBox { "
					"background: lightBlue; "
					"border: 2px solid gray; "
					"border-radius: 5px; "
					"margin-top: 1ex; "
					"}"
					"QLineEdit { "
					"border: 1px solid gray; "
					"border-radius: 3px; "
					"}"
					"QTextEdit { "
					"border: 1px solid gray; "
					"border-radius: 3px; "
					"}"
					);

	Widget w;
	w.show();

	return a.exec();
}
