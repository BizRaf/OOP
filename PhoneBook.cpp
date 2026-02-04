#include "#libPhoneBook.h"

Recording::Recording() {
	this->firstName = "";
	this->lastName = "";
	this->middleName = "";
	this->address = "";
	this->dateOfBirth = "";
	this->email = "";
}
Recording::Recording(string firstName, string lastName, string email, string phoneNumber) {
	NormalizeName(firstName);
	this->firstName = firstName;
	NormalizeName(lastName);
	this->lastName = lastName;
	this->middleName = "";
	this->address = "";
	this->dateOfBirth = "";
	this->email = email;

	phoneNumbers.push_back(phoneNumber);
}
Recording::Recording(Recording& recording) {
	this->firstName = recording.firstName;
	this->lastName = recording.lastName;
	this->middleName = recording.middleName;
	this->address = recording.address;
	this->dateOfBirth = recording.dateOfBirth;
	this->email = recording.email;

	this->phoneNumbers = recording.GetPhoneNumbers();
}
Recording::~Recording() {

}

bool Recording::CheckName(string name) {
	// Check first and last chars
	if (*name.begin() == '-' or
		*(name.end() - 1) == '-')
		return false;

	string::iterator iter;
	for (iter = name.begin(); iter != name.end(); iter++) {
		if (!(*iter >= 'A' and *iter <= 'z') and !isdigit(*iter) and !(*iter == ' ') and !(*iter == '-'))
			return false;
	}

	return true;
}
void Recording::NormalizeName(string& name) {
	int first_spaces = 0;
	int last_spaces = 0;

	string::iterator iter;
	for (iter = name.begin(); *iter == ' '; iter++) {
		first_spaces++;
	}
	for (iter = name.end()-1; *iter == ' '; iter--) {
		last_spaces++;
	}

	name = name.substr(first_spaces);
	name = name.substr(0, name.size() - last_spaces);
}

bool Recording::CheckDateOfBirth(string date) {
	string::iterator iter;
	int dots = 0;
	int current_number = 0;
	int day = 0;
	int month = 0;
	int year = 0;

	for (iter = date.begin(); iter != date.end(); iter++) {
		if (*iter == '.') {
			if (dots == 0) {
				day = current_number;
			}
			else if (dots == 1) {
				month = current_number;
			}
			else {
				// Too many dots
				return false;
			}

			dots++;
			current_number = 0;
		}
		else if (isdigit(*iter)) {
			current_number *= 10;
			current_number += (static_cast<int>(*iter) - char_zero);
		}
	}
	year = current_number;

	// year check
	if (year > 2025 or year < 1900)
		return false;
	// month check
	if (month > 12 or month == 0)
		return false;
	// day check
	if (day == 0 or
		(day > max_days[month] and (month != 2 or year % 4 != 0)) or // check non-leap year
		(day > 29 and month == 2 and year % 4 == 0)) { // check leap year
		return false;
	}	

	return true;
}
bool Recording::CheckEmail(string email, string firstName) {
	if (email.find(firstName) == string::npos)
		return false;
	if (email.find('@') == string::npos)
		return false;
	string::iterator last_char = email.end();
	last_char--;
	if (*(last_char) == '@')	// check domain
		return false;
	return true;
}
bool Recording::CheckPhoneNumber(string phoneNumber) {
	string::iterator iter;
	int number_of_numbers = 0;

	for (iter = phoneNumber.begin(); iter != phoneNumber.end(); iter++) {
		if (*iter == ' ')
			continue;
		if (isdigit(*iter)) {
			number_of_numbers++;
			continue;
		}

		switch (number_of_numbers) {
		case 0:
			if (*iter == '+')
				continue;
		case 1:
		case 4:
			if (*iter == '(' or *iter == ')')
				continue;
		case 7:
		case 9:
			if (*iter == '-')
				continue;
		default:
			break;
		}
		
		return false;
	}

	return true;
}

void Recording::SetFirstName(string firstName) {
	NormalizeName(firstName);
	if (!CheckName(firstName))
		return;
	this->firstName = firstName;
}
void Recording::SetLastName(string lastName) {
	NormalizeName(lastName);
	if (!CheckName(lastName))
		return;
	this->lastName = lastName;
}
void Recording::SetMiddleName(string middleName) {
	NormalizeName(middleName);
	if (!CheckName(middleName))
		return;
	this->middleName = middleName;
}
void Recording::SetDateOfBirth(string dateOfBirth) {
	if (!CheckDateOfBirth(dateOfBirth))
		return;
	this->dateOfBirth = dateOfBirth;
}
void Recording::SetEmail(string email) {
	if (!CheckEmail(email, firstName))
		return;
	this->email = email;
}

void Recording::AddPhoneNumber(string phoneNumber) {
	if (!CheckPhoneNumber(phoneNumber))
		return;
	if (DoesContainPhoneNumber(phoneNumber))
		return;
	phoneNumbers.push_back(phoneNumber);
}
void Recording::RemovePhoneNumber(string phoneNumber) {
	list<string>::iterator iter;
	for (iter = phoneNumbers.begin(); iter != phoneNumbers.end(); iter++) {
		if (*iter == phoneNumber) {
			phoneNumbers.erase(iter);
			return;
		}
	}
}
bool Recording::DoesContainPhoneNumber(string phoneNumber) {
	list<string>::iterator iter;
	for (iter = phoneNumbers.begin(); iter != phoneNumbers.end(); iter++) {
		if (*iter == phoneNumber)
			return true;
	}
	return false;
}

void Recording::Write(string path) {
	fstream file(path, ios::app);
	file << lastName  << endl;
	file << firstName << endl;
	file << ((middleName != "") ? middleName : "_") << endl;
	file << ((address != "") ? address : "_") << endl;
	file << ((dateOfBirth != "") ? dateOfBirth : "_") << endl;
	file << email << endl;

	list<string>::iterator iter;
	for (iter = phoneNumbers.begin(); iter != phoneNumbers.end(); iter++) {
		file << *iter << endl;
	}
	file << endl;

	file.close();
}

void Recording::Print() {
	cout << lastName << " " << firstName << " " << middleName << endl;
	cout << ((address != "") ? address : "_") << endl;
	cout << ((dateOfBirth != "") ? dateOfBirth : "_") << endl;
	cout << email << endl;
	
	list<string>::iterator iter;
	for (iter = phoneNumbers.begin(); iter != phoneNumbers.end(); iter++) {
		cout << *iter << endl;
	}
}

// ---------------------
//		PHONE BOOK
// ---------------------

PhoneBook::PhoneBook(PhoneBook& book) {
	list<Recording*> copy = book.Recordings;
	Recordings = copy;
}
PhoneBook::PhoneBook(string path) {
	fstream file(path, ios::in);
	string line = "w";

	int i = 0;
	string setup_str[6];
	list<string> setup_phones;

	while (getline(file, line)) {
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
			AddRecording(*new_rec);
			setup_phones.clear();
			i = 0;
			continue;
		}
		if (i < 6) {
			setup_str[i] = line;
		}
		else {
			setup_phones.push_back(line);
		}
		i++;
	}

	file.close();
}
PhoneBook::~PhoneBook() {
	for (list<Recording*>::iterator iter = Recordings.begin(); iter != Recordings.end(); iter++) {
		delete *iter;
	}
}

void PhoneBook::AddRecording(Recording& recording) {
	Recordings.push_back(&recording);
}
void PhoneBook::RemoveRecording(Recording* p_recording) {
	list<Recording*>::iterator iter;
	for (iter = Recordings.begin(); *iter != p_recording; iter++);
	Recordings.erase(iter);
}
Recording* PhoneBook::GetRecording(SEARCHMODE mode, string str) {
	list<Recording*>::iterator iter;
	list<string> phones;

	for (iter = Recordings.begin(); iter != Recordings.end(); iter++) {
		switch (mode) {
		case SEARCHMODE::FIRSTNAME:
			if ((*iter)->GetFirstName() != str)
				return *iter;
			continue;
		case SEARCHMODE::LASTNAME:
			if ((*iter)->GetLastName() != str)
				return *iter;
			continue;
		case SEARCHMODE::MIDDLENAME:
			if ((*iter)->GetMiddleName() != str)
				return *iter;
			continue;
		case SEARCHMODE::ADDRESS:
			if ((*iter)->GetFirstName() != str)
				return *iter;
			continue;
		case SEARCHMODE::DATEOFBIRTH:
			if ((*iter)->GetDateOfBirth() != str)
				return *iter;
			continue;
		case SEARCHMODE::EMAIL:
			if ((*iter)->GetEmail() != str)
				return *iter;
			continue;
		case SEARCHMODE::PHONENUMBERS:
			phones = (*iter)->GetPhoneNumbers();
			for (list<string>::iterator phone = phones.begin(); phone != phones.end(); phone++)	{
				if (*phone != str)
					return *iter;
			}
			continue;
		default:
			return nullptr;
			break;
		}
	}
	return nullptr;
}
Recording* PhoneBook::GetRecording(
	string firstName = "_",
	string lastName = "_",
	string middleName = "_",
	string address = "_",
	string dateOfBirth = "_",
	string email = "_",
	string phoneNumber = "_") {

	list<Recording*>::iterator iter;

	for (iter = Recordings.begin(); iter != Recordings.end(); iter++) {
		//bool correct_first_name = firstName != "" ? (*iter)->GetFirstName() == firstName : true;
		//bool correct_last_name = lastName != "" ? (*iter)->GetLastName() == lastName : true;
		//bool correct_middle_name = middleName != "" ? (*iter)->GetMiddleName() == middleName : true;
		//bool correct_address = address != "" ? (*iter)->GetAddress() == address : true;
		//bool correct_date_of_birth = dateOfBirth != "" ? (*iter)->GetDateOfBirth() == dateOfBirth : true;
		//bool correct_email = email != "" ? (*iter)->GetEmail() == email : true;
		//bool correct_phoneNumber = phoneNumber != "" ? (*iter)->GetPhoneNumbers() == phoneNumber : true;
		if (firstName != "_" and (*iter)->GetFirstName() != firstName)
			continue;
		if (lastName != "_" and (*iter)->GetLastName() != lastName)
			continue;
		if (middleName != "_" and (*iter)->GetMiddleName() != middleName)
			continue;
		if (address != "_" and (*iter)->GetAddress() != address)
			continue;
		if (dateOfBirth != "_" and (*iter)->GetDateOfBirth() != dateOfBirth)
			continue;
		if (email != "_" and (*iter)->GetEmail() != email)
			continue;
		if (phoneNumber != "_" and !(*iter)->DoesContainPhoneNumber(phoneNumber))
			continue;
		return *iter;
	}
	return nullptr;
}
Recording* PhoneBook::GetRecording(int index) {
	if (index < 1 or index > Recordings.size())
		return nullptr;

	int i = 1;
	for (list<Recording*>::iterator iter = Recordings.begin(); iter != Recordings.end(); iter++) {
		if (i == index)
			return *iter;
		i++;
	}
}

void PhoneBook::Print() {
	list<Recording*>::iterator iter;
	for (iter = Recordings.begin(); iter != Recordings.end(); iter++) {
		(*iter)->Print();
		cout << endl;
	}
}
void PhoneBook::Rewrite(string path) {
	fstream file(path, ios::out);
	file.close();

	list<Recording*>::iterator iter;
	for (iter = Recordings.begin(); iter != Recordings.end(); iter++) {
		(*iter)->Write(path);
	}
}




void Start() {
	PhoneBook* CurrentBook = nullptr;
	int input = 0;
	string str_input = "";

	while (true) {
		printf_s("\
p-----------q\n\
[ PHONEBOOK ]\n\
b-----------d\n\
");
		printf("\
[1] Create new phonebook \n\
[2] Read phonebook from file \n\
[any] Exit program \n\
");
		printf_s("\
      To coninue\n\
Write command's number\n\
");

		std::cin >> input;
		switch (input) {
		case 1:
			CurrentBook = new PhoneBook;
			_continue_with_book(CurrentBook);
			break;
		case 2:
			printf_s("Write down a file name\n");
			std::cin >> str_input;
			CurrentBook = new PhoneBook(str_input + ".txt");
			if (CurrentBook != nullptr)
				_continue_with_book(CurrentBook);
			break;
		default:
			return;
		}
	}

	if (CurrentBook != nullptr)	{
		delete CurrentBook;
	}
}

void _continue_with_book(PhoneBook* CurrentBook) {
	int input = 0;
	string str_input[7] = { "" };
	Recording* rec;

	while (true) {
		printf("\
[1] Print every recording \n\
[2] Add new recording \n\
[3] Find and change recording\n\
[4] Save phonebook \n\
[any] Exit phonebook \n\
");

		std::cin >> input;
		switch (input) {
		case 1:
			CurrentBook->Print();
			break;
		case 2:
			printf_s("\
Enter the following:\n\
");
			for (int i = 0; i < 4; i++) {
				cout << INFO_create_recording[i] << ":\t";
				std::cin >> str_input[i];
			}
			rec = new Recording(str_input[0], str_input[1], str_input[2], str_input[3]);
			CurrentBook->AddRecording(*rec);
			break;
		case 3:
			printf_s("\
Write down fields of searched recording:\n\
(you can skip fields by entering '_')\n\
");
			for (int i = 0; i < 7; i++) {
				cout << INFO_search_recording[i] << ":\t";
				std::cin >> str_input[i];
			}
			rec = CurrentBook->GetRecording(
				str_input[0],
				str_input[1],
				str_input[2],
				str_input[3],
				str_input[4],
				str_input[5],
				str_input[6]
			);
			_continue_with_recording(rec);
			break;
		case 4:
			printf_s("Write down a file name\n");
			std::cin >> str_input[0];
			CurrentBook->Rewrite(str_input[0] + ".txt");
			break;
		default:
			return;
		}
	}
}

void _continue_with_recording(Recording* current_recording) {
	if (current_recording == nullptr)
		return;

	int input = 0;
	string str_input = "";

	while (true) {
		current_recording->Print();
		printf("\
Choose what to change: \n\
[1] Last name\n\
[2] First name\n\
[3] Middle name\n\
[4] Address\n\
[5] Date of birth\n\
[6] Email\n\
[7] Add phone number\n\
[8] Remove phone number\n\
[any] Finish changing recording \n\
");
		std::cin >> input;
		if (input <= 0 or input > 8)
			return;
		std::cin >> str_input;
		switch (input) {
		case 1:
			current_recording->SetLastName(str_input);
			break;
		case 2:
			current_recording->SetFirstName(str_input);
			break;
		case 3:
			current_recording->SetMiddleName(str_input);
			break;
		case 4:
			current_recording->SetAddress(str_input);
			break;
		case 5:
			current_recording->SetDateOfBirth(str_input);
			break;
		case 6:
			current_recording->SetEmail(str_input);
		case 7:
			current_recording->AddPhoneNumber(str_input);
			break;
		case 8:
			current_recording->RemovePhoneNumber(str_input);
			break;
		default:
			return;
		}
	}
}