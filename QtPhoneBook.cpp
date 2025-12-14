#include "#libQtPhoneBook.h"

void PhoneBookManager::RecreateModel() {
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
            cout << (**iter).GetEmail();

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

Recording* PhoneBookManager::BuildRecording(string* fields) {
    //Recording* output = new Recording(fields[0], fields[2], fields[5], fields[6]);
    Recording* output = new Recording();
    if (output != nullptr) {
        
        output->SetMiddleName(fields[1]);
        output->SetAddress(fields[3]);
        output->SetDateOfBirth(fields[4]);
    }
    return output;
}

int PhoneBookManager::Start() {
    int arg_plug = 0;
    QApplication app(arg_plug, nullptr);

    QWidget window;
    window.setWindowTitle("Phonebook Manager");
    window.resize(920, 540);

    QVBoxLayout* canvas = new QVBoxLayout(&window);
    QStackedWidget* stacked_widget = new QStackedWidget();

    // MAIN MENU 0
    {
        QWidget* main_menu = new QWidget();
        QVBoxLayout* main_layout = new QVBoxLayout(main_menu);

        QLabel* title = new QLabel("Phonebook Manager");
        main_layout->addWidget(title, 0, Qt::AlignHCenter);
        QPushButton* new_pb_button = new QPushButton("New phonebook");
        main_layout->addWidget(new_pb_button, 0, Qt::AlignHCenter);
        QObject::connect(new_pb_button, &QPushButton::clicked, [this, stacked_widget]() {
            CurrentBook = nullptr;
            RecreateModel();
            stacked_widget->setCurrentIndex(PAGE::TABLET);
            });
        QPushButton* load_pb_button = new QPushButton("Load phonebook");
        main_layout->addWidget(load_pb_button, 0, Qt::AlignHCenter);
        QObject::connect(load_pb_button, &QPushButton::clicked, [stacked_widget]() {stacked_widget->setCurrentIndex(PAGE::LOAD); });

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
        QObject::connect(add_btn, &QPushButton::clicked, [this, stacked_widget]() {
            CurrentRecording = nullptr;
            stacked_widget->setCurrentIndex(PAGE::EDIT);
            });

        QLabel* spin_label = new QLabel("Choose a recording");
        panel_layout->addWidget(spin_label);
        QSpinBox* spin = new QSpinBox();
        panel_layout->addWidget(spin);

        QPushButton* change_btn = new QPushButton("Change");
        panel_layout->addWidget(change_btn);
        QObject::connect(change_btn, &QPushButton::clicked, [this, spin, stacked_widget]() {
            CurrentRecording = CurrentBook->GetRecording(spin->value());
            if (CurrentRecording != nullptr)
                stacked_widget->setCurrentIndex(PAGE::EDIT);
            });
        QPushButton* remove_btn = new QPushButton("Remove");
        panel_layout->addWidget(remove_btn);
        QObject::connect(remove_btn, &QPushButton::clicked, [this, spin]() {
            CurrentRecording = CurrentBook->GetRecording(spin->value());
            if (CurrentRecording != nullptr) {
                CurrentBook->RemoveRecording(CurrentRecording);
                CurrentRecording = nullptr;
                RecreateModel();
            }
            });

        QPushButton* save_btn = new QPushButton("Save phonebook");
        panel_layout->addWidget(save_btn);
        QObject::connect(save_btn, &QPushButton::clicked, [stacked_widget]() {stacked_widget->setCurrentIndex(PAGE::SAVE); });
        QPushButton* exit_btn = new QPushButton("Exit without saving");
        panel_layout->addWidget(exit_btn);
        QObject::connect(exit_btn, &QPushButton::clicked, [stacked_widget]() {stacked_widget->setCurrentIndex(PAGE::MAIN); });


        Recordings_table = new QStandardItemModel(0, 7);
        Recordings_table->setHorizontalHeaderLabels(field_names);

        QTableView* table_view = new QTableView;
        table_view->setModel(Recordings_table);
        table_view->setColumnWidth(0, 80); // First name
        table_view->setColumnWidth(1, 80); // Middle name
        table_view->setColumnWidth(2, 80); // Last name
        table_view->setColumnWidth(4, 75); // Date of birth

        phonebook_layout->addWidget(table_view);

        RecreateModel();

        stacked_widget->addWidget(pb_menu);
    }

    // LOAD PHONEBOOK 2
    {
        QWidget* menu = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(menu);

        QLabel* title = new QLabel("Read phonebook from file");
        layout->addWidget(title, 0, Qt::AlignHCenter);
        {
            QHBoxLayout* edit_layout = new QHBoxLayout(menu);

            QLabel* label = new QLabel("Write path to txt-file here");
            edit_layout->addWidget(label);

            QLineEdit* lineEdit = new QLineEdit();
            edit_layout->addWidget(lineEdit);

            QPushButton* submit_button = new QPushButton("Submit");
            edit_layout->addWidget(submit_button);
            QObject::connect(submit_button, &QPushButton::clicked, [this, lineEdit, stacked_widget]() {
                CurrentBook = new PhoneBook(lineEdit->text().toStdString());
                RecreateModel();
                stacked_widget->setCurrentIndex(PAGE::TABLET); 
                });

            layout->addLayout(edit_layout);
        }

        QPushButton* exit_btn = new QPushButton("Exit");
        layout->addWidget(exit_btn);
        QObject::connect(exit_btn, &QPushButton::clicked, [stacked_widget]() {stacked_widget->setCurrentIndex(PAGE::MAIN); });

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
            QObject::connect(submit_button, &QPushButton::clicked, [this, lineEdit, stacked_widget]() {
                CurrentBook->Rewrite(lineEdit->text().toStdString());
                stacked_widget->setCurrentIndex(PAGE::TABLET);
                });

            layout->addLayout(edit_layout);
        }

        QPushButton* exit_btn = new QPushButton("Exit");
        layout->addWidget(exit_btn);
        QObject::connect(exit_btn, &QPushButton::clicked, [stacked_widget]() {stacked_widget->setCurrentIndex(PAGE::MAIN); });

        stacked_widget->addWidget(menu);
    }

    // EDIT RECORDING 4
    {
        QWidget* menu = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(menu);

        QLabel* title = new QLabel("Edit recording");
        layout->addWidget(title);
        
        QLineEdit* lineEdits[7];

        for (int i = 0; i < 7; i++) {
            QHBoxLayout* edit_layout = new QHBoxLayout(menu);

            QLabel* label = new QLabel(field_names.at(i));
            edit_layout->addWidget(label);

            lineEdits[i] = new QLineEdit();
            if (CurrentRecording != nullptr)
                continue;
            edit_layout->addWidget(lineEdits[i]);

            layout->addLayout(edit_layout);
        }
        if (CurrentRecording != nullptr) {
            lineEdits[0]->setText(QString::fromStdString(CurrentRecording->GetFirstName()));
            lineEdits[1]->setText(QString::fromStdString(CurrentRecording->GetMiddleName()));
            lineEdits[2]->setText(QString::fromStdString(CurrentRecording->GetLastName()));
            lineEdits[3]->setText(QString::fromStdString(CurrentRecording->GetAddress()));
            lineEdits[4]->setText(QString::fromStdString(CurrentRecording->GetDateOfBirth()));
            lineEdits[5]->setText(QString::fromStdString(CurrentRecording->GetEmail()));
        }

        QPushButton* save_btn = new QPushButton("Save");
        layout->addWidget(save_btn);
        QObject::connect(save_btn, &QPushButton::clicked, [this, lineEdits, stacked_widget]() {
            //Recording* pre_record = CurrentBook->GetRecording("_", "_", "_", "_", "_", lineEdits[5]->text().toStdString(), "_");

            auto f = [lineEdits](int i) { return lineEdits[i]->text().toStdString(); };
            string fields[7] = {f(0), f(1), f(2), f(3), f(4), f(5), f(6)};
            Recording* new_record = BuildRecording(fields);
            CurrentBook->AddRecording(*new_record);
            CurrentRecording = nullptr;
            stacked_widget->setCurrentIndex(PAGE::TABLET);
            });
        QPushButton* back_btn = new QPushButton("Back");
        layout->addWidget(back_btn);
        QObject::connect(back_btn, &QPushButton::clicked, [this, stacked_widget]() {
            CurrentRecording = nullptr;
            stacked_widget->setCurrentIndex(PAGE::TABLET);
            });

        stacked_widget->addWidget(menu);
    }

    stacked_widget->setCurrentIndex(PAGE::MAIN);

    canvas->addWidget(stacked_widget);
    window.show();
    return app.exec();
}