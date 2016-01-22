#include "viewtestdlg.h"
#include "ui_viewtestdlg.h"

ViewTestDlg::ViewTestDlg(QWidget *parent, const QString &number, const QString &vault, const QString &component, const QString &name, const QString &description, const QString &procedure) :
	QDialog(parent),
	ui(new Ui::ViewTestDlg)
{
	ui->setupUi(this);

	ui->numberEdit->setReadOnly(true);
	ui->vaultEdit->setReadOnly(true);
	ui->componentEdit->setReadOnly(true);
	ui->featureEdit->setReadOnly(true);
	ui->descriptionEdit->setReadOnly(true);
	ui->procedureText->setReadOnly(true);

	ui->numberEdit->setText(number);
	ui->vaultEdit->setText(vault);
	ui->componentEdit->setText(component);
	ui->featureEdit->setText(name);
	ui->descriptionEdit->setText(description);
	ui->procedureText->setText(procedure);
}

ViewTestDlg::~ViewTestDlg()
{
	delete ui;
}
