#include "widget.h"
#include "ui_widget.h"
#include "databasedlg.h"
#include "utils.h"
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>


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

	connect(this, SIGNAL(RefreshRHS()), this, SLOT(onRefreshRHS()));
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
QString sqlRegressions = "SELECT ModuleID, RegressionTestID, TestFix FROM regtesttbl WHERE ModuleID = :id";

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
				populateRHS({ RHS_Features, sqlFeatures, props.testID });
			else if (props.moduleID > 0 && !hasChildren)
				populateLHS(current, sqlComponents, props);
			break;

		case TestProperties::CDR:
		case TestProperties::ACR:
			if (props.moduleID > 0)
				populateRHS({ RHS_Regressions, sqlRegressions, props.moduleID });
			break;
		}

		if (canDelete(props))
			ui->deleteComponentButton->setEnabled(true);
	}
}

void Widget::populateRHS(const RHS_Settings &settings)
{
	ui->tableView->setModel(nullptr);
	ui->tableView->verticalHeader()->hide();
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	QSqlQuery query;
	query.prepare(settings.query);
	query.bindValue(":id", settings.id);

	if (Utils::ExecQuery(query))
	{
		auto model = new QSqlQueryModel(this);
		model->setQuery(query);
		ui->tableView->setModel(model);
		rhsSettings = settings;
	}
	else
	{
		rhsSettings.type = RHS_None;
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

void Widget::DeleteFeature(int testID, int featureID)
{
	QSqlQuery select;
	select.prepare("SELECT FROM featuretbl WHERE FeatureID = :featureID AND TestID = :testID");
	if (Utils::ExecQuery(select))
	{
		if (select.size() == 1)
		{
			auto confirm = QMessageBox::question(this, "Delete", "Are you sure you want to delete this record?");
			if (confirm == QMessageBox::Yes)
			{
				db.transaction();

				QSqlQuery modify1;
				modify1.prepare("DELETE FROM resulttbl WHERE FeatureID = :featureID");
				modify1.bindValue(":featureID", featureID);
				Utils::ExecQuery(modify1);

				QSqlQuery modify2;
				modify2.prepare("DELETE FROM featuretbl WHERE FeatureID = :featureID AND TestID = :testID");
				modify2.bindValue(":featureID", featureID);
				modify2.bindValue(":testID", testID);
				Utils::ExecQuery(modify2);

				if (db.commit())
				{
					emit RefreshRHS();
				}
				else
				{
					db.rollback();
				}
			}
		}
		else
		{
			QMessageBox::critical(this, "Error", "Unexpectantly found nothing to delete");
		}
	}
}

void Widget::DeleteRegression(int moduleID, int regTestID)
{
	auto confirm = QMessageBox::question(this, "Delete", "Are you sure you want to delete this record?");
	if (confirm == QMessageBox::Yes)
	{
		db.transaction();

		QSqlQuery modify1;
		modify1.prepare("DELETE FROM regresulttbl WHERE RegressionTestID = :regTestID");
		modify1.bindValue(":regTestID", regTestID);
		Utils::ExecQuery(modify1);

		QSqlQuery modify2;
		modify2.prepare("DELETE FROM regtesttbl WHERE ModuleID = :moduleID AND RegressionTestID = :regTestID");
		modify2.bindValue(":moduleID", moduleID);
		modify2.bindValue(":regTestID", regTestID);
		Utils::ExecQuery(modify2);

		if (db.commit())
		{
			emit RefreshRHS();
		}
		else
		{
			db.rollback();
		}
	}
}

void Widget::onRefreshRHS()
{
	QSqlQueryModel *model = static_cast<QSqlQueryModel *>(ui->tableView->model());
	if (model)
		model->setQuery("");
}

void Widget::on_deleteFeatureButton_clicked()
{
	auto selectionModel = ui->tableView->selectionModel();
	if (selectionModel)
	{
		auto selectionList = selectionModel->selectedRows();
		if (selectionList.size() == 1)
		{
			QModelIndex index = selectionList.at(0);
			QSqlQueryModel *model = static_cast<QSqlQueryModel *>(ui->tableView->model());

			if (model)
			{
				QSqlRecord record = model->record(index.row());
				if (RHS_Features == rhsSettings.type)
				{
					bool ok1, ok2;
					int testID = record.value("TestID").toInt(&ok1);
					int featureID = record.value("FeatureID").toInt(&ok2);
					if (ok1 && ok2)
						DeleteFeature(testID, featureID);
				}
				else if (RHS_Regressions == rhsSettings.type)
				{
					bool ok1, ok2;
					int moduleID = record.value("ModuleID").toInt(&ok1);
					int regTestID = record.value("RegressionTestID").toInt(&ok2);
					if (ok1 && ok2)
						DeleteRegression(moduleID, regTestID);
				}
			}
		}
	}
}
