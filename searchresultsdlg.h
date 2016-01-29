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
	SearchResultsDlg(QWidget *parent, QVector<SearchResults> &results, SearchResults &selection);
	~SearchResultsDlg();

private slots:
	void on_SearchResultsDlg_accepted();

private:
	Ui::SearchResultsDlg *ui;
	QVector<SearchResults> &m_results;
	SearchResults &m_selection;
};

#endif // SEARCHRESULTSDLG_H
