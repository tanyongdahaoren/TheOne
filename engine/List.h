#pragma once

#include "list"
using namespace std;

template<class T>
class List
{
public:
	// ------------------------------------------
	// Iterators
	// ------------------------------------------
	typedef typename std::list<T>::iterator iterator;
	typedef typename std::list<T>::const_iterator const_iterator;
	typedef typename std::list<T>::reverse_iterator reverse_iterator;
	typedef typename std::list<T>::const_reverse_iterator const_reverse_iterator;

	iterator begin() {return _data.begin();}

	const_iterator begin() const {return _data.begin();}

	iterator end() {return _data.end();}

	const_iterator end() const {return _data.end();}

    reverse_iterator rbegin() { return _data.rbegin(); }

    const_reverse_iterator rbegin() const { return _data.rbegin(); }
    
    reverse_iterator rend() { return _data.rend(); }

    const_reverse_iterator rend() const { return _data.rend(); }

	T front() const
	{
		return _data.front();
	}

	T back() const
	{
		return _data.back();
	}

	iterator insert(iterator it, T object)
	{
		object->Retain();
		return _data.insert(it, object);
	}

	iterator push_front(T object)
	{
		return insert(_data.begin(), object);
	}

	iterator push_back(T object)
	{
		return insert(_data.end(), object);
	}

	void popFront()
	{
		T front = _data.front();
		_data.pop_front();
		front->Release();
	}

	void popBack()
	{
		T last = _data.back();
		_data.pop_back();
		last->Release();
	}

	iterator erase(iterator it)
	{
		(*it)->Release();
		return _data.erase(it);
	}

	void clear()
	{
		for(iterator it = _data.begin(); it!=_data.end(); ++it)
		{
			(*it)->Release();
		}
		_data.clear();
	}
public:
	List()
		: _data()
	{
		static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for Vector<T>!");
	}

	~List<T>()
	{
		clear();
	}

	List<T>(const List<T>& other)
	{
		static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for Vector<T>!");
		_data = other._data;
		addRefForAllObjects();
	}

	List<T>& operator=(const List<T>& other)
	{
		if (this != &other) {
			clear();
			_data = other._data;
			addRefForAllObjects();
		}
		return *this;
	}
protected:
	void addRefForAllObjects()
	{
		for(const auto &obj : _data)
		{
			obj->Retain();
		}
	}
	list<T> _data;
};