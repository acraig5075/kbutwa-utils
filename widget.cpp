#include "widget.h"
#include "ui_widget.h"
#include "databasedlg.h"
#include <QSqlError>
#include <QDebug>

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
	if (db.isOpen())
	{
		QString status = QString("Connected to %1:%2 (%3)")
				.arg(credentials.server)
				.arg(credentials.database)
				.arg(credentials.user);
		ui->databaseLabel->setStyleSheet("QLabel { color : green; }");
		ui->databaseLabel->setText(status);
	}
	else
	{
		ui->databaseLabel->setStyleSheet("QLabel { color : red; }");
		ui->databaseLabel->setText("No database connection.");
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
		db = QSqlDatabase::addDatabase("QMYSQL");
		db.setHostName(credentials.server);
		db.setDatabaseName(credentials.database);
		db.setUserName(credentials.user);
		db.setPassword(credentials.pass);
		if (!db.open())
		{
			qDebug() << db.lastError();
		}

		// set database string
		setDatabaseStatus();
	}
}
