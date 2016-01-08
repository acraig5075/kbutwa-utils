#include "movetargetdlg.h"
#include "ui_movetargetdlg.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

MoveTargetDlg::MoveTargetDlg(QWidget *parent, RHSType type, int &moduleID, int &testID) :
	QDialog(parent),
	m_moduleID(moduleID),
	m_testID(testID),
	ui(new Ui::MoveTargetDlg)
{
	ui->setupUi(this);
	enableButtons(false);

	if (type == RHS_Features)
		fill(TestProperties::CDC, "Civil Designer component", 1, 10, TestProperties::ACC, "AllyCAD component", 11, 25);
	else if (type == RHS_Regressions)
		fill(TestProperties::CDR, "Civil Designer regression", 1, 10, TestProperties::ACR, "AllyCAD regression", 26, 26);
}

void MoveTargetDlg::enableButtons(bool enable)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
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

void MoveTargetDlg::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
	enableButtons(false);

	QVariant var = current->data(0, Qt::UserRole);
	if (!var.isNull() && var.isValid())
	{
		TestProperties props = var.value<TestProperties>();
		bool hasChildren = current->childCount() > 0;

		switch (props.testType)
		{
		case TestProperties::CDC:
		case TestProperties::ACC:
			if (props.testID == 0)
			{
				if (props.moduleID > 0 && !hasChildren)
				{
					// populate with child items
					Utils::ComponentTestsSubtree(current, props);
					ui->treeWidget->expandItem(current);
				}
			}
			else
			{
				// enable OK button if valid selection
				bool isValid = props.moduleID > 0 && props.testID > 0 && props.testID != m_testID;
				enableButtons(isValid);
			}
			break;
		}
	}
}


void MoveTargetDlg::on_MoveTargetDlg_accepted()
{
	m_moduleID = 0;
	m_testID = 0;

	QTreeWidgetItem *current = ui->treeWidget->currentItem();
	if (current)
	{
		QVariant var = current->data(0, Qt::UserRole);
		if (!var.isNull() && var.isValid())
		{
			TestProperties props = var.value<TestProperties>();
			m_moduleID = props.moduleID;
			m_testID = props.testID;
			QMessageBox::information(this, "Debug info", QString("Target module = %1, test = %2").arg(m_moduleID).arg(m_testID));
			return;
		}
		else
			qDebug() << "Current item data is null or invalid.";
	}
	else
		qDebug() << "No current item selected.";

	QMessageBox::critical(this, "Error", QString("Invalid target destination, very unexpected."));
}
