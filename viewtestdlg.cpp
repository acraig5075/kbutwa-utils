#include "viewtestdlg.h"
#include "ui_viewtestdlg.h"

ViewTestDlg::ViewTestDlg(QWidget *parent, const QString &number, const QString &vault, const QVector<QPair<QString, QString>> &labels, const QString &procedure) :
	QDialog(parent),
	ui(new Ui::ViewTestDlg)
{
	ui->setupUi(this);

	ui->numberEdit->setReadOnly(true);
	ui->vaultEdit->setReadOnly(true);
	ui->label1Edit->setReadOnly(true);
	ui->label2Edit->setReadOnly(true);
	ui->label3Edit->setReadOnly(true);
	ui->procedureText->setReadOnly(true);

	QLabel *labelNames[] = { ui->label1Name, ui->label2Name, ui->label3Name };
	QLineEdit *labelEdits[] = { ui->label1Edit, ui->label2Edit, ui->label3Edit };

	std::for_each(std::begin(labelNames), std::end(labelNames), [](QLabel *label){ label->setVisible(false); });
	std::for_each(std::begin(labelEdits), std::end(labelEdits), [](QLineEdit *edit){ edit->setVisible(false); });

	for (int i = 0; i < qMin(labels.size(), 3); ++i)
	{
		labelNames[i]->setVisible(true);
		labelEdits[i]->setVisible(true);
		labelNames[i]->setText(labels.at(i).first);
		labelEdits[i]->setText(labels.at(i).second);
	}

	ui->numberEdit->setText(number);
	ui->vaultEdit->setText(vault);
	ui->procedureText->setText(procedure);
}

ViewTestDlg::~ViewTestDlg()
{
	delete ui;
}
