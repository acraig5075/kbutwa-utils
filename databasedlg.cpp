#include "databasedlg.h"
#include "ui_databasedlg.h"
#include <QProcess>

DatabaseDlg::DatabaseDlg(QWidget *parent, QString &dsn) :
	QDialog(parent),
	dsn(dsn),
	ui(new Ui::DatabaseDlg)
{
	ui->setupUi(this);

	ui->dsnEdit->setText(dsn);
}

DatabaseDlg::~DatabaseDlg()
{
	delete ui;
}

void DatabaseDlg::on_DatabaseDlg_accepted()
{
	dsn = ui->dsnEdit->text();
}

/* Nice idea: Iterate HKLM\Software\Wow6432Node\ODBC\ODBC.INI\ODBC Data Sources and populate combobox instead.
 */

void DatabaseDlg::on_odbcButton_clicked()
{
	QProcess *process = new QProcess(this);
	QString file = "c:\\windows\\sysWOW64\\odbcad32.exe";
	process->start(file);
}
