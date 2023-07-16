#include "pch.h"
#include "CppUnitTest.h"
#include "HT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HT;

namespace TestCaseHTAPI
{
	TEST_CLASS(TestCaseHTAPI)
	{
	public:
		
		TEST_METHOD(TestInsertSameKey) {
			HTHANDLE* ht = Create(4, 2, 10, 10, L"../tests/OS10_04_TestInsertSameKey.ht");
			Insert(ht, new Element("key1", 4, "value1", 6));
			Insert(ht, new Element("key1", 4, "value2", 6));
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("Key exists\n"));
			Close(ht);
		}

		TEST_METHOD(TestInsertAndGetWithCollision) {
			HTHANDLE* ht = Create(5, 2, 10, 10, L"../tests/OS10_04_TestInsertAndGetWithCollision.ht");
			Element* elem1 = new Element("key1", 4, "value1", 6);
			Element* elem2 = new Element("key6", 4, "value2", 6);
			Element* elem3 = new Element("key11", 5, "value3", 6);
			Element* elem4 = new Element("key16", 5, "value4", 6);
			bool inserted = Insert(ht, elem1);
			Assert::IsTrue(inserted);
			inserted = Insert(ht, elem2);
			Assert::IsTrue(inserted);
			inserted = Insert(ht, elem3);
			Assert::IsTrue(inserted);
			inserted = Insert(ht, elem4);
			Assert::IsTrue(inserted);

			Element* retrieved = Get(ht, elem1);
			Assert::AreEqual(std::string((char*)retrieved->payload), std::string("value1"));

			retrieved = Get(ht, elem2);
			Assert::AreEqual(std::string((char*)retrieved->payload), std::string("value2"));

			retrieved = Get(ht, elem3);
			Assert::AreEqual(std::string((char*)retrieved->payload), std::string("value3"));

			retrieved = Get(ht, elem4);
			Assert::AreEqual(std::string((char*)retrieved->payload), std::string("value4"));

			Close(ht);
		}

		TEST_METHOD(TestRemoveNonExistent) {
			HTHANDLE* ht = Create(4, 2, 10, 10, L"../tests/OS10_04_RemoveNonExistent.ht");
			Element* elem = new Element("key", 3, "value", 5);
			Delete(ht, elem);
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("Not found key\n"));
			Close(ht);
		}

		TEST_METHOD(TestMaxKeyLength) {
			HTHANDLE* ht = Create(4, 2, 10, 10, L"../tests/OS10_04_MaxKeyLength.ht");
			Element* elem = new Element("verylongkeythatexceedsthemaximumlengthallowed", 46, "value", 6);
			Insert(ht, elem);
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("el's key is too long"));
			Close(ht);
		}

		TEST_METHOD(TestMaxPayloadLength) {
			HTHANDLE* ht = Create(4, 2, 10, 20, L"../tests/OS10_04_MaxPayloadLength.ht");
			Element* elem = new Element("key", 3, "verylongpayloadthatexceedsthemaximumlengthallowed", 46);
			Insert(ht, elem);
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("el's key is too long"));
			Close(ht);
		}

		TEST_METHOD(TestInsertKeyZeroLength) {
			HTHANDLE* ht = Create(4, 2, 10, 20, L"../tests/OS10_04_TestInsertKeyZeroLength.ht");
			Element* elem = new Element("", 3, "value", 0);
			Insert(ht, elem);
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("el's field is zero"));
			Close(ht);
		}

		TEST_METHOD(TestInsertPayloadZeroLength) {
			HTHANDLE* ht = Create(4, 2, 10, 20, L"../tests/OS10_04_TestInsertPayloadZeroLength.ht");
			Element* elem = new Element("key", 3, "", 0);
			Insert(ht, elem);
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("el's field is zero"));
			Close(ht);
		}

		TEST_METHOD(TestOpenManyHT) {
			try {
				HTHANDLE* newht = NULL;
				HTHANDLE* newht2 = NULL;
				HTHANDLE* ht = Create(4, 2, 10, 20, L"../tests/OS10_04_TestOpenManyHT.ht");
				newht = Open(L"../tests/OS10_04_TestOpenManyHT.ht");
				newht2 = Open(L"../tests/OS10_04_TestOpenManyHT.ht");
				Close(ht);
			}
			catch (const char* message)
			{
				Assert::AreEqual(message, "create file mapping error");
			}
		}

		TEST_METHOD(TestInsertMoreThanCapacity) {
			HTHANDLE* newht = NULL;
			HTHANDLE* newht2 = NULL;
			HTHANDLE* ht = Create(1, 2, 10, 20, L"../tests/OS10_04_TestInsertMoreThanCapacity.ht");
			Element* elem1 = new Element("key1", 4, "value1", 6);
			Element* elem2 = new Element("key5", 4, "value2", 6);
			Insert(ht, elem1);
			Insert(ht, elem2);
			Assert::AreEqual(std::string(ht->LastErrorMessage), std::string("Hash table is full"));
			Close(ht);
		}
	};
}
