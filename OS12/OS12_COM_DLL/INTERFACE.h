#pragma once
#include <objbase.h>

#define FNAME L"KVS.CA.COM"
#define VINDX L"KVS.CA.1"
#define PRGID L"KVS.CA"


// {5B20DF23-BFDA-4084-84A3-8ED6FCE178D8} идентификатор класса 
static const GUID CLSID_CA =
{ 0x5b20df23, 0xbfda, 0x4084, { 0x84, 0xa3, 0x8e, 0xd6, 0xfc, 0xe1, 0x78, 0xd8 } };

// {5FB747AD-1CBC-4CC4-B915-19AB5316E387} идентификатор интерфейса
static const GUID IID_IAdder =
{ 0x5fb747ad, 0x1cbc, 0x4cc4, { 0xb9, 0x15, 0x19, 0xab, 0x53, 0x16, 0xe3, 0x87 } };


interface IAdder :IUnknown {
    STDMETHOD(Add(const double, const double, double*)) PURE;
    STDMETHOD(Sub(const double, const double, double*)) PURE;
};

// {E97BC590-0246-47C8-AF94-7FDFFA1AB6CD} идентификатор интерфейса
static const GUID IID_IMultiplier =
{ 0xe97bc590, 0x246, 0x47c8, { 0xaf, 0x94, 0x7f, 0xdf, 0xfa, 0x1a, 0xb6, 0xcd } };


interface IMultiplier :IUnknown {
    STDMETHOD(Mul(const double, const double, double*))  PURE;
    STDMETHOD(Div(const double, const double, double*)) PURE;
};


