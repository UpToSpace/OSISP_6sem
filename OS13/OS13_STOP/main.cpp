#include <windows.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) // OS13_STOP ./files/os13_create.ht
{
	if (argc != 2)
	{
		throw "Введите количество аргументов: 2";
	}

	std::string filename = argv[1];
	std::wstring eventName = L"Stop" + std::wstring(filename.begin(), filename.end());

	HANDLE hStopEvent = CreateEvent(NULL,
		TRUE, // FALSE - автоматический сброс; TRUE - ручной
		FALSE,
		eventName.c_str());

	if (hStopEvent == NULL) {
		std::cout << "Failed to create event.\n";
		return 1;
	}

	SetEvent(hStopEvent);

	CloseHandle(hStopEvent);

	return 0;
}
