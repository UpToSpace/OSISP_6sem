#include "HT.h"
#include <iostream>
#include <fstream>
#include <string>
#pragma warning (disable : 4996)

void PrintError(char* str);

int main()
{

	HT::HTHANDLE* HT = NULL;
	HT::HTHANDLE* HT2 = NULL;

	try {
		std::ifstream inputFile("../params/params1.txt");
		int capacity, snapshotIntervalSec, maxKeyLength, maxPayloadLength;
		char fileName[256], fileName2[256];

		if (inputFile.is_open()) {
			inputFile >> capacity >> snapshotIntervalSec >> maxKeyLength >> maxPayloadLength >> fileName;
			inputFile.close();
		}
		const size_t cSize = strlen(fileName) + 1;
		wchar_t* lFileName = new wchar_t[cSize];
		mbstowcs(lFileName, fileName, cSize);

		HT = HT::Create(capacity, snapshotIntervalSec, maxKeyLength, maxPayloadLength, lFileName); // создать НТ-хранилище
		if (HT) std::cout << "\tCreate HT: success\t\n";
		else throw "Create: error";

		std::ifstream inputFile2("../params/params2.txt");
		if (inputFile2.is_open()) {
			inputFile2 >> capacity >> snapshotIntervalSec >> maxKeyLength >> maxPayloadLength >> fileName2;
			inputFile2.close();
		}
		const size_t cSize2 = sizeof(fileName2) + 1;
		wchar_t* lFileName2 = new wchar_t[cSize2];
		mbstowcs(lFileName2, fileName2, cSize2);

		HT2 = HT::Create(capacity, snapshotIntervalSec, maxKeyLength, maxPayloadLength, lFileName2); // создать НТ-хранилище
		if (HT2) std::cout << "\tCreate HT2: success\t\n";
		else throw "Create: error";

		HT::Element* el1 = new HT::Element("s1", 3, "p1", 3);
		HT::Element* el2 = new HT::Element("st2", 4, "pa2", 4);
		HT::Element* el3 = new HT::Element("str3", 5, "pay3", 5);
		HT::Element* el4 = new HT::Element("stri4", 6, "payl4", 6);

		if (!HT::Insert(HT, el1) || !HT::Insert(HT, el2)) {
			PrintError(HT::GetLastError(HT));
		}

		if (!HT::Insert(HT2, el3) || !HT::Insert(HT2, el4)) {
			PrintError(HT::GetLastError(HT2));
		}

		HT::Element* el;
		std::cout << "\n\tget after insert\t\n" << std::endl;
		std::cout << "\n\tHT\t\n" << std::endl;
		if ((el = HT::Get(HT, el1)) == NULL) PrintError(HT::GetLastError(HT));
		else HT::Print(el);
		if ((el = HT::Get(HT, el2)) == NULL) PrintError(HT::GetLastError(HT));
		else HT::Print(el);
		std::cout << "\n\tHT2\t\n" << std::endl;
		if ((el = HT::Get(HT2, el3)) == NULL) PrintError(HT::GetLastError(HT2));
		else HT::Print(el);
		if ((el = HT::Get(HT2, el4)) == NULL) PrintError(HT::GetLastError(HT2));
		else HT::Print(el);

		if (!HT::Update(HT, el1, el2->payload, el2->payloadlength)) {
			PrintError(HT::GetLastError(HT));
		}
		if (!HT::Update(HT2, el3, el4->payload, el4->payloadlength)) {
			PrintError(HT::GetLastError(HT2));
		}

		if (!HT::Snap(HT))
			PrintError(HT::GetLastError(HT));
		if (!HT::Snap(HT2))
			PrintError(HT::GetLastError(HT2));

		std::cout << "\n\tget after update\t\n" << std::endl;
		std::cout << "\n\tHT\t\n" << std::endl;
		if ((el = HT::Get(HT, el1)) == NULL) PrintError(HT::GetLastError(HT));
		else HT::Print(el);
		if ((el = HT::Get(HT, el2)) == NULL) PrintError(HT::GetLastError(HT));
		else HT::Print(el);
		std::cout << "\n\tHT2\t\n" << std::endl;
		if ((el = HT::Get(HT2, el3)) == NULL) PrintError(HT::GetLastError(HT2));
		else HT::Print(el);
		if ((el = HT::Get(HT2, el4)) == NULL) PrintError(HT::GetLastError(HT2));
		else HT::Print(el);

		if (!HT::Delete(HT, el2)) {
			PrintError(HT::GetLastError(HT));
		}
		if (!HT::Delete(HT2, el4)) {
			PrintError(HT::GetLastError(HT2));
		}

		std::cout << "\n\tget after delete\t\n" << std::endl;
		std::cout << "\n\tHT\t\n" << std::endl;
		if ((el = HT::Get(HT, el1)) == NULL) PrintError(HT::GetLastError(HT));
		else HT::Print(el);
		std::cout << "\n\tHT2\t\n" << std::endl;
		if ((el = HT::Get(HT2, el3)) == NULL) PrintError(HT::GetLastError(HT2));
		else HT::Print(el);

		std::cout << "\n\tclose HT...\t\n" << std::endl;
		HT::Close(HT);
		std::cout << "\n\tclose HT2...\t\n" << std::endl;
		HT::Close(HT2);

		std::cout << "\n\topen HT...\t\n" << std::endl;
		std::cout << "\n\topen HT2...\t\n" << std::endl;

		HT::HTHANDLE* HT1 = HT::Open(lFileName);
		std::cout << "\n\tget after close and open HT\t\n" << std::endl;
		if ((el = HT::Get(HT1, el1)) == NULL) PrintError(HT::GetLastError(HT1));
		else HT::Print(el);

		std::cout << "\n\tget after close and open HT2\t\n" << std::endl;
		HT::HTHANDLE* HT3 = HT::Open(lFileName2);
		if ((el = HT::Get(HT3, el3)) == NULL) PrintError(HT::GetLastError(HT3));
		else HT::Print(el);

		std::cout << "\n\tclose HT...\t\n" << std::endl;
		HT::Close(HT1);
		std::cout << "\n\tclose HT2...\t\n" << std::endl;
		HT::Close(HT3);
	}
	catch (const char* msg)
	{
		std::cout << msg << "\n";
		if (HT != NULL) std::cout << HT::GetLastError(HT);
	}

	system("pause");
	return 0;
}
void PrintError(char* str) {
	std::cout << "error: \t" << str << std::endl;
}