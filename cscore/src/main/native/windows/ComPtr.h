#pragma once

#include <comdef.h>

#include <shlwapi.h>  // QISearch

namespace cs
{

template <class T>
class ComPtr
{
public:
    ComPtr()
    {
    }
    ComPtr(T* lp)
    {
        p = lp;
    }
    ComPtr(_In_ const ComPtr<T>& lp)
    {
        p = lp.p;
    }
    virtual ~ComPtr()
    {
    }

    T** operator&()
    {
        return p.operator&();
    }
    T* operator->() const
    {
        return p.operator->();
    }
    operator bool()
    {
        return p.operator!=(NULL);
    }

    T* Get() const
    {
        return p;
    }

    void Release()
    {
        if (p)
            p.Release();
    }

    // query for U interface
    template<typename U>
    HRESULT As(_Out_ ComPtr<U>& lp) const
    {
        lp.Release();
        return p->QueryInterface(__uuidof(U), reinterpret_cast<void**>((T**)&lp));
    }
private:
    _COM_SMARTPTR_TYPEDEF(T, __uuidof(T));
    TPtr p;
};
}

#define _ComPtr cs::ComPtr
