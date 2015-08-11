#include "Data.h"
#include <iostream>

const Data Data::Null;

Data::Data() :
_bytes(nullptr),
_size(0)
{
}

Data::Data(Data&& other) :
_bytes(nullptr),
_size(0)
{
    Move(other);
}

Data::Data(const Data& other) :
_bytes(nullptr),
_size(0)
{
    Copy(other._bytes, other._size);
}

Data::~Data()
{
    clear();
}

Data& Data::operator= (const Data& other)
{
    Copy(other._bytes, other._size);
    return *this;
}

Data& Data::operator= (Data&& other)
{
    Move(other);
    return *this;
}

void Data::Move(Data& other)
{
    _bytes = other._bytes;
    _size = other._size;
    
    other._bytes = nullptr;
    other._size = 0;
}

bool Data::IsNull() const
{
    return (_bytes == nullptr || _size == 0);
}

unsigned char* Data::GetBytes() const
{
    return _bytes;
}

ssize_t Data::GetSize() const
{
    return _size;
}

void Data::Copy(const unsigned char* bytes, const ssize_t size)
{
    clear();
    
    if (size > 0)
    {
        _size = size;
        _bytes = (unsigned char*)malloc(sizeof(unsigned char) * _size);
        memcpy(_bytes, bytes, _size);
    }
}

void Data::FastSet(unsigned char* bytes, const ssize_t size)
{
    _bytes = bytes;
    _size = size;
}

void Data::clear()
{
    free(_bytes);
    _bytes = nullptr;
    _size = 0;
}