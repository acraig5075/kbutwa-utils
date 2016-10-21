#include "databasedlg.h"
#include "ui_databasedlg.h"
#include <QProcess>
#include <QSettings>
#include <QMessageBox>

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
	//QString file = "c:\\windows\\sysWOW64\\odbcad32.exe";
	QString file = "c:\\windows\\system32\\odbcad32.exe";
	bool started = QProcess::startDetached(file);
	if (!started)
	{
		QMessageBox::warning(this, "Warning", "ODBC Sources failed to start\nAre you running with elevated priviledges (i.e. as Administrator)?");
	}
}


QStringList DatabaseDlg::GetConnectionsFromRegistry()
{
    QSettings settings_x86("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\ODBC\\ODBC.INI\\ODBC Data Sources", QSettings::NativeFormat);
    QSettings settings_x64("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources", QSettings::NativeFormat);

    QStringList ret;
    ret += settings_x86.childKeys();
    ret += settings_x64.childKeys();
    return ret;
}
