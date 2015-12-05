#ifndef DATABASEDLG_H
#define DATABASEDLG_H

#include <QDialog>
#include "utils.h"

namespace Ui {
class DatabaseDlg;
}

class DatabaseDlg : public QDialog
{
	Q_OBJECT

public:
	explicit DatabaseDlg(QWidget *parent, Credentials &credentials);
	~DatabaseDlg();

private slots:
	void on_DatabaseDlg_accepted();

private:
	Ui::DatabaseDlg *ui;
	Credentials &credentials;
};

#endif // DATABASEDLG_H
