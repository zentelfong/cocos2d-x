#ifndef __CC_WEAK_PTR_H__
#define __CC_WEAK_PTR_H__
/// @cond DO_NOT_SHOW

#include "base/CCRef.h"
#include "base/ccMacros.h"


NS_CC_BEGIN

#define CC_WEAK_PTR_SAFE_RETAIN(ref)\
    do\
    {\
        if (ref)\
        {\
            ref->incWeakRef();\
        }\
    \
    }   while (0);


#define CC_WEAK_PTR_SAFE_RELEASE(ref)\
    do\
    {\
		if (ref && ref->decWeakRef() == 0) { \
			delete ref; \
			ref = nullptr; \
		}\
    \
    }   while (0);


template <typename T> class WeakPtr
{
public:

	WeakPtr()
		: _ptr(nullptr), _ref(nullptr)
	{

	}

	WeakPtr(WeakPtr<T> && other)
	{
		_ptr = other._ptr;
		_ref = other._ref;
		other._ptr = nullptr;
		other._ref = nullptr;
	}

	WeakPtr(T * ptr)
		: _ptr(ptr), _ref(nullptr)
	{
		if (_ptr) {
			_ref = ptr->refCount();
			CC_WEAK_PTR_SAFE_RETAIN(_ref);
		}
	}

	WeakPtr(std::nullptr_t ptr)
		: _ptr(nullptr), _ref(nullptr)
	{
	}

	WeakPtr(const WeakPtr<T> & other)
		: _ptr(other._ptr),_ref(other._ref)
	{
		CC_WEAK_PTR_SAFE_RETAIN(_ref);
	}

	~WeakPtr()
	{
		CC_WEAK_PTR_SAFE_RELEASE(_ref);
	}

	WeakPtr<T> & operator = (const WeakPtr<T> & other)
	{
		if (other._ptr != _ptr)
		{
			CC_WEAK_PTR_SAFE_RETAIN(other._ref);
			CC_WEAK_PTR_SAFE_RELEASE(_ref);
			_ptr = other._ptr;
			_ref = other._ref;
		}
		return *this;
	}

	WeakPtr<T> & operator = (WeakPtr<T> && other)
	{
		if (other._ptr != _ptr)
		{
			CC_WEAK_PTR_SAFE_RELEASE(_ref);
			_ptr = other._ptr;
			_ref = other._ref;
			other._ptr = nullptr;
			other._ref = nullptr;
		}
		return *this;
	}

	WeakPtr<T> & operator = (T * other)
	{
		if (other != _ptr)
		{
			CC_WEAK_PTR_SAFE_RELEASE(_ref);

			if (other) {
				_ref = other->refCount();
				CC_WEAK_PTR_SAFE_RETAIN(_ref);
			}
			_ptr = other;
		}
		return *this;
	}

	WeakPtr<T> & operator = (std::nullptr_t other)
	{
		CC_WEAK_PTR_SAFE_RELEASE(_ref);
		_ptr = nullptr;
		return *this;
	}

	operator T * () const { return _ref && _ref->valied()?_ptr:nullptr; }

	T & operator * () const
	{
		CCASSERT(_ptr && _ref->valied(), "Attempt to dereference a null pointer!");
		return *_ptr;
	}

	T * operator->() const
	{
		CCASSERT(_ptr && _ref->valied(), "Attempt to dereference a null pointer!");
		return _ptr;
	}

	T * get() const { 
		return _ref && _ref->valied() ? _ptr:nullptr;
	}

	operator bool() const { return _ref && _ref->valied(); }

	bool operator == (const WeakPtr<T> & other) const { return get() == other.get(); }
	bool operator == (const T * other) const { return get() == other; }
	bool operator == (const std::nullptr_t other) const { return get() == other; }

	bool operator != (const WeakPtr<T> & other) const { return get() != other.get(); }
	bool operator != (const T * other) const { return get() != other; }
	bool operator != (const std::nullptr_t other) const { return get() != other; }

	void reset()
	{
		CC_WEAK_PTR_SAFE_RELEASE(_ref);
		_ptr = nullptr;
	}

	void swap(WeakPtr<T> & other)
	{
		if (other->_ptr != _ptr)
		{
			T * tmp = _ptr;
			_ptr = other._ptr;
			other._ptr = tmp;

			RefCount* ref_tmp = _ref;
			_ref = other._ref;
			other._ref = ref_tmp;
		}
	}

private:
	T * _ptr;
	RefCount* _ref;
	// NOTE: We can ensure T is derived from cocos2d::Ref at compile time here.
	static_assert(std::is_base_of<Ref, typename std::remove_const<T>::type>::value, "T must be derived from Ref");
};

NS_CC_END

#endif

