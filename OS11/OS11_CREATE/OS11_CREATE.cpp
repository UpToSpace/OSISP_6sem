// OS11_CREATE.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "HT.h"
#include <string>
#pragma comment(lib, "OS11_HTAPI")

using namespace std;
using namespace HT;

bool checkInt(char* input);

int main(int argc, char* argv[]) // OS11_CREATE ./files/os11_create.ht 10 5 20 10              OS11_CREATE ./files/os11_04.ht 2000 3 4 4
{ 
	setlocale(LC_ALL, "Russian");

	try
	{
		if (argc != 6)
		{
			throw "Введите количество аргументов: 6";
		}
		if (!checkInt(argv[2]) || !checkInt(argv[3]) || !checkInt(argv[4]) || !checkInt(argv[5]))
		{
			throw "Введите корректные значения";
		}
		const size_t cSize = strlen(argv[1]) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, argv[1], cSize);

		if (GetFileAttributes(wc) != INVALID_FILE_ATTRIBUTES)
		{
			throw "Файл уже существует";
		}

		int capacity = stoi(argv[2]);
		int snapshotinterval = stoi(argv[3]);
		int maxKeyLength = stoi(argv[4]);
		int maxPayloadLength = stoi(argv[5]);

		HTHANDLE* HT = Create(capacity, snapshotinterval, maxKeyLength, maxPayloadLength, wc);
		if (HT == NULL)
		{
			throw "Хранилище не создано";
		}

		cout << "HT-Storage Created!" << endl;
		wcout << "filename = " << wc << endl;
		cout << "snapshotinterval = " << snapshotinterval << endl;
		cout << "capacity = " << capacity << endl;
		cout << "maxkeylength = " << maxKeyLength << endl;
		cout << "maxdatalength = " << maxPayloadLength << endl;

		Close(HT);
	}
	catch (const char* err)
	{
		cout << err << endl;
		exit(-1);
	}

	exit(0);
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