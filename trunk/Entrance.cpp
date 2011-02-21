#include "Entrance.h"
#include "MainWindow.h"
#include "Library.h"
#include "UserSetting.h"
#include <QInputDialog>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QTranslator>

Entrance::Entrance(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
	connect(ui.buttonEnter,   SIGNAL(clicked()), this, SLOT(slotEnter()));
	connect(ui.buttonAddUser, SIGNAL(clicked()), this, SLOT(slotAddUser()));
	connect(ui.buttonDelUser, SIGNAL(clicked()), this, SLOT(slotDelUser()));
}

void Entrance::show()
{
	ui.comboBoxUser->addItems(Library::getUserList());   // load user names
    QString defaultUser = UserSetting::findFile("Other/isDefault", true);
	if(!defaultUser.isEmpty())
		go(defaultUser);
	else
		QDialog::show();
}

void Entrance::slotEnter()
{
	const QString currentUser = ui.comboBoxUser->currentText();
	if(currentUser.isEmpty())
		return;

	if(ui.checkBox->isChecked())
		UserSetting::getInstance(currentUser)->setValue("Other/isDefault", true);

	go(currentUser);
}

void Entrance::slotAddUser()
{
	QString name = QInputDialog::getText(
		this, tr("Add user"), tr("Input user name, starting with letter or _"),
		QLineEdit::Normal, tr("NewUser"));
	if(name.isEmpty())
		return;
	if(ui.comboBoxUser->findText(name) != -1)
	{
		QMessageBox::critical(this, tr("Error"), tr("This user already exists"));
		return;
	}

	Library::addUser(name);
	ui.comboBoxUser->addItem(name);
	ui.comboBoxUser->setCurrentIndex(ui.comboBoxUser->findText(name));
}

void Entrance::slotDelUser()
{		
	QString user = ui.comboBoxUser->currentText();
	if(user.isEmpty())
		return;

	if(QMessageBox::warning(this, tr("Warning"), tr("Are you sure to delete?"), 
							QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
	{
		Library::delUser(user);
		ui.comboBoxUser->removeItem(ui.comboBoxUser->findText(user));
	}
}

void Entrance::go(const QString& userName)
{
	hide();
	MainWindow* wnd = new MainWindow(userName);
	wnd->showMaximized();
	close();
}
