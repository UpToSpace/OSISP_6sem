#pragma warning(disable : 4996)

#include <iostream>
#include <windows.h>

#include "../OS13_HTCOM_LIB/pch.h"
#include "../OS13_HTCOM_LIB/OS13_HTCOM_LIB.h"

#ifdef _WIN64
#pragma comment(lib, "../x64/Debug/OS13_HTCOM_LIB.lib")
#else
#pragma comment(lib, "../Debug/OS13_HTCOM_LIB.lib")
#endif

using namespace std;

wchar_t* getWC(const char* c);
bool checkInt(char* input);

int main(int argc, char* argv[]) // OS13_CREATE 1000 5 20 10 ./files/os13_create.ht 
{
	setlocale(LC_ALL, "Russian");

	try
	{
		if (argc != 6)
		{
			throw "Введите количество аргументов: 6";
		}
		if (!checkInt(argv[2]) || !checkInt(argv[3]) || !checkInt(argv[4]) || !checkInt(argv[1]))
		{
			throw "Введите корректные значения";
		}

		OS13_HTCOM_HANDEL h = OS13_HTCOM::Init();

		ht::HtHandle* ht = OS13_HTCOM::HT::create(h, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), getWC(argv[5]));

		if (ht)
		{
			cout << "HT-Storage Created" << endl;
			wcout << "filename: " << ht->fileName << endl;
			cout << "secSnapshotInterval: " << ht->secSnapshotInterval << endl;
			cout << "capacity: " << ht->capacity << endl;
			cout << "maxKeyLength: " << ht->maxKeyLength << endl;
			cout << "maxPayloadLength: " << ht->maxPayloadLength << endl;

			OS13_HTCOM::HT::close(h, ht);
		}
		else
			cout << "-- create: error" << endl;

		OS13_HTCOM::Dispose(h);
	}
	catch (const char* e) { cout << e << endl; }
	catch (int e) { cout << "HRESULT error: " << e << endl; }

}

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}

bool checkInt(char* input)
{
	int i = 0;
	while (input[i] != '\0')
	{
		if (input[i] < '0' || input[i] > '9')
			return false;
		i++;
	}
	return true;
}
