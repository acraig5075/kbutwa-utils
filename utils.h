#ifndef UTILS_H
#define UTILS_H

#include <QSqlQuery>

struct Credentials
{
	QString user = "root";
	QString pass = "Back2Reality";
	QString server = "127.0.0.1";
	QString database = "UnitTest";
};

namespace Utils
{

bool ExecQuery(QSqlQuery &query);
int QuerySize(QSqlQuery &query);

}

#endif // UTILS_H

