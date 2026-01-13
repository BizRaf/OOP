#pragma once
#include "#libPhoneBook.h"
#include <string>
#include <list>
#include <iostream>
#include <fstream>
using namespace std;

const int char_zero = static_cast<int>('0');
					//	JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC
const int max_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 30};
enum SEARCHMODE {
	FIRSTNAME,
	LASTNAME,
	MIDDLENAME,
	ADDRESS,
	DATEOFBIRTH,
	EMAIL,
	PHONENUMBERS
};

const string INFO_create_recording[] = {
	"First name",
	"Last name",
	"Email",
	"Phone number"
};
const string INFO_search_recording[] = {
	"First name",
	"Last name",
	"Middle name",
	"Address",
	"Date of birth",
	"Email",
	"Phone number" };

class Recording {
private:
	string firstName;
	string lastName;
	string middleName;
	string address;
	string dateOfBirth;
	string email;
	list<string> phoneNumbers;

	void NormalizeName(string&);

public:
	Recording();
	Recording(string firstName, string lastName, string email, string phoneNumber);
	Recording(Recording& recording);
	~Recording();

	static bool CheckName(string);
	static bool CheckDateOfBirth(string);
	static bool CheckEmail(string, string);
	static bool CheckPhoneNumber(string);

	void SetFirstName(string);
	void SetLastName(string);
	void SetMiddleName(string);
	void SetAddress(string address) { this->address = address; };
	void SetDateOfBirth(string);
	void SetEmail(string);

	string GetFirstName() { return firstName; };
	string GetLastName() { return lastName; };
	string GetMiddleName() { return middleName; };
	string GetAddress() { return address; };
	string GetDateOfBirth() { return dateOfBirth; };
	string GetEmail() { return email; };

	void AddPhoneNumber(string);
	void RemovePhoneNumber(string);
	void RemoveEveryPhoneNumber() { phoneNumbers.clear(); };
	list<string> GetPhoneNumbers() { return phoneNumbers; };
	bool DoesContainPhoneNumber(string);

	void Write(string path);

	void Print();
};

class PhoneBook {
private:
	list<Recording*> Recordings;

public:
	PhoneBook() {};
	PhoneBook(PhoneBook& book);
	PhoneBook(string path);
	~PhoneBook();

	void AddRecording(Recording& recording);
	void RemoveRecording(Recording* p_recording);
	Recording* GetRecording(SEARCHMODE, string);
	Recording* GetRecording(
		string firstName,
		string lastName,
		string middleName,
		string address,
		string dateOfBirth,
		string email,
		string phoneNumbers);
	Recording* GetRecording(int index);
	const list<Recording*> GetAllRecordings() { return Recordings; };

	void Print();
	void Rewrite(string path);
};

void Start();
void _continue_with_book(PhoneBook*);
void _continue_with_recording(Recording*);