#ifndef WORDEDITDLG_H
#define WORDEDITDLG_H

#include <QDialog>
#include "ui_WordEditDlg.h"

class WordEditDlg : public QDialog
{
	Q_OBJECT

public:
	WordEditDlg(QWidget *parent = 0);

	QString getWord()        const;
	QString getPhonetic()    const;
	QString getExplanation() const;
	QString getNote()        const;

	void setWord       (const QString& word);
	void setPhonetic   (const QString& phonetic);
	void setExplanation(const QString& explanation);
	void setNote       (const QString& note);

private:
	Ui::WordEditDlgClass ui;
};

#endif // WORDEDITDLG_H
