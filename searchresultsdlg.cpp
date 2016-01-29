#include "searchresultsdlg.h"
#include "ui_searchresultsdlg.h"

namespace
{
	QString getModuleName(int type)
	{
		switch (type)
		{
		case TestProperties::CDC:
			return "CD Component";
		case TestProperties::CDR:
			return "CD Regression";
		case TestProperties::ACC:
			return "AC Component";
		case TestProperties::ACR:
			return "AC Regression";
		}

		return "";
	}
}

SearchResultsDlg::SearchResultsDlg(QWidget *parent, QVector<SearchResults> &results) :
	QDialog(parent),
	ui(new Ui::SearchResultsDlg),
	m_results(results)
{
	ui->setupUi(this);

	QStringList headers { "Module", "Number", "Name", "Description" };

	ui->tableWidget->setColumnCount(headers.size());
	ui->tableWidget->setRowCount(results.size());
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidget->setHorizontalHeaderLabels(headers);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	int row = 0;
	for (const auto &test : results)
	{
		ui->tableWidget->setItem(row, 0, new QTableWidgetItem(getModuleName(test.test.testType)));
		ui->tableWidget->setItem(row, 1, new QTableWidgetItem(test.number));
		ui->tableWidget->setItem(row, 2, new QTableWidgetItem(test.name));
		ui->tableWidget->setItem(row, 3, new QTableWidgetItem(test.description));
		row++;
	}
}

SearchResultsDlg::~SearchResultsDlg()
{
	delete ui;
}
