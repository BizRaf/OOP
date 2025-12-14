#pragma once
#include "#libPhoneBook.h"

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

enum PAGE {
	MAIN = 0,
	TABLET = 1,
	LOAD = 2,
	SAVE = 3,
	EDIT = 4
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

class PhoneBookManager {
private:
	PhoneBook* CurrentBook;
	Recording* CurrentRecording;
	QStandardItemModel* Recordings_table;

	void RecreateModel();
	Recording* BuildRecording(string* fields);

public:
	PhoneBookManager() { CurrentBook = nullptr; CurrentRecording = nullptr; };
	~PhoneBookManager() { delete CurrentBook; delete CurrentRecording; };

	int Start();
};