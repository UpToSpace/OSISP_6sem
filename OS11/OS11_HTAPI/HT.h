#pragma once
#include <Windows.h>
#include <iomanip>
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

#ifdef OS11HTAPI_EXPORTS
#define OS11HTAPI __declspec(dllexport)
#else
#define OS11HTAPI __declspec(dllimport)
#endif

namespace HT    // HT API
{
	// API HT - программный интерфейс для доступа к НТ-хранилищу 
	//          НТ-хранилище предназначено для хранения данных в ОП в формате ключ/значение
	//          Персистестеность (сохранность) данных обеспечивается с помощью snapshot-менханизма 
	//          Create - создать  и открыть HT-хранилище для использования   
	//          Open   - открыть HT-хранилище для использования
	//          Insert - создать элемент данных
	//          Delete - удалить элемент данных    
	//          Get    - читать  элемент данных
	//          Update - изменить элемент данных
	//          Snap   - выпонить snapshot
	//          Close  - выполнить Snap и закрыть HT-хранилище для использования
	//          GetLastError - получить сообщение о последней ошибке

	extern "C" OS11HTAPI struct HTHANDLE    // блок управления HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512]);
		int     Capacity;               // емкость хранилища в количестве элементов 
		int     SecSnapshotInterval;    // переодичность сохранения в сек. 
		int     MaxKeyLength;           // максимальная длина ключа
		int     MaxPayloadLength;       // максимальная длина данных
		char    FileName[512];          // имя файла 
		HANDLE  File;                   // File HANDLE != 0, если файл открыт
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
		LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
		char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
		time_t  Lastsnaptime;           // дата последнего snap'a (time())  
		HANDLE  Mutex;
		int ElementCount; // текущее кол-во элементов
		HANDLE SnapshotThread; //поток для снепшота
	};

	extern "C" OS11HTAPI struct Element   // элемент 
	{
		OS11HTAPI Element();
		OS11HTAPI Element(const void* key, int keylength);                                             // for Get
		OS11HTAPI Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		OS11HTAPI Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		void* key;                 // значение ключа 
		int             keylength;           // рахмер ключа
		void* payload;             // данные 
		int             payloadlength;       // размер данных
	};
	extern "C" OS11HTAPI HTHANDLE * OpenExist     //  открыть HT             
	(
		const wchar_t    FileName[512]         // имя файла 
	); 	// != NULL успешное завершение  

	extern "C" OS11HTAPI HTHANDLE * Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища в количестве элементов
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const wchar_t FileName[512]          // имя файла 
	); 	// != NULL успешное завершение  

	extern "C" OS11HTAPI HTHANDLE * Open     //  открыть HT             
	(
		const wchar_t FileName[512]//,// имя файла 
		//std::string& fill
	); 	// != NULL успешное завершение  
	extern "C" OS11HTAPI HTHANDLE * OpenExist(const wchar_t FileName[512]);

	extern "C" OS11HTAPI BOOL Snap         // выполнить Snapshot
	(
		HTHANDLE * ht           // управление HT (File, FileMapping)
	);


	extern "C" OS11HTAPI BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
	(
		HTHANDLE * ht           // управление HT (File, FileMapping)
	);	//  == TRUE успешное завершение   


	extern "C" OS11HTAPI BOOL Insert      // добавить элемент в хранилище
	(
		HTHANDLE * ht,            // управление HT
		Element * el              // элемент
	);	//  == TRUE успешное завершение 


	extern "C" OS11HTAPI BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE * ht,            // управление HT (ключ)
		Element * el              // элемент 
	);	//  == TRUE успешное завершение 

	extern "C" OS11HTAPI Element * Get     //  читать элемент в хранилище
	(
		HTHANDLE * ht,            // управление HT
		Element * el              // элемент 
	); 	//  != NULL успешное завершение 


	extern "C" OS11HTAPI BOOL Update     //  именить элемент в хранилище
	(
		HTHANDLE * ht,            // управление HT
		Element * oldelement,          // старый элемент (ключ, размер ключа)
		const void* newpayload,          // новые данные  
		int             newpayloadlength     // размер новых данных
	); 	//  != NULL успешное завершение 

	char* GetLastError  // получить сообщение о последней ошибке
	(
		HTHANDLE* ht                         // управление HT
	);

	const int DELETED = -1;

	void SetDeletedFlag(Element* el);

	void SetLastError(HTHANDLE* ht, const char* message); //

	BOOL CheckElementParm(HTHANDLE* ht, Element* el);

	BOOL CheckElementParm(HTHANDLE* ht, int payloadLength);

	BOOL CheckEqualElementKeys(Element* el1, Element* el2);

	int HashFunction(const Element* el, int size, int p);

	int NextHash(int hash, int size, int p);

	Element* GetElementFromHT(HTHANDLE* ht, int hash);

	BOOL SetElementToHT(HTHANDLE* ht, Element* el, int n);

	DWORD WINAPI SnapshotRoutine(HTHANDLE* ht);

	BOOL IsDeleted(Element* el);

	void UpdateElement(HTHANDLE* ht, Element* el, const void* newpayload, int newpayloadlength);

	extern "C" OS11HTAPI void Print                               // распечатать элемент 
	(
		const Element * el              // элемент 
	);
};
