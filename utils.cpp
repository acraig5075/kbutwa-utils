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
