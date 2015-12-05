#include "widget.h"
#include "ui_widget.h"
#include "databasedlg.h"
#include "utils.h"
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>


namespace
{
bool canDelete(const TestProperties &props)
{
	return props.testID > 0 && props.moduleID > 0 && props.testType > 0;
}
}


Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	setDatabaseStatus();

	disableButtons();

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
		QString status = QString("Connected to DSN %1").arg(dsn);
		ui->databaseLabel->setStyleSheet("QLabel { color : green; }");
		ui->databaseLabel->setText(status);
	}
	else
	{
		ui->databaseLabel->setStyleSheet("QLabel { color : red; }");
		ui->databaseLabel->setText("No database connection.");
	}
}

void Widget::disableButtons()
{
	ui->deleteComponentButton->setEnabled(false);
	ui->viewFeatureButton->setEnabled(false);
	ui->deleteFeatureButton->setEnabled(false);
	ui->moveFeatureButton->setEnabled(false);
}

void Widget::on_databaseButton_clicked()
{
	dsn = "Development_UnitTest";
	DatabaseDlg *dlg = new DatabaseDlg(this, dsn);
	if (dlg->exec() == QDialog::Accepted)
	{
		// close current database
		db.close();

		// reset controls to default
		ui->treeWidget->clear();
		ui->tableView->setModel(nullptr);
		disableButtons();

		// open new database
		db = QSqlDatabase::addDatabase("QODBC");
		db.setDatabaseName(dsn);
		db.open();

		// set database string
		setDatabaseStatus();

		// setup controls
		if (db.isOpen())
		{
			this->setCursor(QCursor(Qt::WaitCursor));
			setup();
			this->unsetCursor();
		}
		else
		{
			qDebug() << db.lastError();
			QMessageBox::critical(this, "Error opening database", db.lastError().text());
		}
	}
}

void Widget::on_Widget_destroyed(QObject* /*arg1*/)
{
	db.close();
}

void Widget::on_Widget_destroyed()
{
	db.close();
}

QTreeWidgetItem* NewTreeItem(QTreeWidgetItem *parent, const TestProperties &prop, const QString &value)
{
	auto item = new QTreeWidgetItem(QStringList(value));
	auto data = QVariant::fromValue(prop);
	item->setData(0, Qt::UserRole, data);
	if (parent)
		parent->addChild(item);
	return item;
}

void Widget::setup()
{
	auto cdcItem = NewTreeItem(nullptr, { TestProperties::CDC }, "Civil Designer component");
	auto cdrItem = NewTreeItem(nullptr, { TestProperties::CDR }, "Civil Designer regression");
	auto accItem = NewTreeItem(nullptr, { TestProperties::ACC }, "AllyCAD component");
	auto acrItem = NewTreeItem(nullptr, { TestProperties::ACR }, "AllyCAD regression");

	QSqlQuery query;
	query.prepare("SELECT ModuleID, ModuleName FROM moduletbl");

	if (Utils::ExecQuery(query))
	{
		while (query.next())
		{
			int id = query.value("ModuleID").toInt();
			QString name = query.value("ModuleName").toString();

			if (id <= 10)
			{
				NewTreeItem(cdcItem, { TestProperties::CDC, id }, name);
				NewTreeItem(cdrItem, { TestProperties::CDR, id }, name);
			}
			else if (id <= 25)
			{
				NewTreeItem(accItem, { TestProperties::ACC, id }, name);
			}
			else if (id == 26)
			{
				NewTreeItem(acrItem, { TestProperties::ACR, id }, name);
			}
		}
	}

	ui->treeWidget->setColumnCount(1);
	ui->treeWidget->setHeaderLabel("Test types");
	ui->treeWidget->insertTopLevelItem(0, cdcItem);
	ui->treeWidget->insertTopLevelItem(1, cdrItem);
	ui->treeWidget->insertTopLevelItem(2, accItem);
	ui->treeWidget->insertTopLevelItem(3, acrItem);
}

// RHS
QString sqlFeatures = "SELECT TestID, FeatureID, FeatName FROM featuretbl WHERE TestID = :id";
QString sqlRegressions = "SELECT RegressionTestID, TestFix FROM regtesttbl WHERE moduleID = :id";

// LHS
QString sqlComponents = "SELECT TestName, TestID from testtbl WHERE ModuleID = :moduleID";

void Widget::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
	ui->deleteComponentButton->setEnabled(false);

	QVariant var = current->data(0, Qt::UserRole);
	if (!var.isNull() && var.isValid())
	{
		TestProperties props = var.value<TestProperties>();
		bool hasChildren = current->childCount() > 0;

		switch (props.testType)
		{
		case TestProperties::CDC:
		case TestProperties::ACC:
			if (props.testID > 0)
				populateRHS(sqlFeatures, props.testID);
			else if (props.moduleID > 0 && !hasChildren)
				populateLHS(current, sqlComponents, props);
			break;

		case TestProperties::CDR:
		case TestProperties::ACR:
			if (props.moduleID > 0)
				populateRHS(sqlRegressions, props.moduleID);
			break;
		}

		if (canDelete(props))
			ui->deleteComponentButton->setEnabled(true);
	}
}

void Widget::populateRHS(QString &select, int id)
{
	ui->tableView->setModel(nullptr);
	ui->tableView->verticalHeader()->hide();
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	QSqlQuery query;
	query.prepare(select);
	query.bindValue(":id", id);

	if (Utils::ExecQuery(query))
	{
		auto model = new QSqlQueryModel(this);
		model->setQuery(query);
		ui->tableView->setModel(model);
	}

	ui->tableView->show();
}

void Widget::populateLHS(QTreeWidgetItem *parent, const QString &select, const TestProperties &props)
{
	QSqlQuery query;
	query.prepare(select);
	query.bindValue(":moduleID", props.moduleID);

	if (Utils::ExecQuery(query))
	{
		while (query.next())
		{
			QString testName = query.value("TestName").toString();
			int testID = query.value("TestID").toInt();

			NewTreeItem(parent, { props.testType, props.moduleID, testID }, testName);
		}

		ui->treeWidget->expandItem(parent);
	}
}

void Widget::on_tableView_clicked(const QModelIndex &)
{
	bool enabled = false;
	auto selectionModel = ui->tableView->selectionModel();
	if (selectionModel)
	{
		auto selectionList = selectionModel->selectedRows();
		enabled = (selectionList.size() == 1);
	}

	ui->deleteFeatureButton->setEnabled(enabled);
	ui->viewFeatureButton->setEnabled(enabled);
	ui->moveFeatureButton->setEnabled(enabled);
}
