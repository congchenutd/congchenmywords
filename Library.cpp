#include "Library.h"
#include "DictLoader.h"
#include "UserSetting.h"
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QProgressDialog>
#include <QMessageBox>
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QSettings>

QStringList Library::getDictList()
{
	QStringList list;
	QSqlQuery query;
	query.exec("SELECT * FROM Library");
	while(query.next())
		list << query.value(0).toString();
	return list;
}

int Library::getDictSize(const QString& dictName)
{
	QSqlQuery query;
	query.exec(QObject::tr("SELECT COUNT(Id) FROM %1").arg(dictName));
	return query.next() ? query.value(0).toInt() : 0;
}

void Library::setCredit(const QString& dictName, const QString& userName, 
						const QString& word, int credit)
{
	if(credit < 0)
		return;

	QSqlQuery query;
	query.exec(QObject::tr("SELECT * FROM %1 WHERE word=\"%2\"").arg(userName)
															    .arg(word));
	if(query.next())  // exists in userName table
	{
		if(credit == 0)
			query.exec(QObject::tr("DELETE FROM %1 WHERE word=\"%2\"").arg(userName)
																	  .arg(word));
		else
			query.exec(QObject::tr("UPDATE %1 SET credit=%2 WHERE word=\"%3\"").arg(userName)
																			   .arg(credit)
																			   .arg(word));
	}
	else
	{
		query.exec(QObject::tr("INSERT INTO %1 VALUES(\"%2\", %3)").arg(userName)
																   .arg(word)
																   .arg(credit));
	}
	query.exec(QObject::tr("UPDATE %1 SET credit=%2 WHERE word=\"%3\"").arg(dictName)
																	   .arg(credit)
																	   .arg(word));
}

int Library::getCredit(const QString& userName, const QString& english)
{
	QSqlQuery query;
	query.exec(QObject::tr("SELECT credit FROM \"%1\" WHERE word=\"%2\"").arg(userName)
																		 .arg(english));
	return query.next() ? query.value(0).toInt() : 0;
}

int Library::searchEnglish(const QString& dictName, const QString& english)
{
	QSqlQuery query;
	query.exec(QObject::tr("SELECT id FROM %1 WHERE word = \"%2\"").arg(dictName)
																   .arg(english));
	return query.next() ? query.value(0).toInt() : -1;
}

QString Library::getChinese(const QString& dictName, const QString& english)
{
	QSqlQuery query;
	query.exec(QObject::tr("SELECT explanation FROM %1 WHERE word=\"%2\"").arg(dictName)
																          .arg(english));
	return query.next() ? query.value(0).toString() : QString();
}

bool Library::openDB(const QString& name)
{
	if(!QFile::exists(name))
	{
		if(QMessageBox::critical(0, QObject::tr("Error"), 
				 QObject::tr("Cannot open database %1, do you want to recreate?").arg(name),
				 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
			return false;

	}
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName(name);
	if(!database.open())
	{
		QMessageBox::critical(0, "Error", "Can not open database");
		return false;
	}
	return true;
}

void Library::createTables()
{
	QSqlQuery query;
	query.exec("create table Library(dict varchar primary key)");
	query.exec("create table User(name varchar primary key)");
	query.exec(QObject::tr("CREATE TABLE Synonyms(chinese varchar primary key, \
												  words   varchar)"));
	query.exec(QObject::tr("CREATE TABLE Antonyms(chinese varchar primary key, \
												  words   varchar)"));
	query.exec(QObject::tr("CREATE TABLE Confusing(chinese varchar primary key, \
												   words   varchar)"));
	query.exec(" \
	create table SectionSizes \
	(                         \
		tableName varchar,    \
		section   int,        \
		size      decimal     \
	)");
}

int Library::getNextID(const QString& tableName)
{
	QSqlQuery query;
	query.exec(QObject::tr("SELECT max(id) FROM %1").arg(tableName));
	return query.next() ? query.value(0).toInt() + 1 : 1;
}

void Library::addUser(const QString& name)
{
	QSqlQuery query;
	query.exec(QObject::tr("INSERT INTO User VALUES(\'%1\')").arg(name));
	query.exec(QObject::tr("CREATE TABLE %1 (word   varchar primary key, \
											 credit int)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 0, 0)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 1, 0.2)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 2, 0.5)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 3, 0.2)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 4, 0)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 5, 0)").arg(name));
	query.exec(QObject::tr("INSERT INTO SectionSizes VALUES(\'%1\', 6, 0)").arg(name));
}

void Library::delUser(const QString& name)
{
	QSqlQuery query;
	query.exec(QObject::tr("DELETE FROM User         WHERE name      = \"%1\"").arg(name));
	query.exec(QObject::tr("DELETE FROM SectionSizes WHERE tableName = \"%1\"").arg(name));
	query.exec(QObject::tr("DROP TABLE %1").arg(name));
	QFile::remove(name + ".ini");
}

QStringList Library::getUserList()
{
	QStringList result;
	QSqlQuery query;
	query.exec("SELECT name FROM User ORDER BY name");
	while(query.next())
		result << query.value(0).toString();
	return result;
}

void Library::exportMp3(const QString& destDir, const QString& english)
{
	const QString wavDir = UserSetting::getInstance("Global")->value("WavDir").toString();
	const QString mp3Dir = UserSetting::getInstance("Global")->value("Mp3Dir").toString();

	QString mp3FileName = mp3Dir + "/" + QString(english.at(0)) + "/" + english + ".mp3";
	QString wavFileName = wavDir + "/" + QString(english.at(0)) + "/" + english + ".wma";
	if(QFile::exists(mp3FileName))
		QFile::copy(mp3FileName, destDir + "/" + english.toUpper() + ".mp3");
	else                                       // use wav file instead
		QFile::copy(wavFileName, destDir + "/" + english.toUpper() + ".wav");
}

QString Library::getDBFileName() {
	return UserSetting::getInstance("Global")->value("dbFileName").toString();
}

bool Library::createDictTable(const QString& dict)
{
	QSqlQuery query;
	return query.exec(QObject::tr("CREATE TABLE %1 (   \
						 Id          int primary key,  \
						 Word        varchar,          \
						 Explanation varchar,          \
						 Credit      int,              \
						 Phonetic    varchar,          \
						 Note        varchar,          \
						 Sorting     int)").arg(dict));
}

void Library::delDict(const QString& dict)
{
	QSqlQuery query;
	query.exec(QObject::tr("DROP TABLE %1").arg(dict));
	query.exec(QObject::tr("DELETE FROM Library WHERE dict = \"%1\"").arg(dict));
	query.exec(QObject::tr("DELETE FROM History WHERE dict = \"%1\"").arg(dict));
}

void Library::addDictToLibrary(const QString& dict)
{
	QSqlQuery query;
	query.exec(QObject::tr("INSERT INTO Library VALUES(\'%1\')").arg(dict));
}

void Library::addWord(const QString& dictName, const QString& word, 
					  const QString& phonetic, const QString& explanation, const QString& note)
{
	int id = searchEnglish(dictName, word);
	if(id != -1)
	{
		QMessageBox::warning(0, QObject::tr("Error"), QObject::tr("This word already exists"));
		return;
	}
	QSqlQuery query;
	query.exec(QObject::tr("INSERT INTO %1 VALUES (\"%2\", \"%3\", \"%4\", 0, \"%5\", \"%6\", 0)")
		.arg(dictName).arg(getNextID(dictName)).arg(word).arg(explanation).arg(phonetic).arg(note));
}