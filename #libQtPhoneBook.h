#pragma once
#include "#libPhoneBook.h"
#include <sstream>

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qstackedwidget.h>

//#include <QtWidgets/qtablewidget.h>
#include <QtGui/qstandarditemmodel.h>
#include <QtWidgets/qtableview.h>

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qspinbox.h>

#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>

enum PAGE {
	MAIN = 0,
	TABLET = 1,
	LOAD = 2,
	SAVE = 3,
	EDIT = 4
};

enum TABLESORTED {
	FIRST_NAME_ASC,
	FIRST_NAME_DES,
	MIDDLE_NAME_ASC,
	MIDDLE_NAME_DES,
	LAST_NAME_ASC,
	LAST_NAME_DES,
	ADDRESS_ASC,
	ADDRESS_DES,
	DATE_ASC,
	DATE_DES,
	EMAIL_ASC,
	EMAIL_DES,
	PHONES_ASC,
	PHONES_DES,
	NONE = 0
};

struct fieldStruct {
	string firstName;
	string middleName;
	string lastName;
	string address;
	string dateOfBirth;
	string email;
	string phones;
};

const QStringList field_names = {
	"First name",
	"Middle name",
	"Last name",
	"Address",
	"Date of birth",
	"Email",
	"Phone numbers"
};

class QVListWidget : public QVBoxLayout {
private:
	int max_size;
	list<QHBoxLayout*> list_box;
	list<QLineEdit*> list_line_edit;
	list<QPushButton*> list_delete_btn;
	QPushButton* add_btn;

	void DeleteElement(QHBoxLayout*, QLineEdit*, QPushButton*);

public:
	QVListWidget(int);
	~QVListWidget();

	void AddElement();
	void CopyElement(string);
	void DeleteEveryElement();
	string GetEveryElement();
};

class PhoneBookManager {
private:
	PhoneBook* CurrentBook;
	Recording* CurrentRecording;

	QStandardItemModel* Recordings_table;
	QTableView* table_view;
	QStackedWidget* stacked_widget;
	QLineEdit* lineEdits[6];
	QVListWidget* phoneEdits;

	void UpdateTablet();
	void FilterTablet(fieldStruct fields);
	void UpdateEditing();
	Recording* BuildRecording(fieldStruct fields);
	Recording* GetRecordingFromTableView();

	void ChangePage(PAGE);

	bool CheckRecordingFields(fieldStruct fields);

public:
	PhoneBookManager() { CurrentBook = nullptr; CurrentRecording = nullptr; };
	~PhoneBookManager() { delete CurrentBook; delete CurrentRecording; };

	int Start();
};

void QFileSavePhoneBook(PhoneBook*, QString path);
PhoneBook* QFileReadPhoneBook(QString path);