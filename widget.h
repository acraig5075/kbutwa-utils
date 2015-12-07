#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QTreeWidgetItem>


namespace Ui {
class Widget;
}

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


class Widget : public QWidget
{
	Q_OBJECT

	enum RHSType { RHS_None, RHS_Features, RHS_Regressions };

	struct RHS_Settings
	{
		RHSType type = RHS_None;
		QString query;
		int id = 0;

		RHS_Settings() = default;
		RHS_Settings(RHSType type, const QString &query, int id)
			: type(type), query(query), id(id)
		{}
	};

public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

private:
	void setDatabaseStatus();
	void disableButtons();
	void setup();
	void populateRHS(const RHS_Settings &settings);
	void populateLHS(QTreeWidgetItem *parent, const QString &select, const TestProperties &props);
	void DeleteFeature(int testID, int featureID);
	void DeleteRegression(int moduleID, int regTestID);

private slots:
	void on_databaseButton_clicked();
	void on_Widget_destroyed(QObject *arg1);
	void on_Widget_destroyed();
	void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_tableView_clicked(const QModelIndex &index);
	void on_deleteFeatureButton_clicked();
	void onRefreshRHS();

signals:
	void RefreshRHS();

private:
	Ui::Widget *ui;
	QString dsn;
	QSqlDatabase db;
	RHS_Settings rhsSettings;
};

#endif // WIDGET_H
