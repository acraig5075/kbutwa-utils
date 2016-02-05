#include "databasedlg.h"
#include "ui_databasedlg.h"
#include <QProcess>
#include <QSettings>

DatabaseDlg::DatabaseDlg(QWidget *parent, QString &dsn) :
	QDialog(parent),
	dsn(dsn),
	ui(new Ui::DatabaseDlg)
{
	ui->setupUi(this);

	QStringList connections = GetConnectionsFromRegistry();

	ui->dsnCombo->addItems(connections);
	ui->dsnCombo->setCurrentText(dsn);
}

DatabaseDlg::~DatabaseDlg()
{
	delete ui;
}

void DatabaseDlg::on_DatabaseDlg_accepted()
{
	dsn = ui->dsnCombo->currentText();
}

void DatabaseDlg::on_odbcButton_clicked()
{
	QProcess *process = new QProcess(this);
	QString file = "\"c:\\windows\\sysWOW64\\odbcad32.exe\"";
	process->start(file);
	//QProcess::startDetached("\"c:\\windows\\sysWOW64\\odbcad32.exe\"");
}

QStringList DatabaseDlg::GetConnectionsFromRegistry()
{
	QSettings settings("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\ODBC\\ODBC.INI\\ODBC Data Sources", QSettings::NativeFormat);
	return settings.childKeys();
}
