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

public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

private:
	void setDatabaseStatus();
	void disableButtons();
	void setup();
	void populateRHS(QString &select, int id);
	void populateLHS(QTreeWidgetItem *parent, const QString &select, const TestProperties &props);

private slots:
	void on_databaseButton_clicked();
	void on_Widget_destroyed(QObject *arg1);
	void on_Widget_destroyed();
	void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void on_tableView_clicked(const QModelIndex &index);

private:
	Ui::Widget *ui;
	QString dsn;
	QSqlDatabase db;
};

#endif // WIDGET_H
