#include "HT.h"

using namespace std;

namespace HT
{
	HTHANDLE::HTHANDLE() { }

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512])
	{
		this->Capacity = Capacity;
		this->SecSnapshotInterval = SecSnapshotInterval;
		this->MaxKeyLength = MaxKeyLength;
		this->MaxPayloadLength = MaxPayloadLength;
		this->ElementCount = 0;
		memcpy(this->FileName, FileName, sizeof(this->FileName));
	}


	Element::Element()
	{
		this->key = NULL;
		this->keylength = NULL;
		this->payload = NULL;
		this->payloadlength = NULL;
	}

	Element::Element(const void* key, int keylength)
	{
		Element();
		this->keylength = keylength;
		this->key = (void*)key;
	}

	Element::Element(const void* key, int keylength, const void* payload, int  payloadlength)
	{
		this->key = (void*)key;
		this->keylength = keylength;
		this->payload = (void*)payload;
		this->payloadlength = payloadlength;
	}

	Element::Element(Element* oldelement, const void* newpayload, int newpayloadlength)
	{
		this->key = oldelement->key;
		this->keylength = oldelement->keylength;
		this->payload = (void*)newpayload;
		this->payloadlength = newpayloadlength;
	}

	HTHANDLE* Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const wchar_t FileName[512]          // имя файла 
	) 	// != NULL успешное завершение  
	{
		HTHANDLE* ht = NULL;

		HANDLE hFile = NULL;
		HANDLE hMap = NULL;
		HANDLE hMutex = NULL;
		LPVOID lpFileMap = NULL;
		HANDLE hSnaphot = NULL;
		DWORD SnapshotThread = NULL;

		hFile = CreateFile((LPCWSTR)FileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile)
		{
			throw "create file error";
		}

		long size = sizeof(HTHANDLE) + (MaxKeyLength + MaxPayloadLength) * Capacity;

		hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, size, NULL);
		if (!hMap)
		{
			throw "create file mapping error";
		}

		lpFileMap = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (!lpFileMap)
		{
			throw "create file mapping view error";
		}

		ZeroMemory(lpFileMap, size);

		hMutex = CreateMutex(NULL, 0, FileName);
		if (!hMutex)
		{
			throw "create mutex error";
		}

		hSnaphot = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SnapshotRoutine, ht, 0, &SnapshotThread);
		if (!hSnaphot)
		{
			return NULL;
		}

		ht = new(lpFileMap) HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		ht->File = hFile;
		ht->FileMapping = hMap;
		ht->Addr = lpFileMap;
		ht->Mutex = hMutex;
		ht->SnapshotThread;

		return ht;
	}


	HTHANDLE* Open     //  открыть HT           
	(
		const wchar_t FileName[512]         // имя файла 
	) 	// != NULL успешное завершение 
	{
		HANDLE hFile = NULL;
		HANDLE hMap = NULL;
		HANDLE hMutex = NULL;
		LPVOID lpFileMap = NULL;
		HANDLE hSnaphot = NULL;
		DWORD SnapshotThread = NULL;

		hFile = CreateFile((LPCWSTR)FileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile)
		{
			throw "create file error";
		}

		hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
		if (!hMap)
		{
			throw "create file mapping error";
		}

		lpFileMap = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (!lpFileMap)
		{
			throw "create file mapping view error";
		}

		hMutex = CreateMutex(NULL, 0, (LPCWSTR)FileName);
		if (!hMutex)
		{
			throw "create mutex error";
		}

		HTHANDLE* ht = (HTHANDLE*)lpFileMap;

		hSnaphot = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SnapshotRoutine, ht, 0, &SnapshotThread);
		if (!hSnaphot)
		{
			return NULL;
		}

		ht->File = hFile;
		ht->FileMapping = hMap;
		ht->Addr = lpFileMap;
		ht->Mutex = hMutex;
		ht->SnapshotThread = hSnaphot;

		return ht;
	}

	BOOL Snap         // выполнить Snapshot 
	(
		HTHANDLE* ht           // управление HT (File, FileMapping)
	)
	{
		WaitForSingleObject(ht->Mutex, INFINITE);
		if (!FlushViewOfFile(ht->Addr, NULL))
		{
			ReleaseMutex(ht->Mutex);
			SetLastError(ht, "cannot flush view to disk");
			return FALSE;
		}
		ht->Lastsnaptime = time(NULL);
		ReleaseMutex(ht->Mutex);
		return TRUE;
	}


	BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
	(
		HTHANDLE* ht         // управление HT (File, FileMapping)
	)
	{
		Snap(ht);
		HANDLE hMap, hFile, hMutex;

		memcpy(&hMap, &ht->FileMapping, sizeof(HANDLE));
		memcpy(&hFile, &ht->File, sizeof(HANDLE));
		memcpy(&hMutex, &ht->Mutex, sizeof(HANDLE));

		TerminateThread(ht->SnapshotThread, 0);

		if (!UnmapViewOfFile(ht->Addr) || !CloseHandle(hMap) || !CloseHandle(hFile) || !CloseHandle(hMutex))
		{
			return FALSE;
		}

		return TRUE;
	}


	BOOL Insert      // добавить элемент в хранилище 
	(
		HTHANDLE* ht,            // управление HT
		Element* el            // элемент
	)
	{
		if (ht->Capacity == ht->ElementCount)
		{
			SetLastError(ht, "Hash table is full");
			return FALSE;
		}

		if (!CheckElementParm(ht, el))
		{
			return FALSE;
		}

		bool isInserted = FALSE;

		for (int i = 0, j = HashFunction(el, ht->Capacity, 0);
			i != ht->Capacity && !isInserted;
			j = NextHash(j, ht->Capacity, ++i))
		{
			Element* elFromHT = GetElementFromHT(ht, j);
			if (elFromHT == NULL || IsDeleted(elFromHT))
			{
				SetElementToHT(ht, el, j);
				ht->ElementCount++;
				isInserted = true;
			}
			if (elFromHT != NULL && CheckEqualElementKeys(elFromHT, el)) {
				SetLastError(ht, "Key exists\n");
				ReleaseMutex(ht->Mutex);
				return FALSE;
			}
		}
		ReleaseMutex(ht->Mutex);
		return isInserted;
	}


	BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE* ht,            // управление HT (ключ)
		Element* el              // элемент 
	)	//  == TRUE успешное завершение 
	{
		if (!CheckElementParm(ht, el))
			return FALSE;
		WaitForSingleObject(ht->Mutex, INFINITE);
		int indexInHT = -1;
		bool deleted = false;
		if (ht->ElementCount != 0)
			for (int i = 0, j = HashFunction(el, ht->Capacity, 0);
				GetElementFromHT(ht, j) != NULL && i != ht->Capacity && !deleted;
				j = NextHash(j, ht->Capacity, ++i)) {
			Element* elFromHT = GetElementFromHT(ht, j);
			if (!IsDeleted(elFromHT)) {
				if (CheckEqualElementKeys(elFromHT, el)) {
					indexInHT = j;
					deleted = true;
				}
			}
		}
		if (indexInHT < 0) {
			SetLastError(ht, "Not found key\n");
			ReleaseMutex(ht->Mutex);
			return FALSE;
		}

		SetDeletedFlag(GetElementFromHT(ht, indexInHT));

		//Element* elFromHT = GetElementFromHT(ht, indexInHT);
		//memcpy(elFromHT->key, (const int*)-1, sizeof(const int));

		ht->ElementCount--;
		ReleaseMutex(ht->Mutex);
		return TRUE;
	}

	Element* Get     //  читать элемент в хранилище
	(
		HTHANDLE* ht,            // управление HT
		Element* el              // элемент 
	) 	//  != NULL успешное завершение 
	{
		if (!CheckElementParm(ht, el))
			return NULL;
		WaitForSingleObject(ht->Mutex, INFINITE);
		int indexInHT = -1;
		bool found = false;
		if (ht->ElementCount != 0)
			for (int i = 0, j = HashFunction(el, ht->Capacity, 0);
				GetElementFromHT(ht, j) != NULL && i != ht->Capacity && !found;
				j = NextHash(j, ht->Capacity, ++i))
		{
			Element* elFromHT = GetElementFromHT(ht, j);
			if (!IsDeleted(elFromHT))
			{
				if (CheckEqualElementKeys(elFromHT, el))
				{
					indexInHT = j; found = true;
				}
			}
		}
		if (indexInHT < 0) {
			SetLastError(ht, "Not found key\n");
			ReleaseMutex(ht->Mutex);
			return NULL;
		}
		ReleaseMutex(ht->Mutex);
		return GetElementFromHT(ht, indexInHT);
	}


	BOOL Update     //  именить элемент в хранилище
	(
		HTHANDLE* ht,            // управление HT
		Element* oldelement,          // старый элемент (ключ, размер ключа)
		const void* newpayload,          // новые данные  
		int             newpayloadlength     // размер новых данных
	) 	//  != NULL успешное завершение 
	{
		if (!CheckElementParm(ht, oldelement) || !CheckElementParm(ht, newpayloadlength))
			return FALSE;
		WaitForSingleObject(ht->Mutex, INFINITE);
		int indexInHT = -1;
		bool updated = false;
		if (ht->ElementCount != 0)
			for (int i = 0, j = HashFunction(oldelement, ht->Capacity, 0);
				GetElementFromHT(ht, j) != NULL && i != ht->Capacity && !updated;
				j = NextHash(j, ht->Capacity, ++i)) {
			Element* elFromHT = GetElementFromHT(ht, j);
			if (!IsDeleted(elFromHT)) {
				if (CheckEqualElementKeys(elFromHT, oldelement)) {
					indexInHT = j;
					updated = true;
				}
			}
		}
		if (indexInHT < 0) {
			SetLastError(ht, "Not found key\n");
			ReleaseMutex(ht->Mutex);
			return FALSE;
		}

		UpdateElement(ht, GetElementFromHT(ht, indexInHT), (void*)newpayload, newpayloadlength);
		ReleaseMutex(ht->Mutex);
		return TRUE;
	}

	char* GetLastError  // получить сообщение о последней ошибке
	(
		HTHANDLE* ht                         // управление HT
	)
	{
		return ht->LastErrorMessage;
	}

	void Print                               // распечатать элемент 
	(
		const Element* el              // элемент 
	)
	{
		std::cout << "Element:" << std::endl;
		std::cout << "{" << std::endl;
		std::cout << "\t\"key\": \"" << (char*)el->key << "\"," << std::endl;
		std::cout << "\t\"keyLength\": " << el->keylength << "," << std::endl;
		std::cout << "\t\"payload\": \"" << (char*)el->payload << "\"," << std::endl;
		std::cout << "\t\"payloadLength\": " << el->payloadlength << std::endl;
		std::cout << "}" << std::endl;
	}

	BOOL CheckElementParm(HTHANDLE* ht, Element* el)
	{
		if (el->keylength > ht->MaxKeyLength || el->payloadlength > ht->MaxPayloadLength)
		{
			SetLastError(ht, "el's key is too long");
			return FALSE;
		}

		if (el->payloadlength > ht->MaxPayloadLength)
		{
			SetLastError(ht, "el's payload is too long");
			return FALSE;
		}

		if (el->keylength == 0 || el->payloadlength == 0)
		{
			SetLastError(ht, "el's field is zero");
			return FALSE;
		}

		return TRUE;
	}

	BOOL CheckElementParm(HTHANDLE* ht, int payloadLength)
	{
		if (payloadLength > ht->MaxPayloadLength)
		{
			SetLastError(ht, "el's key is too long");
			return FALSE;
		}
		return TRUE;
	}

	void SetLastError(HTHANDLE* ht, const char* message)
	{
		memcpy(ht->LastErrorMessage, (char*)message, sizeof(message) + 1);
	}

	int HashFunction(const Element* el, int size, int p)
	{
		char* arrKeyBytes = new char[el->keylength];
		memcpy(arrKeyBytes, el->key, el->keylength);
		int sumBytes = 0;
		for (int i = 0; i < el->keylength; i++) {
			sumBytes += arrKeyBytes[i];
		}
		double key2 = 5 * ((0.6180339887499 * sumBytes) - int((0.6180339887499 * sumBytes)));
		delete[] arrKeyBytes;
		return (p + sumBytes) % size;
	}

	int NextHash(int hash, int size, int p)
	{
		return (hash + 5 * p + 3 * p * p) % size;
	}

	BOOL CheckEqualElementKeys(Element* el1, Element* el2) {
		int result = !memcmp(el1->key, el2->key, el2->keylength);
		return result;
	}

	Element* GetElementFromHT(HTHANDLE* ht, int hash) {
		void* elementsAddr = ht + 1;
		int maxElementSize = ht->MaxKeyLength + ht->MaxPayloadLength + 2 * sizeof(int);
		void* elementAddr = (char*)elementsAddr + maxElementSize * hash;

		Element* el = new Element();
		el->key = elementAddr;
		el->keylength = *(int*)((char*)elementAddr + ht->MaxKeyLength);
		el->payload = ((char*)elementAddr + ht->MaxKeyLength + sizeof(int));
		el->payloadlength = *(int*)((char*)elementAddr + ht->MaxKeyLength + sizeof(int) + ht->MaxPayloadLength);
		if (el->keylength == 0) {
			delete el;
			return NULL;
		}
		return el;
	}

	BOOL SetElementToHT(HTHANDLE* ht, Element* el, int n) {
		void* elementsAddr = ht + 1;
		int maxElementSize = ht->MaxKeyLength + ht->MaxPayloadLength + 2 * sizeof(int);
		void* elementAddr = (char*)elementsAddr + maxElementSize * n;

		memcpy(elementAddr, el->key, el->keylength);
		memcpy(((char*)elementAddr + ht->MaxKeyLength), &el->keylength, sizeof(int));
		memcpy(((char*)elementAddr + ht->MaxKeyLength + sizeof(int)), el->payload, el->payloadlength);
		memcpy(((char*)elementAddr + ht->MaxKeyLength + sizeof(int) + +ht->MaxPayloadLength), &el->payloadlength, sizeof(int));
		return TRUE;
	}

	DWORD WINAPI SnapshotRoutine(HTHANDLE* ht) {
		while (true)
		{
			if (ht) {
				if (time(NULL) >= ht->Lastsnaptime + ht->SecSnapshotInterval)
				{
					WaitForSingleObject(ht->Mutex, INFINITE);
					if (!FlushViewOfFile(ht->Addr, NULL)) {
						SetLastError(ht, "Snapshot error");
						return FALSE;
					}
					ht->Lastsnaptime = time(NULL);
					cout << "----SNAPSHOT in Thread----" << endl;

					ReleaseMutex(ht->Mutex);
				}
			}
			else
				break;
		}
		return TRUE;
	}

	BOOL IsDeleted(Element* el) {
		if (*(int*)el->key == -1) {
			return TRUE;
		}
		return FALSE;
	}

	void UpdateElement(HTHANDLE* ht, Element* el, const void* newpayload, int newpayloadlength) {
		ZeroMemory(el->payload, ht->MaxPayloadLength + sizeof(int));
		memcpy(el->payload, newpayload, newpayloadlength);
		memcpy((char*)el->payload + ht->MaxPayloadLength, &newpayloadlength, sizeof(int));
	}

	void SetDeletedFlag(Element* el) {
		memcpy(el->key, &DELETED, sizeof(DELETED));
	}
}