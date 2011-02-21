#include "WordEditDlg.h"

WordEditDlg::WordEditDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

QString WordEditDlg::getWord() const {
	return ui.leWord->text();
}
QString WordEditDlg::getPhonetic() const {
	return ui.lePhonetic->text();
}
QString WordEditDlg::getExplanation() const {
	return ui.leExplanation->text();
}
QString WordEditDlg::getNote() const {
	return ui.teNote->toPlainText();
}
void WordEditDlg::setWord(const QString& word) {
	ui.leWord->setText(word);
}
void WordEditDlg::setPhonetic(const QString& phonetic) {
	ui.lePhonetic->setText(phonetic);
}
void WordEditDlg::setExplanation(const QString& explanation) {
	ui.leExplanation->setText(explanation);
}
void WordEditDlg::setNote(const QString& note) {
	ui.teNote->setPlainText(note);
}

