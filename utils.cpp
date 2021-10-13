#include "utils.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlDriver>
#include <QtGlobal>
#include <QMessageBox>


namespace Utils
{

bool ExecQuery(QSqlQuery &query)
{
	QSqlDatabase db = QSqlDatabase::database();
	if (db.isOpen())
	{
		if (query.exec())
		{
			qDebug() << query.lastQuery();
			return true;
		}
		else
		{
			qDebug() << query.lastError();
			return false;
		}
	}
	else
	{
		qDebug() << "Database is not open!";
		return false;
	}
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


void ComponentTestsSubtree(QTreeWidgetItem *parent, const TestProperties &props)
{
	QSqlQuery query;
	query.prepare("SELECT TestName, TestID from TestTbl WHERE ModuleID = :moduleID");
	query.bindValue(":moduleID", props.moduleID);

	if (Utils::ExecQuery(query))
	{
		while (query.next())
		{
			QString testName = query.value("TestName").toString();
			int testID = query.value("TestID").toInt();

			NewTreeItem(parent, { props.testType, props.moduleID, testID }, testName);
		}

		// you may want to now call expandItem(parent);
	}
}


int QuerySize(QSqlQuery &query)
{
	int size = -1;
	if (query.isActive() && query.isSelect())
	{
		if (query.driver()->hasFeature(QSqlDriver::QuerySize))
		{
			size = query.size();
		}
		else
		{
			size = 0;
			while (query.next())
				size++;
		}
	}

	query.first();
	return size;
}

bool MoveFeature(QWidget *parent, int featureID, int srcTestID, int targetTestID)
{
	bool isValid = featureID > 0 && srcTestID > 0 && targetTestID > 0 && srcTestID != targetTestID;

	QString error;

	Q_ASSERT(isValid);
	if (isValid)
	{
		QSqlQuery lookup;
		isValid = lookup.prepare("SELECT LPAD(MAX(f.FeatNumber) + 1, 3, '0') AS 'NextFeatNum', t.TestNumber "
								 "FROM FeatureTbl AS f "
								 "INNER JOIN TestTbl AS t ON f.TestID = t.TestID "
								 "WHERE f.TestID = :testID "
								 "GROUP BY t.TestNumber");
		lookup.bindValue(":testID", targetTestID);

		if (isValid && ExecQuery(lookup))
		{
			if (lookup.next())
			{
				QString featNumber = lookup.value("NextFeatNum").toString();
				QString testNumber = lookup.value("TestNumber").toString();

				if (!featNumber.isEmpty() && !testNumber.isEmpty())
				{
					QSqlQuery update;
					update.prepare("UPDATE FeatureTbl SET "
								   "TestID = :testID, "
								   "FeatNumber = :featNumber, "
								   "TestNumber = :testNumber "
								   "WHERE FeatureID = :featureID");
					update.bindValue(":testID", targetTestID);
					update.bindValue(":featNumber", featNumber);
					update.bindValue(":testNumber", testNumber);
					update.bindValue(":featureID", featureID);

					if (ExecQuery(update))
					{
						return true;
					}
					else
					{
						error = QString("%1\n%2").arg(lookup.lastError().text()).arg(lookup.lastQuery());
					}
				}
			}
		}
		else
		{
			error = QString("%1\n%2").arg(lookup.lastError().text()).arg(lookup.lastQuery());
		}
	}
	else
		error = QString("Invalid input with parameters featureID = %1, srcTestID = %2, targetTestID = %3").arg(featureID).arg(srcTestID).arg(targetTestID);

	QMessageBox::critical(parent, "Error", error);
	return false;
}

int CountFeatures(int testID)
{
	QSqlQuery count;
	count.prepare("SELECT COUNT(1) FROM FeatureTbl WHERE TestID = :testID");
	count.bindValue(":testID", testID);

	if (ExecQuery(count))
	{
		if (count.next())
		{
			bool ok;
			int numFeatures = count.value(0).toInt(&ok);
			if (ok)
				return numFeatures;
		}
	}

	return -1;
}

bool DeleteComponent(QWidget *parent, int moduleID, int testID)
{
	if (QMessageBox::Yes == QMessageBox::question(parent, "Delete", "Proceed with deletion?", QMessageBox::Yes|QMessageBox::No))
	{
		QSqlQuery deletion;
		deletion.prepare("DELETE FROM TestTbl WHERE ModuleID = :moduleID AND TestID = :testID");
		deletion.bindValue(":moduleID", moduleID);
		deletion.bindValue(":testID", testID);

		if (ExecQuery(deletion))
		{
			return true;
		}
		else
		{
			QMessageBox::critical(parent, "Error", QString("Unable to delete from TestTbl with ModuleID %1 and TestID %2")
								  .arg(moduleID)
								  .arg(testID));
		}
	}

	return false;
}

QVector<SearchResults> KeywordSearch_Components(const QString &keyword)
{
	QVector<SearchResults> results;
	QSqlQuery search;
	QString query = QString("SELECT CONCAT(m.ModuleCode, t.TestNumber) AS 'TestNumber', t.ModuleID, t.TestID, t.TestName, TestDescription "
							"FROM TestTbl AS t "
							"INNER JOIN ModuleTbl AS m ON m.ModuleID = t.ModuleID "
							"WHERE TestName LIKE \'%%1%\' OR TestDescription LIKE \'%%2%\'").arg(keyword).arg(keyword);
	search.prepare(query);

	if (ExecQuery(search))
	{
		while (search.next())
		{
			int moduleID = search.value("ModuleID").toInt();
			int testID = search.value("TestID").toInt();
			QString number = search.value("TestNumber").toString();
			QString name = search.value("TestName").toString();
			QString description = search.value("TestDescription").toString();

			SearchResults item;
			item.test.testType = (moduleID <= 10 ? TestProperties::CDC : TestProperties::ACC);
			item.test.moduleID = moduleID;
			item.test.testID = testID;
			item.number = number;
			item.name = name;
			item.description = description;
			results.push_back(item);
		}
	}
	return results;
}

QVector<SearchResults> KeywordSearch_Features(const QString &keyword)
{
	QVector<SearchResults> results;
	QSqlQuery search;
	QString query = QString("SELECT CONCAT(m.ModuleCode, t.TestNumber, '_', f.FeatNumber) AS 'TestNumber', f.FeatureID, f.TestID, f.FeatName, f.FeatDescription, t.ModuleID "
							"FROM FeatureTbl AS f "
							"INNER JOIN TestTbl AS t ON f.TestID = t.TestID "
							"INNER JOIN ModuleTbl AS m ON m.ModuleID = t.ModuleID "
							"WHERE FeatName LIKE \'%%1%\' OR FeatDescription LIKE \'%%2%\'").arg(keyword).arg(keyword);
	search.prepare(query);

	if (ExecQuery(search))
	{
		while (search.next())
		{
			int moduleID = search.value("ModuleID").toInt();
			int testID = search.value("TestID").toInt();
			QString number = search.value("TestNumber").toString();
			QString name = search.value("FeatName").toString();
			QString description = search.value("FeatDescription").toString();

			SearchResults item;
			item.test.testType = (moduleID <= 10 ? TestProperties::CDC : TestProperties::ACC);
			item.test.moduleID = moduleID;
			item.test.testID = testID;
			item.number = number;
			item.name = name;
			item.description = description;
			results.push_back(item);
		}
	}
	return results;
}

QVector<SearchResults> KeywordSearch_Regressions(const QString &keyword)
{
	QVector<SearchResults> results;
	QSqlQuery search;
	QString query = QString("SELECT ModuleID, RegressionTestID, TestName, TestFix FROM RegTestTbl WHERE TestFix LIKE \'%%1%\'").arg(keyword);
	search.prepare(query);

	if (ExecQuery(search))
	{
		while (search.next())
		{
			int moduleID = search.value("ModuleID").toInt();
			int testID = search.value("RegressionTestID").toInt();
			QString number = search.value("TestName").toString();
			QString description = search.value("TestFix").toString();

			SearchResults item;
			item.test.testType = (moduleID <= 10 ? TestProperties::CDR : TestProperties::ACR);
			item.test.moduleID = moduleID;
			item.test.testID = testID;
			item.number = number;
			item.description = description;
			results.push_back(item);
		}
	}
	return results;
}

QVector<SearchResults> KeywordSearch(const QString &keyword)
{
	QVector<SearchResults> results;
	results += KeywordSearch_Components(keyword);
	results += KeywordSearch_Features(keyword);
	results += KeywordSearch_Regressions(keyword);
	return results;
}

QTreeWidgetItem *FindTreeItem(QTreeWidgetItem *root, const TestProperties &search)
{
	if (root)
	{
		QTreeWidgetItemIterator it(root);
		while (*it)
		{
			QVariant var = (*it)->data(0, Qt::UserRole);
			if (!var.isNull() && var.isValid())
			{
				TestProperties props = var.value<TestProperties>();
				if (props.moduleID == search.moduleID)
				{
					return (*it);
				}
			}
			++it;
		}
		return root;
	}
	return nullptr;
}

}
