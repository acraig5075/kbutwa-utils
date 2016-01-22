#ifndef VIEWTESTDLG_H
#define VIEWTESTDLG_H

#include <QDialog>

namespace Ui {
class ViewTestDlg;
}

class ViewTestDlg : public QDialog
{
	Q_OBJECT

public:
	explicit ViewTestDlg(QWidget *parent, const QString &number, const QString &vault, const QString &component, const QString &name, const QString &description, const QString &procedure);
	~ViewTestDlg();

private:
	Ui::ViewTestDlg *ui;
};

#endif // VIEWTESTDLG_H
