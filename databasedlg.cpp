#include "databasedlg.h"
#include "ui_databasedlg.h"

DatabaseDlg::DatabaseDlg(QWidget *parent, Credentials &credentials) :
	QDialog(parent),
	credentials(credentials),
	ui(new Ui::DatabaseDlg)
{
	ui->setupUi(this);

	ui->userEdit->setText(credentials.user);
	ui->passEdit->setText(credentials.pass);
	ui->serverEdit->setText(credentials.server);
	ui->databaseEdit->setText(credentials.database);
}

DatabaseDlg::~DatabaseDlg()
{
	delete ui;
}

void DatabaseDlg::on_DatabaseDlg_accepted()
{
	credentials.user = ui->userEdit->text();
	credentials.pass = ui->passEdit->text();
	credentials.server = ui->serverEdit->text();
	credentials.database = ui->databaseEdit->text();
}
