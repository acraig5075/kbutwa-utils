#include "widget.h"
#include "ui_widget.h"
#include "databasedlg.h"

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	setDatabaseStatus();

	ui->searchLabel->hide();
	ui->previousSearchButton->hide();
	ui->nextSearchButton->hide();
}

Widget::~Widget()
{
	delete ui;
}

void Widget::setDatabaseStatus()
{
	if (credentials.database.isEmpty() || credentials.user.isEmpty())
	{
		ui->databaseLabel->setStyleSheet("QLabel { color : red; }");
		ui->databaseLabel->setText("No database connection.");
	}
	else
	{
		QString status = QString("Connected to %1:%2 (%3)")
				.arg(credentials.server)
				.arg(credentials.database)
				.arg(credentials.user);
		ui->databaseLabel->setStyleSheet("QLabel { color : green; }");
		ui->databaseLabel->setText(status);
	}
}

void Widget::on_databaseButton_clicked()
{
	DatabaseDlg *dlg = new DatabaseDlg(this, credentials);
	if (dlg->exec() == QDialog::Accepted)
	{
		// close current database
		// reset controls to default
		// open new database
		// set database string
		setDatabaseStatus();
	}
}
