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
					"QLineEdit, QTextEdit, QComboBox, QListBox, QTreeWidget, QTableView { "
						"border: 1px solid #3399FF; "
						"border-radius: 3px; "
					"}"
					"QSplitter::handle {"
						"background: solid lightgrey; "
						"border: 5px; "
					"}"
					"QPushButton { "
						"border: 2px solid #8f8f91; "
						"border-radius: 6px; "
						"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde); "
						"min-width: 80px;"
						"min-height: 26px;"
					"}"
					);

	Widget w;
	w.readPositionSettings();
	w.show();

	return a.exec();
}
