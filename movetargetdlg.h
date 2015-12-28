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
	explicit MoveTargetDlg(QWidget *parent, RHSType type, int &moduleID);
	~MoveTargetDlg();

private:
	void fill(TestProperties::TestType type1, const QString &label1, int start1, int end1, TestProperties::TestType type2, const QString &label2, int start2, int end2);

private:
	Ui::MoveTargetDlg *ui;
	int &m_moduleID;
};

#endif // MOVETARGETDLG_H
