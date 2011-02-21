/*
 *	Import dictionary
 */

#ifndef DICTLOADER_H
#define DICTLOADER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class DictLoader : public QObject
{
	Q_OBJECT

public:
	DictLoader(const QString& file, const QString& dict, QObject *parent = 0);
	void load();
	
private:
	int     check();
	bool    doLoad();
	void    showError();
	bool    isEnglishLine(const QString& line);
	bool    isBeginLine  (const QString& line);
	QString getLine();
	void    addRecord(const QString& englishLine, const QString& chinese);

private:
	QString     fileName;
	QString     dictName;
	QFile       file;
	QTextStream is;
	int         id;
	int         currentRow;
};

#endif // DICTLOADER_H
