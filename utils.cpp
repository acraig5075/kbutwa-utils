#include "utils.h"
#include <QSqlError>
#include <QDebug>


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

}
