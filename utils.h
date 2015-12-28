#ifndef UTILS_H
#define UTILS_H

#include <QSqlQuery>
#include <QTreeWidgetItem>

struct Credentials
{
	QString user = "root";
	QString pass = "Back2Reality";
	QString server = "127.0.0.1";
	QString database = "UnitTest";
};

struct TestProperties
{
	enum TestType { CDC = 1, CDR, ACC, ACR };

	int testType = 0;
	int moduleID = 0;
	int testID = 0;

	TestProperties() = default;

	TestProperties(int testType, int moduleID = 0, int testID = 0)
		: testType(testType)
		, moduleID(moduleID)
		, testID(testID)
	{}
};
Q_DECLARE_METATYPE(TestProperties)

enum RHSType { RHS_None, RHS_Features, RHS_Regressions };


namespace Utils
{

bool ExecQuery(QSqlQuery &query);
QTreeWidgetItem* NewTreeItem(QTreeWidgetItem *parent, const TestProperties &prop, const QString &value);
int QuerySize(QSqlQuery &query);


}

#endif // UTILS_H

