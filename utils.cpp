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
	query.prepare("SELECT TestName, TestID from testtbl WHERE ModuleID = :moduleID");
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
		QSqlDatabase db = QSqlDatabase::database();
		db.transaction();

		QSqlQuery lookup;
		lookup.prepare("SELECT RIGHT(CONCAT('000', CAST(MAX(f.FeatNumber) + 1 AS CHAR(3))),3) AS NextFeatNum, t.TestNumber "
					   "FROM featuretbl AS f "
					   "INNER JOIN testtbl as t on f.TestID = t.testID "
					   "where f.TestID = :testID");
		lookup.bindValue(":testID", targetTestID);

		error = lookup.lastQuery();

		if (ExecQuery(lookup))
		{
			if (lookup.next())
			{
				QString featNumber = lookup.value("NextFeatNum").toString();
				QString testNumber = lookup.value("TestNumber").toString();

				if (!featNumber.isEmpty() && !testNumber.isEmpty())
				{
					QSqlQuery update;
					update.prepare("UPDATE featuretbl SET "
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
						db.commit();
						return true;
					}
					else
					{
						error = update.lastQuery();
					}
				}
			}
		}

		db.rollback();
	}
	else
		error = QString("Invalid input with parameters featureID = %1, srcTestID = %2, targetTestID = %3").arg(featureID).arg(srcTestID).arg(targetTestID);

	QMessageBox::critical(parent, "Error", error);
	return false;
}

int CountFeatures(int testID)
{
	QSqlQuery count;
	count.prepare("SELECT COUNT(1) FROM featuretbl WHERE TestID = :testID");
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

bool DeleteFeature(QWidget *parent, int moduleID, int testID)
{
	if (QMessageBox::Yes == QMessageBox::question(parent, "Delete", "Proceed with deletion?", QMessageBox::Yes|QMessageBox::No))
	{
		QSqlQuery deletion;
		deletion.prepare("DELETE FROM testtbl WHERE ModuleID = :moduleID AND TestID = :testID");
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
	QString query = QString("SELECT ModuleID, TestID, TestNumber, TestName, TestDescription FROM testtbl WHERE TestName LIKE \'%%1%\' OR TestDescription LIKE \'%%2%\'").arg(keyword).arg(keyword);
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
	QString query = QString("SELECT FeatureID, TestID, TestNumber, FeatName, FeatDescription FROM featuretbl WHERE FeatName LIKE \'%%1%\' OR FeatDescription LIKE \'%%2%\'").arg(keyword).arg(keyword);
	search.prepare(query);

	if (ExecQuery(search))
	{
		while (search.next())
		{
			int moduleID = 0;
			int featureID = search.value("FeatureID").toInt();
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
	QString query = QString("SELECT ModuleID, RegressionTestID, TestName, TestFix FROM regtesttbl WHERE TestFix LIKE \'%%1%\'").arg(keyword);
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
			item.test.testType = (moduleID <= 10 ? TestProperties::CDC : TestProperties::ACC);
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

}
