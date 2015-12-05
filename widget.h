#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "utils.h"


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

private:
	void setDatabaseStatus();

private slots:
	void on_databaseButton_clicked();

private:
	Ui::Widget *ui;
	Credentials credentials;
};

#endif // WIDGET_H
