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
	explicit DatabaseDlg(QWidget *parent, QString &dsn);
	~DatabaseDlg();

private slots:
	void on_DatabaseDlg_accepted();

	void on_odbcButton_clicked();

private:
	Ui::DatabaseDlg *ui;
	QString &dsn;
    QStringList GetConnectionsFromRegistry();
};

#endif // DATABASEDLG_H
