#include "widget.h"
#include "ui_widget.h"
#include "databasedlg.h"
#include "movetargetdlg.h"
#include "viewtestdlg.h"
#include "searchresultsdlg.h"
#include "utils.h"
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>


//namespace
//{
//bool canDelete(const TestProperties &props)
//{
//	return props.testID > 0 && props.moduleID > 0 && props.testType > 0;
//}
//}


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

	QAction *searchAction = ui->searchEdit->addAction(QIcon(":/search.ico"), QLineEdit::TrailingPosition);
	connect(searchAction, &QAction::triggered, this, &Widget::on_searchEdit_returnPressed);

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

void Widget::setup()
{
	auto cdcItem = Utils::NewTreeItem(nullptr, { TestProperties::CDC }, "Civil Designer component");
	auto cdrItem = Utils::NewTreeItem(nullptr, { TestProperties::CDR }, "Civil Designer regression");
	auto accItem = Utils::NewTreeItem(nullptr, { TestProperties::ACC }, "AllyCAD component");
	auto acrItem = Utils::NewTreeItem(nullptr, { TestProperties::ACR }, "AllyCAD regression");

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
				Utils::NewTreeItem(cdcItem, { TestProperties::CDC, id }, name);
				Utils::NewTreeItem(cdrItem, { TestProperties::CDR, id }, name);
			}
			else if (id <= 25)
			{
				Utils::NewTreeItem(accItem, { TestProperties::ACC, id }, name);
			}
			else if (id == 26)
			{
				Utils::NewTreeItem(acrItem, { TestProperties::ACR, id }, name);
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
QString sqlFeatures = "SELECT CONCAT(m.ModuleCode, t.TestNumber, '_', f.FeatNumber) AS 'Number', f.TestID, f.FeatureID, f.FeatName "
					  "FROM featuretbl AS f "
					  "INNER JOIN testtbl AS t ON f.TestID = t.TestID "
					  "INNER JOIN moduletbl AS m ON m.ModuleID = t.ModuleID "
					  "WHERE t.TestID = :id";
QString sqlRegressions = "SELECT TestName AS 'Number', ModuleID, RegressionTestID, TestFix "
						 "FROM regtesttbl "
						 "WHERE ModuleID = :id";

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
		ui->tableView->hideColumn(1);
		ui->tableView->hideColumn(2);
		ui->tableView->horizontalHeader()->setStretchLastSection(true);
		rhsSettings = settings;
	}
	else
	{
		rhsSettings.type = RHS_None;
	}

	ui->tableView->show();
	int rhsCount = ui->tableView->model()->rowCount();
	ui->deleteComponentButton->setEnabled(rhsCount == 0);
}

void Widget::populateLHS(QTreeWidgetItem *parent, const QString &, const TestProperties &props)
{
	Utils::ComponentTestsSubtree(parent, props);
	ui->treeWidget->expandItem(parent);
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
	select.prepare("SELECT CONCAT(TestNumber, '_', FeatNumber) AS FeatureFormattedName FROM featuretbl WHERE FeatureID = :featureID AND TestID = :testID");
	select.bindValue(":featureID", featureID);
	select.bindValue(":testID", testID);

	if (Utils::ExecQuery(select))
	{
		int size = Utils::QuerySize(select);
		if (size == 1)
		{
			QString name = select.value("FeatureFormattedName").toString();
			auto confirm = QMessageBox::question(this, "Delete", QString("Are you sure you want to delete feature %1?").arg(name));
			if (confirm == QMessageBox::Yes)
			{
				select.finish();
				db.transaction();

				QSqlQuery modify1;
				modify1.prepare("DELETE FROM resulttbl WHERE FeatureID = :featureID");
				modify1.bindValue(":featureID", featureID);
				bool ok1 = Utils::ExecQuery(modify1);

				QSqlQuery modify2;
				modify2.prepare("DELETE FROM featuretbl WHERE FeatureID = :featureID AND TestID = :testID");
				modify2.bindValue(":featureID", featureID);
				modify2.bindValue(":testID", testID);
				bool ok2 = Utils::ExecQuery(modify2);

				if (ok1 && ok2)
				{
					if (db.commit())
					{
						emit RefreshRHS();
						return;
					}
				}
				db.rollback();
			}
		}
		else
		{
			QMessageBox::critical(this, "Error", QString("Unexpectantly found %1 records to delete, when expecting only one").arg(size));
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
		bool ok1 = Utils::ExecQuery(modify1);

		QSqlQuery modify2;
		modify2.prepare("DELETE FROM regtesttbl WHERE ModuleID = :moduleID AND RegressionTestID = :regTestID");
		modify2.bindValue(":moduleID", moduleID);
		modify2.bindValue(":regTestID", regTestID);
		bool ok2 = Utils::ExecQuery(modify2);

		if (ok1 && ok2)
		{
			if (db.commit())
			{
				emit RefreshRHS();
				return;
			}
		}
		db.rollback();
	}
}

void Widget::onRefreshRHS()
{
	QSqlQueryModel *model = static_cast<QSqlQueryModel *>(ui->tableView->model());
	if (model)
	{
		QSqlQuery query;
		query.prepare(rhsSettings.query);
		query.bindValue(":id", rhsSettings.id);

		Utils::ExecQuery(query);
		model->setQuery(query);

		int rhsCount = model->rowCount();
		ui->deleteComponentButton->setEnabled(rhsCount == 0);
		ui->deleteFeatureButton->setEnabled(false);
		ui->viewFeatureButton->setEnabled(false);
		ui->moveFeatureButton->setEnabled(false);
	}
}

void Widget::GenericRHSOperation(void (Widget::*featureFunc)(int, int), void (Widget::*regressionFunc)(int, int))
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
					{
						if (featureFunc)
							(this->*featureFunc)(testID, featureID);
						else
							QMessageBox::information(this, "Unsupported", "The chosen operation is currently unsupported for Feature tests");
					}
				}
				else if (RHS_Regressions == rhsSettings.type)
				{
					bool ok1, ok2;
					int moduleID = record.value("ModuleID").toInt(&ok1);
					int regTestID = record.value("RegressionTestID").toInt(&ok2);
					if (ok1 && ok2)
					{
						if (regressionFunc)
							(this->*regressionFunc)(moduleID, regTestID);
						else
							QMessageBox::information(this, "Unsupported", "The chosen operation is currently unsupported for Regression tests");
					}
				}
			}
		}
	}
}

void Widget::on_deleteFeatureButton_clicked()
{
	GenericRHSOperation(&Widget::DeleteFeature, &Widget::DeleteRegression);
}

void Widget::on_moveFeatureButton_clicked()
{
	GenericRHSOperation(&Widget::MoveFeature, nullptr);
}

void Widget::on_viewFeatureButton_clicked()
{
	GenericRHSOperation(&Widget::ViewFeature, nullptr);
}

void Widget::MoveFeature(int testID, int featureID)
{
	int targetTestID = testID;
	int moduleID = 0;

	MoveTargetDlg *dlg = new MoveTargetDlg(this, rhsSettings.type, moduleID, targetTestID);
	if (dlg->exec() == QDialog::Accepted)
	{
		if (Utils::MoveFeature(this, featureID, testID, targetTestID))
		{
			emit RefreshRHS();
			QMessageBox::information(this, "Done", R"(You should now manually edit the test procedure and rename any test files on N:\ to refer to the new test number.)");
		}
	}
}

void Widget::on_deleteComponentButton_clicked()
{
	QTreeWidgetItem *current = ui->treeWidget->currentItem();
	if (current->childCount() == 0)
	{
		QVariant var = current->data(0, Qt::UserRole);
		if (!var.isNull() && var.isValid())
		{
			TestProperties props = var.value<TestProperties>();

			if (props.testType == TestProperties::CDC || props.testType == TestProperties::ACC)
			{
				if (props.moduleID > 0 && props.testID > 0)
				{
					int count = Utils::CountFeatures(props.testID);

					if (count == 0)
					{
						if (Utils::DeleteFeature(this, props.moduleID, props.testID))
						{
							QTreeWidgetItem *parentItem = current->parent();
							parentItem->removeChild(current);
						}
					}
					else
					{
						QMessageBox::critical(this, "Error", QString("Expected no features having ModuleID %1, TestID %2, instead found %3 feature(s).")
											  .arg(props.moduleID)
											  .arg(props.testID)
											  .arg(count));
					}
				}
			}
			else
			{
				QMessageBox::information(this, "Unsupported", "Only deletion of components having no features is currently supported.\n"
															  "Use the Delete button on the right if you wish to delete a feature or regression.");
			}
		}
	}
}

void Widget::ViewFeature(int /*testID*/, int featureID)
{
	QSqlQuery lookup;
	lookup.prepare("SELECT CONCAT(m.ModuleCode, t.TestNumber, '_', f.FeatNumber) AS 'TestNumber', "
				   "f.VaultNumber, t.TestName, f.FeatName, f.FeatDescription, f.FeatProcedure "
				   "FROM featuretbl AS f "
				   "INNER JOIN testtbl AS t ON f.TestID = t.TestID "
				   "INNER JOIN moduletbl AS m ON m.ModuleID = t.ModuleID "
				   "WHERE FeatureID = :featureID");
	lookup.bindValue(":featureID", featureID);

	if (Utils::ExecQuery(lookup))
	{
		if (lookup.next())
		{
			QString number = lookup.value("TestNumber").toString();;
			QString vault = lookup.value("VaultNumber").toString();
			QString component = lookup.value("TestName").toString();
			QString name = lookup.value("FeatName").toString();
			QString description = lookup.value("FeatDescription").toString();
			QString procedure = lookup.value("FeatProcedure").toString();

			ViewTestDlg *dlg = new ViewTestDlg(this, number, vault, component, name, description, procedure);
			if (dlg->exec() == QDialog::Accepted)
			{
			}
		}
	}
}

int getTreeRootIndex(TestProperties::TestType type)
{
	switch (type)
	{
	case TestProperties::CDC:
		return 0;
	case TestProperties::CDR:
		return 1;
	case TestProperties::ACC:
		return 2;
	case TestProperties::ACR:
		return 3;
	}
	return -1;
}

void Widget::on_searchEdit_returnPressed()
{
	QString search = ui->searchEdit->text();
	QVector<SearchResults> results = Utils::KeywordSearch(search);
	if (results.empty())
	{
		QMessageBox::information(this, "Search", "0 results found");
	}
	else
	{
		SearchResults selection;
		SearchResultsDlg *dlg = new SearchResultsDlg(this, results, selection);

		if (dlg->exec() == QDialog::Accepted)
		{
			int rootIndex = getTreeRootIndex(static_cast<TestProperties::TestType>(selection.test.testType));
			QTreeWidgetItem *root = ui->treeWidget->topLevelItem(rootIndex);
			QTreeWidgetItem *found = Utils::FindTreeItem(root, selection.test);

			if (found)
				ui->treeWidget->setCurrentItem(found);
		}
	}
}
