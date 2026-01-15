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

#include <QtSql>

enum PAGE {
	MAIN = 0,
	TABLET = 1,
	LOAD = 2,
	SAVE = 3,
	EDIT = 4
};

enum TABLESORTEDFIELD {
	NONE,
	FIRST_NAME,
	MIDDLE_NAME,
	LAST_NAME,
	ADDRESS_F,
	DATE,
	EMAIL_F,
	PHONES
};

enum SORTORDER {
	DESCENDING,
	ASCENDING
};

struct tableSort {
	TABLESORTEDFIELD field;
	SORTORDER order;
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
	QSqlDatabase DataBase;

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

	void ConnectToBD();
	void WriteToBD(QString);
	void ReadFromBD(QString);
	void DisconnectWithBD();

public:
	PhoneBookManager() { CurrentBook = nullptr; CurrentRecording = nullptr; };
	~PhoneBookManager() { delete CurrentBook; delete CurrentRecording; };

	int Start();
};

void QFileSavePhoneBook(PhoneBook*, QString path);
PhoneBook* QFileReadPhoneBook(QString path);
QDate ConvertStringToQDate(string dateStr);
QStringList pgArrayToStringList(QString pgArray);