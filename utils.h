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


struct SearchResults
{
	TestProperties test;
	QString number;
	QString name;
	QString description;
};

namespace Utils
{

bool ExecQuery(QSqlQuery &query);
int QuerySize(QSqlQuery &query);

QTreeWidgetItem* NewTreeItem(QTreeWidgetItem *parent, const TestProperties &prop, const QString &value);
void ComponentTestsSubtree(QTreeWidgetItem *parent, const TestProperties &props);

bool MoveFeature(QWidget *parent, int featureID, int srcTestID, int targetTestID);
int CountFeatures(int testID);
bool DeleteFeature(QWidget *parent, int moduleID, int testID);
QVector<SearchResults> KeywordSearch(const QString &keyword);

}

#endif // UTILS_H

