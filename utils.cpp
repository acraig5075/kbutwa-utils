#include "utils.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlDriver>


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
}
