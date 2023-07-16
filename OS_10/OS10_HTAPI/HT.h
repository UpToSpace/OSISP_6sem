#pragma once
#include <Windows.h>
#include<iomanip>
#include <iostream>

namespace HT    // HT API
{
	// API HT - ����������� ��������� ��� ������� � ��-��������� 
	//          ��-��������� ������������� ��� �������� ������ � �� � ������� ����/��������
	//          ���������������� (�����������) ������ �������������� � ������� snapshot-���������� 
	//          Create - �������  � ������� HT-��������� ��� �������������   
	//          Open   - ������� HT-��������� ��� �������������
	//          Insert - ������� ������� ������
	//          Delete - ������� ������� ������    
	//          Get    - ������  ������� ������
	//          Update - �������� ������� ������
	//          Snap   - �������� snapshot
	//          Close  - ��������� Snap � ������� HT-��������� ��� �������������
	//          GetLastError - �������� ��������� � ��������� ������


	extern "C" struct HTHANDLE    // ���� ���������� HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512]);
		int     Capacity;               // ������� ��������� � ���������� ��������� 
		int     SecSnapshotInterval;    // ������������� ���������� � ���. 
		int     MaxKeyLength;           // ������������ ����� �����
		int     MaxPayloadLength;       // ������������ ����� ������
		char    FileName[512];          // ��� ����� 
		HANDLE  File;                   // File HANDLE != 0, ���� ���� ������
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, ���� mapping ������  
		LPVOID  Addr;                   // Addr != NULL, ���� mapview ��������  
		char    LastErrorMessage[512];  // ��������� �� ��������� ������ ��� 0x00  
		time_t  Lastsnaptime;           // ���� ���������� snap'a (time())  
		HANDLE  Mutex;
		int ElementCount; // ������� ���-�� ���������
		HANDLE SnapshotThread; //����� ��� ��������
	};

	extern "C" struct Element   // ������� 
	{
		Element();
		Element(const void* key, int keylength);                                             // for Get
		Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		void* key;                 // �������� ����� 
		int             keylength;           // ������ �����
		void* payload;             // ������ 
		int             payloadlength;       // ������ ������
	};

	extern "C" HTHANDLE * Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ��������� � ���������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const wchar_t FileName[512]          // ��� ����� 
	); 	// != NULL �������� ����������  

	extern "C" HTHANDLE * Open     //  ������� HT             
	(
		const wchar_t FileName[512]//,// ��� ����� 
		//std::string& fill
	); 	// != NULL �������� ����������  

	extern "C" BOOL Snap         // ��������� Snapshot
	(
		HTHANDLE * ht           // ���������� HT (File, FileMapping)
	);


	extern "C" BOOL Close        // Snap � ������� HT  �  �������� HTHANDLE
	(
		HTHANDLE * ht           // ���������� HT (File, FileMapping)
	);	//  == TRUE �������� ����������   


	extern "C" BOOL Insert      // �������� ������� � ���������
	(
		HTHANDLE * ht,            // ���������� HT
		Element * el              // �������
	);	//  == TRUE �������� ���������� 


	extern "C" BOOL Delete      // ������� ������� � ���������
	(
		HTHANDLE * ht,            // ���������� HT (����)
		Element * el              // ������� 
	);	//  == TRUE �������� ���������� 

	extern "C" Element * Get     //  ������ ������� � ���������
	(
		HTHANDLE * ht,            // ���������� HT
		Element * el              // ������� 
	); 	//  != NULL �������� ���������� 


	extern "C" BOOL Update     //  ������� ������� � ���������
	(
		HTHANDLE * ht,            // ���������� HT
		Element * oldelement,          // ������ ������� (����, ������ �����)
		const void* newpayload,          // ����� ������  
		int             newpayloadlength     // ������ ����� ������
	); 	//  != NULL �������� ���������� 

	extern "C" char* GetLastError  // �������� ��������� � ��������� ������
	(
		HTHANDLE * ht                         // ���������� HT
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

	void Print                               // ����������� ������� 
	(
		const Element* el              // ������� 
	);

};
