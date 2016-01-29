#ifndef SEARCHRESULTSDLG_H
#define SEARCHRESULTSDLG_H

#include <QDialog>
#include "utils.h"

namespace Ui {
class SearchResultsDlg;
}

class SearchResultsDlg : public QDialog
{
	Q_OBJECT

public:
	SearchResultsDlg(QWidget *parent, QVector<SearchResults> &results);
	~SearchResultsDlg();

private:
	Ui::SearchResultsDlg *ui;
	QVector<SearchResults> &m_results;
};

#endif // SEARCHRESULTSDLG_H
