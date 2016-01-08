#ifndef MOVETARGETDLG_H
#define MOVETARGETDLG_H

#include "utils.h"
#include <QDialog>

namespace Ui {
class MoveTargetDlg;
}

class MoveTargetDlg : public QDialog
{
	Q_OBJECT

public:
	explicit MoveTargetDlg(QWidget *parent, RHSType type, int &moduleID, int &testID);
	~MoveTargetDlg();

private slots:
	void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *);

	void on_MoveTargetDlg_accepted();

private:
	void enableButtons(bool enable);
	void fill(TestProperties::TestType type1, const QString &label1, int start1, int end1, TestProperties::TestType type2, const QString &label2, int start2, int end2);
	void fillChildren(QTreeWidgetItem *parent, const QString &select, const TestProperties &props);
private:
	Ui::MoveTargetDlg *ui;
	int &m_moduleID;
	int &m_testID;
};

#endif // MOVETARGETDLG_H
