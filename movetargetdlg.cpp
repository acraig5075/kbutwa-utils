#include "movetargetdlg.h"
#include "ui_movetargetdlg.h"

MoveTargetDlg::MoveTargetDlg(QWidget *parent, RHSType type, int &moduleID) :
	QDialog(parent),
	m_moduleID(moduleID),
	ui(new Ui::MoveTargetDlg)
{
	ui->setupUi(this);

	if (type == RHS_Features)
		fill(TestProperties::CDC, "Civil Designer component", 1, 10, TestProperties::ACC, "AllyCAD component", 11, 25);
	else if (type == RHS_Regressions)
		fill(TestProperties::CDR, "Civil Designer regression", 1, 10, TestProperties::ACR, "AllyCAD regression", 26, 26);
}

void MoveTargetDlg::fill(TestProperties::TestType type1, const QString &label1, int start1, int end1, TestProperties::TestType type2, const QString &label2, int start2, int end2)
{
	QTreeWidgetItem *initial = nullptr;

	auto item1 = Utils::NewTreeItem(nullptr, { type1 }, label1);
	auto item2 = Utils::NewTreeItem(nullptr, { type2 }, label2);

	QSqlQuery query;
	query.prepare("SELECT ModuleID, ModuleName FROM moduletbl");

	auto Between = [](int test, int start, int end) -> bool { return start <= test && test <= end; };

	if (Utils::ExecQuery(query))
	{
		while (query.next())
		{
			int id = query.value("ModuleID").toInt();
			QString name = query.value("ModuleName").toString();

			QTreeWidgetItem *newItem = nullptr;
			if (Between(id, start1, end1))
				newItem = Utils::NewTreeItem(item1, { type1, id }, name);
			else if (Between(id, start2, end2))
				newItem = Utils::NewTreeItem(item2, { type2, id }, name);

			if (id == m_moduleID)
				initial = newItem;
		}
	}

	ui->treeWidget->setColumnCount(1);
	ui->treeWidget->setHeaderLabel("Test types");
	ui->treeWidget->insertTopLevelItem(0, item1);
	ui->treeWidget->insertTopLevelItem(1, item2);
	ui->treeWidget->setCurrentItem(initial);
}

MoveTargetDlg::~MoveTargetDlg()
{
	delete ui;
}
