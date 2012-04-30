/**
 * @brief     C++ allocator template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_ALLOCATOR_H_
#define taapp_ALLOCATOR_H_

#include <cassert>
#include <cstdlib>

namespace taapp
{

template<typename T> class allocator
{
public:

    template<typename U> struct rebind
    {
        typedef allocator<U> other;
    };

#ifndef NDEBUG
    allocator() : counter_(0)
    {
    }

    ~allocator()
    {
        assert(counter_ == 0);
    }
#endif

    inline bool operator==(const allocator&) const
    {
        return true;
    }

    inline bool operator!=(const allocator&) const
    {
        return false;
    }

    // allocate but don't initialize num elements of type T
    inline T* allocate (size_t n, const void* = 0)
    {
        T* t = static_cast<T*>(malloc(sizeof(*t) * n));
#ifndef NDEBUG
        ++counter_;
#endif
        return t;
    }

    inline T* reallocate(void* p, size_t n)
    {
        T* t = static_cast<T*>(realloc(p, sizeof(*t)*n));
#ifndef NDEBUG
        if(p == NULL)
        {
            ++counter_;
        }
#endif
        return t;
    }

    // initialize elements of allocated storage p with value v
    inline void construct (T* p, const T& v)
    {
        new(static_cast<void*>(p)) constructor(v);
    }

    // destroy but do not deallocate elements of initialized storage p
    inline void destroy (T* p)
    {
        p->~T();
    }

    // deallocate storage p of deleted elements
    inline void deallocate(T* p, size_t n)
    {
#ifndef NDEBUG
        --counter_;
#endif
        free(p);
    }

private:

#ifndef NDEBUG
    int counter_;
#endif

    // define a custom placement new operator to remove dependency on
    // the std <new> header
    class constructor
    {
    public:
        T t_;

        inline constructor(const T& t) : t_(t)
        {
        }

        inline void* operator new (size_t size, void* ptr)
        {
            return ptr;
        }

        inline void operator delete (void *, void *)
        {
        }
    };
};

}

#endif // taapp_ALLOCATOR_H_

