#include <QtGui>
#include "Entrance.h"
#include "Library.h"
#include "UserSetting.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB18030"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB18030"));
	QFontDatabase::addApplicationFont("TOPhonetic.ttf");
	
	if(!Library::openDB(Library::getDBFileName()))
	{
		app.quit();
		return 0;
	}

	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	QTranslator translator;
	translator.load(setting->value("language").toString());
	qApp->installTranslator(&translator);

	Library::createTables();
	Entrance entrance;
	entrance.show();
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	int ret = app.exec();
	UserSetting::destroySettingManager();
	return ret;
}
