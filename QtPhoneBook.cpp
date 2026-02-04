#include "#libQtPhoneBook.h"

void PhoneBookManager::UpdateTablet() {
    Recordings_table->removeRows(0, Recordings_table->rowCount());
    if (CurrentBook != nullptr) {
        const list<Recording*> Recordings = CurrentBook->GetAllRecordings();
        for (list<Recording*>::const_iterator iter = Recordings.begin(); iter != Recordings.end(); iter++) {
            QList<QStandardItem*> new_row;
            new_row.append(new QStandardItem(QString::fromStdString((**iter).GetFirstName())));
            new_row.append(new QStandardItem(QString::fromStdString((**iter).GetMiddleName())));
            new_row.append(new QStandardItem(QString::fromStdString((**iter).GetLastName())));
            new_row.append(new QStandardItem(QString::fromStdString((**iter).GetAddress())));
            new_row.append(new QStandardItem(QString::fromStdString((**iter).GetDateOfBirth())));
            new_row.append(new QStandardItem(QString::fromStdString((**iter).GetEmail())));

            QString str_phones = "";
            list<string> phones = (**iter).GetPhoneNumbers();
            for (list<string>::iterator iter = phones.begin(); iter != phones.end(); iter++) {
                str_phones += QString::fromStdString(*iter);
                str_phones += "\n";
            }
            str_phones.chop(1);
            new_row.append(new QStandardItem(str_phones));

            Recordings_table->appendRow(new_row);
        }
    }
}
void PhoneBookManager::FilterTablet(fieldStruct fields) {
    for (int i = 0; i < Recordings_table->rowCount(); i++) {
        if (
            (fields.firstName != "" and !Recordings_table->item(i, 0)->text().contains(QString::fromStdString(fields.firstName))) or
            (fields.middleName != "" and !Recordings_table->item(i, 1)->text().contains(QString::fromStdString(fields.middleName))) or
            (fields.lastName != "" and !Recordings_table->item(i, 2)->text().contains(QString::fromStdString(fields.lastName))) or
            (fields.address != "" and !Recordings_table->item(i, 3)->text().contains(QString::fromStdString(fields.address))) or
            (fields.dateOfBirth != "" and !Recordings_table->item(i, 4)->text().contains(QString::fromStdString(fields.dateOfBirth))) or
            (fields.email != "" and !Recordings_table->item(i, 5)->text().contains(QString::fromStdString(fields.email))) or
            (fields.phones != "" and !Recordings_table->item(i, 6)->text().contains(QString::fromStdString(fields.phones)))
            )
            table_view->hideRow(i);
        else
            table_view->showRow(i);
    }
}
void PhoneBookManager::UpdateEditing() {
    phoneEdits->DeleteEveryElement();

    if (CurrentRecording == nullptr) {
        for (int i = 0; i < 6; i++) {
            lineEdits[i]->setText("");
        }
    }
    else {
        lineEdits[0]->setText(QString::fromStdString(CurrentRecording->GetFirstName()));
        lineEdits[1]->setText(QString::fromStdString(CurrentRecording->GetMiddleName()));
        lineEdits[2]->setText(QString::fromStdString(CurrentRecording->GetLastName()));
        lineEdits[3]->setText(QString::fromStdString(CurrentRecording->GetAddress()));
        lineEdits[4]->setText(QString::fromStdString(CurrentRecording->GetDateOfBirth()));
        lineEdits[5]->setText(QString::fromStdString(CurrentRecording->GetEmail()));
        list<string> list_phone = CurrentRecording->GetPhoneNumbers();
        for (list<string>::iterator iter = list_phone.begin(); iter != list_phone.end(); iter++) {
            phoneEdits->CopyElement(*iter);
        }
    }
    
}

Recording* PhoneBookManager::BuildRecording(fieldStruct fields) {
    Recording* output = new Recording(fields.firstName, fields.lastName, fields.email, "");
    if (output != nullptr) {
        output->SetMiddleName(fields.middleName);
        output->SetAddress(fields.address);
        output->SetDateOfBirth(fields.dateOfBirth);
    }

    output->RemovePhoneNumber("");
    istringstream iss(fields.phones);
    string phone_number;
    while (getline(iss, phone_number)) {
        output->AddPhoneNumber(phone_number);
    }
    return output;
}
Recording* PhoneBookManager::GetRecordingFromTableView() {
    QModelIndex currentIndex = table_view->currentIndex();

    if (!currentIndex.isValid() or !Recordings_table)
        return nullptr;

    int selectedRow = currentIndex.row();
    int colCount = Recordings_table->columnCount();

    Recording* output = CurrentBook->GetRecording(
        Recordings_table->item(selectedRow, 0)->text().toStdString(),
        Recordings_table->item(selectedRow, 2)->text().toStdString(),
        Recordings_table->item(selectedRow, 1)->text().toStdString(),
        Recordings_table->item(selectedRow, 3)->text().toStdString(),
        Recordings_table->item(selectedRow, 4)->text().toStdString(),
        Recordings_table->item(selectedRow, 5)->text().toStdString(),
        Recordings_table->item(selectedRow, 6)->text().split('\n').first().toStdString()
    );

    return output;
}

void PhoneBookManager::ChangePage(PAGE page) {
    switch (page) {
    case MAIN:
        if (CurrentRecording != nullptr) {
            delete CurrentRecording;
            CurrentRecording = nullptr;
        }
        if (CurrentBook != nullptr) {
            delete CurrentBook;
            CurrentBook = nullptr;
        }
        break;
    case TABLET:
        UpdateTablet();
        CurrentRecording = nullptr;
        break;
    case LOAD:
        break;
    case SAVE:
        break;
    case EDIT:
        UpdateEditing();
        break;
    default:
        return;
    }
    stacked_widget->setCurrentIndex(page);
}
bool PhoneBookManager::CheckRecordingFields(fieldStruct fields) {
    istringstream iss(fields.phones);
    string phone_number;
    while (getline(iss, phone_number)) {
        if (Recording::CheckPhoneNumber(phone_number))
            return true;
    }
    return (
        Recording::CheckName(fields.firstName) and
        Recording::CheckName(fields.middleName) and
        Recording::CheckName(fields.lastName) and
        Recording::CheckDateOfBirth(fields.dateOfBirth) and
        Recording::CheckEmail(fields.email, fields.firstName)
        );
};

QVListWidget::QVListWidget(int max_size) {
    this->max_size = max_size > 0 ? max_size : 1;

    add_btn = new QPushButton("Add new element");
    this->addWidget(add_btn);
    QObject::connect(add_btn, &QPushButton::clicked, [this]() { AddElement(); });
}
QVListWidget::~QVListWidget() {
    delete add_btn;
}
void QVListWidget::AddElement() {
    int current_size = list_line_edit.size();
    if (current_size >= max_size) {
        cout << "Too much text fields!\n";
        return;
    }

    QHBoxLayout* new_box = new QHBoxLayout;
    list_box.push_back(new_box);
    this->addLayout(new_box);

    QLineEdit* new_line_edit = new QLineEdit;
    list_line_edit.push_back(new_line_edit);
    new_box->addWidget(new_line_edit);

    QPushButton* new_delete_btn = new QPushButton("Delete");
    QObject::connect(new_delete_btn, &QPushButton::clicked,
        [this, new_box, new_line_edit, new_delete_btn]() { DeleteElement(new_box, new_line_edit, new_delete_btn); });
    list_delete_btn.push_back(new_delete_btn);
    new_box->addWidget(new_delete_btn);
}
void QVListWidget::CopyElement(string new_element) {
    AddElement();
    list<QLineEdit*>::iterator iter = list_line_edit.end();
    iter--;
    (**iter).setText(QString::fromStdString(new_element));
}
void QVListWidget::DeleteElement(QHBoxLayout* layout, QLineEdit* line_edit, QPushButton* delete_btn) {
    list_delete_btn.remove(delete_btn);
    delete delete_btn;
    list_line_edit.remove(line_edit);
    delete line_edit;
    list_box.remove(layout);
    delete layout;
}
void QVListWidget::DeleteEveryElement() {
    while (!list_line_edit.empty())
        DeleteElement(*list_box.begin(), *list_line_edit.begin(), *list_delete_btn.begin());
}
string QVListWidget::GetEveryElement() {
    if (list_line_edit.size() <= 0)
        return "";

    string output = "";
    string addition;

    for (list<QLineEdit*>::iterator iter = list_line_edit.begin(); iter != list_line_edit.end(); iter++) {
        addition = (*iter)->text().toStdString();
        if (addition == "")
            continue;

        output += addition;
        output += '\n';
    }

    return output;
}


int PhoneBookManager::Start() {
    int arg_plug = 0;
    QApplication app(arg_plug, nullptr);

    QFont appFont = app.font();
    appFont.setPointSize(12);

    QApplication::setFont(appFont);

    QWidget window;
    window.setWindowTitle("Phonebook Manager");
    window.resize(1400, 540);

    QVBoxLayout* canvas = new QVBoxLayout(&window);
    stacked_widget = new QStackedWidget();

    // MAIN MENU 0
    {
        QWidget* main_menu = new QWidget();
        QVBoxLayout* main_layout = new QVBoxLayout(main_menu);

        QLabel* title = new QLabel("Phonebook Manager");
        main_layout->addWidget(title, 0, Qt::AlignHCenter);
        QPushButton* new_pb_button = new QPushButton("New phonebook");
        main_layout->addWidget(new_pb_button, 0, Qt::AlignHCenter);
        QObject::connect(new_pb_button, &QPushButton::clicked, [this]() {
            CurrentBook = new PhoneBook();
            ChangePage(PAGE::TABLET);
            });
        QPushButton* load_pb_button = new QPushButton("Load phonebook");
        main_layout->addWidget(load_pb_button, 0, Qt::AlignHCenter);
        QObject::connect(load_pb_button, &QPushButton::clicked, [this]() {ChangePage(PAGE::LOAD); });

        stacked_widget->addWidget(main_menu);
    }
    
    // PHONEBOOK MENU 1
    {
        QWidget* pb_menu = new QWidget();
        QHBoxLayout* phonebook_layout = new QHBoxLayout(pb_menu);
        QVBoxLayout* panel_layout = new QVBoxLayout();
        phonebook_layout->addLayout(panel_layout);


        QPushButton* add_btn = new QPushButton("Add");
        panel_layout->addWidget(add_btn);
        QObject::connect(add_btn, &QPushButton::clicked, [this]() {
            CurrentRecording = nullptr;
            ChangePage(PAGE::EDIT);
            });

        QPushButton* change_btn = new QPushButton("Change");
        panel_layout->addWidget(change_btn);
        QObject::connect(change_btn, &QPushButton::clicked, [this]() {
            CurrentRecording = GetRecordingFromTableView();
            if (CurrentRecording != nullptr)
                ChangePage(PAGE::EDIT);
            });
        QPushButton* remove_btn = new QPushButton("Remove");
        panel_layout->addWidget(remove_btn);
        QObject::connect(remove_btn, &QPushButton::clicked, [this]() {
            CurrentRecording = GetRecordingFromTableView();
            if (CurrentRecording != nullptr) {
                CurrentBook->RemoveRecording(CurrentRecording);
                CurrentRecording = nullptr;
                UpdateTablet();
            }
            });

        QPushButton* save_btn = new QPushButton("Save phonebook");
        panel_layout->addWidget(save_btn);
        QObject::connect(save_btn, &QPushButton::clicked, [this]() { ChangePage(PAGE::SAVE); });
        QPushButton* exit_btn = new QPushButton("Exit without saving");
        panel_layout->addWidget(exit_btn);
        QObject::connect(exit_btn, &QPushButton::clicked, [this]() { ChangePage(PAGE::MAIN); });


        QVBoxLayout* tablet_layout = new QVBoxLayout();
        phonebook_layout->addLayout(tablet_layout);

        Recordings_table = new QStandardItemModel(0, 7);
        Recordings_table->setHorizontalHeaderLabels(field_names);

        table_view = new QTableView;
        table_view->setModel(Recordings_table);

        // [0] = first name
        // [1] = middle name
        // [2] = last name
        // [3] = address
        // [4] = Date of birth
        // [5] = eMail
        // [6] = phone
        int table_width[7] = { 150, 150, 150, 175, 150, 150, 250 };
        for (int i = 0; i < 7; i++) {
            table_view->setColumnWidth(i, table_width[i]);
        }

        QHBoxLayout* find_fields_layout = new QHBoxLayout();
        QLineEdit* find_line_edits[7];
        for (int i = 0; i < 7; i++) {
            find_line_edits[i] = new QLineEdit();
            find_fields_layout->addWidget(find_line_edits[i]);
        }
        tablet_layout->addLayout(find_fields_layout);

        QPushButton* find_btn = new QPushButton("Filter");
        QObject::connect(find_btn, &QPushButton::clicked, [this, find_line_edits]() {
            auto f = [this, find_line_edits](int i) { return find_line_edits[i]->text().toStdString(); };
            fieldStruct fields;
            fields.firstName = f(0);
            fields.middleName = f(1);
            fields.lastName = f(2);
            fields.address = f(3);
            fields.dateOfBirth = f(4);
            fields.email = f(5);
            fields.phones = f(6);
            FilterTablet(fields);
            });
        find_fields_layout->addWidget(find_btn);


        QHBoxLayout* sort_layout = new QHBoxLayout();
        tablet_layout->addLayout(sort_layout);

        QPushButton* sort_btns[7];
        TABLESORTED* current_sort = new TABLESORTED(TABLESORTED::NONE);
        for (int i = 0; i < 7; i++) {
            sort_btns[i] = new QPushButton("V");
            sort_layout->addWidget(sort_btns[i]);
            QObject::connect(sort_btns[i], &QPushButton::clicked, [this, i, current_sort, sort_btns]() {
                for (int j = 0; j < 7; j++)
                    sort_btns[i]->setText("V");
                if (*current_sort != TABLESORTED::NONE && *current_sort/ 2 == i + 1 && *current_sort ^ 1) {
                    table_view->sortByColumn(i, Qt::SortOrder::AscendingOrder);
                    *current_sort = static_cast<TABLESORTED>(2*i);
                }
                else {
                    sort_btns[i]->setText("^");
                    table_view->sortByColumn(i, Qt::SortOrder::DescendingOrder);
                    *current_sort = static_cast<TABLESORTED>(2 * i + 1);
                }
                cout << *current_sort << endl;
                });
        }

        tablet_layout->addWidget(table_view);
        UpdateTablet();

        stacked_widget->addWidget(pb_menu);
    }

    // LOAD PHONEBOOK 2
    {
        QWidget* menu = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(menu);

        QLabel* title = new QLabel("Read phonebook from file");
        layout->addWidget(title, 0, Qt::AlignHCenter);

        QHBoxLayout* edit_layout = new QHBoxLayout(menu);

        QLabel* label = new QLabel("Write path to txt-file here");
        edit_layout->addWidget(label);

        QLineEdit* lineEdit = new QLineEdit();
        edit_layout->addWidget(lineEdit);

        QPushButton* submit_button = new QPushButton("Submit");
        edit_layout->addWidget(submit_button);
        QObject::connect(submit_button, &QPushButton::clicked, [this, lineEdit]() {
            CurrentBook = QFileReadPhoneBook(lineEdit->text());
            ChangePage(PAGE::TABLET);
            });

        layout->addLayout(edit_layout);

        QPushButton* exit_btn = new QPushButton("Exit");
        layout->addWidget(exit_btn);
        QObject::connect(exit_btn, &QPushButton::clicked, [this]() {ChangePage(PAGE::MAIN); });

        stacked_widget->addWidget(menu);
    }

    // SAVE PHONEBOOK 3
    {
        QWidget* menu = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(menu);

        QLabel* title = new QLabel("Save phonebook to file");
        layout->addWidget(title, 0, Qt::AlignHCenter);
        {
            QHBoxLayout* edit_layout = new QHBoxLayout(menu);

            QLabel* label = new QLabel("Write path to txt-file here");
            edit_layout->addWidget(label);

            QLineEdit* lineEdit = new QLineEdit();
            edit_layout->addWidget(lineEdit);

            QPushButton* submit_button = new QPushButton("Submit");
            edit_layout->addWidget(submit_button);
            QObject::connect(submit_button, &QPushButton::clicked, [this, lineEdit]() {
                QFileSavePhoneBook(CurrentBook, lineEdit->text());
                ChangePage(PAGE::TABLET);
                });

            layout->addLayout(edit_layout);
        }

        QPushButton* exit_btn = new QPushButton("Exit");
        layout->addWidget(exit_btn);
        QObject::connect(exit_btn, &QPushButton::clicked, [this]() {ChangePage(PAGE::MAIN); });

        stacked_widget->addWidget(menu);
    }

    // EDIT RECORDING 4
    {
        QWidget* menu = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(menu);

        QLabel* title = new QLabel("Edit recording");
        layout->addWidget(title);

        for (int i = 0; i < 6; i++) {
            QHBoxLayout* edit_layout = new QHBoxLayout(menu);

            QLabel* label = new QLabel(field_names.at(i));
            edit_layout->addWidget(label);

            lineEdits[i] = new QLineEdit();
            if (CurrentRecording != nullptr)
                continue;
            edit_layout->addWidget(lineEdits[i]);

            layout->addLayout(edit_layout);
        }

        phoneEdits = new QVListWidget(10);
        layout->addLayout(phoneEdits);

        QPushButton* save_btn = new QPushButton("Save");
        layout->addWidget(save_btn);
        QObject::connect(save_btn, &QPushButton::clicked, [this]() {
            auto f = [this](int i) { return lineEdits[i]->text().toStdString(); };
            fieldStruct fields;
            fields.firstName = f(0);
            fields.middleName = f(1);
            fields.lastName = f(2);
            fields.address = f(3);
            fields.dateOfBirth = f(4);
            fields.email = f(5);
            fields.phones = phoneEdits->GetEveryElement();

            if (!CheckRecordingFields(fields)) {
                cout << "Wrong fields" << endl;
                return;
            }

            if (CurrentRecording == nullptr) {
                Recording* new_record = BuildRecording(fields);
                CurrentBook->AddRecording(*new_record);
            }
            else {
                if (fields.firstName != "")
                    CurrentRecording->SetFirstName(fields.firstName);
                if (fields.middleName != "")
                    CurrentRecording->SetMiddleName(fields.middleName);
                if (fields.lastName != "")
                    CurrentRecording->SetLastName(fields.lastName);
                if (fields.address != "")
                    CurrentRecording->SetAddress(fields.address);
                if (fields.dateOfBirth != "")
                    CurrentRecording->SetDateOfBirth(fields.dateOfBirth);
                if (fields.email != "")
                    CurrentRecording->SetEmail(fields.email);

                CurrentRecording->RemoveEveryPhoneNumber();
                istringstream iss(phoneEdits->GetEveryElement());
                string phone_number;
                while (getline(iss, phone_number)) {
                    if (!CurrentRecording->DoesContainPhoneNumber(phone_number))
                        CurrentRecording->AddPhoneNumber(phone_number);
                }
            };
            ChangePage(PAGE::TABLET);
            });
        QPushButton* back_btn = new QPushButton("Back");
        layout->addWidget(back_btn);
        QObject::connect(back_btn, &QPushButton::clicked, [this]() { ChangePage(PAGE::TABLET); });

        stacked_widget->addWidget(menu);
    }

    stacked_widget->setCurrentIndex(PAGE::MAIN);

    canvas->addWidget(stacked_widget);
    window.show();
    return app.exec();
}


void QFileSavePhoneBook(PhoneBook* phonebook, QString path) {
    if (phonebook == nullptr || path == "")
        return;

    QFile writeFile(path);
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&writeFile);

        list<Recording*> recordings = phonebook->GetAllRecordings();
        for (list<Recording*>::iterator rec = recordings.begin(); rec != recordings.end(); rec++) {
            out << QString::fromStdString((**rec).GetLastName()) << endl;
            out << QString::fromStdString((**rec).GetFirstName()) << endl;
            string middleName = (**rec).GetMiddleName();
            out << ((middleName != "") ? QString::fromStdString(middleName) : "_") << endl;
            string address = (**rec).GetAddress();
            out << ((address != "") ? QString::fromStdString(address) : "_") << endl;
            string dateOfBirth = (**rec).GetDateOfBirth();
            out << ((dateOfBirth != "") ? QString::fromStdString(dateOfBirth) : "_") << endl;
            out << QString::fromStdString((**rec).GetEmail()) << endl;

            list<string> phones = (**rec).GetPhoneNumbers();
            list<string>::iterator phone;
            for (phone = phones.begin(); phone != phones.end(); phone++) {
                out << QString::fromStdString(*phone) << endl;
            }
            out << endl;
        }
    }
}

PhoneBook* QFileReadPhoneBook(QString path) {
    if (path == "")
        return nullptr;

    PhoneBook* output = new PhoneBook();

    QFile readFile(path);
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&readFile);
        int lineNumber = 1;

        int i = 0;
        string setup_str[6];
        list<string> setup_phones;

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == "\0") {
                Recording* new_rec = new Recording();
                new_rec->SetLastName(setup_str[0]);
                new_rec->SetFirstName(setup_str[1]);
                new_rec->SetMiddleName(setup_str[2]);
                new_rec->SetAddress(setup_str[3]);
                new_rec->SetDateOfBirth(setup_str[4]);
                new_rec->SetEmail(setup_str[5]);
                for (list<string>::iterator iter = setup_phones.begin(); iter != setup_phones.end(); iter++)
                    new_rec->AddPhoneNumber(*iter);
                output->AddRecording(*new_rec);
                setup_phones.clear();
                i = 0;
                continue;
            }
            if (i < 6) {
                setup_str[i] = line.toStdString();
            }
            else {
                setup_phones.push_back(line.toStdString());
            }
            i++;
        }
        readFile.close();
    }

    return output;

}
