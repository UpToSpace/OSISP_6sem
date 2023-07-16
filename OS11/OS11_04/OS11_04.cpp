#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "HT.h"
#include <string>
#pragma comment(lib, "OS11_HTAPI")

using namespace std;
using namespace HT;
void PrintError(char* str);

int main(int argc, char* argv[]) // D:\University\осисп\labs\OS11\x64\Debug>OS11_04 ./files/os11_create.ht    OS11_04 ./files/os11_04.ht
{
	setlocale(LC_ALL, "Russian");
	srand(time(nullptr));

	try
	{
		if (argc != 2)
		{
			throw "Введите количество аргументов: 2";
		}

		const size_t cSize = strlen(argv[1]) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, argv[1], cSize);

		HTHANDLE* HT = OpenExist(wc);
		if (HT == NULL)
		{
			throw "Хранилище не создано";
		}

		string key;

		while (true)
		{
			Sleep(1000);

			key = to_string(rand() % 50);
			cout << "key: " << key << endl;

			HT::Element* elFromHT = Get(HT, new Element(key.c_str(), (int)key.length() + 1));
			if (!elFromHT)
			{
				PrintError(HT->LastErrorMessage);
				continue;
			}
			Print(elFromHT);
			int newpayload = atoi((char *)elFromHT->payload) + 1;

			if (!Update(HT, elFromHT, to_string(newpayload).c_str(), elFromHT->payloadlength))
			{
				PrintError(HT->LastErrorMessage);
				continue;
			}
			Print(Get(HT, new Element(key.c_str(), (int)key.length() + 1)));
			cout << "Elements amount: " << HT->ElementCount << endl;
		}

		Close(HT);
	}
	catch (const char* err)
	{
		cout << err << endl;
		exit(-1);
	}

	exit(0);
}

void PrintError(char* str) {
	std::cout << "error: \t" << str << std::endl;
}