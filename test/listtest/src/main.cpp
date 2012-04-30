/**
 * @brief     unit test for taapp::list
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <taapp/list.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>

static int listtest_allocate_counter = 0;
static int listtest_construct_counter = 0;
static int listtest_instance_counter = 0;

#ifdef NDEBUG
#error asserts are not enabled
#endif

template<typename T>
class prim_wrap
{
public:
    T i_;
    
    prim_wrap()
    {
        ++listtest_construct_counter;
    }

    prim_wrap(const prim_wrap& b) : i_(b.i_)
    {
        ++listtest_construct_counter;
    }
    
    prim_wrap(T b) : i_(b)
    {
        ++listtest_construct_counter;
    }
    
    ~prim_wrap()
    {
        --listtest_construct_counter;
    }
    
    bool operator==(T b) const
    {
        return i_ == b;
    }

    bool operator==(const prim_wrap& b) const
    {
        return i_ == b.i_;
    }

    bool operator<(T b) const
    {
        return i_ < b;
    }

    operator T() const
    {
        return i_;
    }
    
    prim_wrap& operator=(const prim_wrap& b)
    {
        i_ = b.i_;
        return *this;
    }
};

typedef prim_wrap<int> int_class;

template<typename T> class listtest_allocator
{
public:

    template<typename U> struct rebind
    {
        typedef listtest_allocator<U> other;
    };

    listtest_allocator()
    {
        ++listtest_instance_counter;
    }

    ~listtest_allocator()
    {
        --listtest_instance_counter;
    }

    inline bool operator==(const listtest_allocator&) const
    {
        return true;
    }

    inline T* allocate (int n, const void* = 0) 
    {
        ++listtest_allocate_counter;
        assert(n == 1);
        return new T;
    }

    inline void deallocate(T* p, int n)
    {
        --listtest_allocate_counter;
        assert(n == 1);
        delete p;
    }

    inline void construct (T* p, const T& v)
    {
        new(static_cast<void*>(p)) constructor(v);
        ++listtest_construct_counter;
    }

    inline void destroy (T* p)
    {
        p->~T();
        --listtest_construct_counter;
    }


private:
    static int instance_counter_;
    static int alloc_counter_;
    static int construct_counter_;

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

    // noncopyable
    listtest_allocator(const listtest_allocator&);
    listtest_allocator& operator=(const listtest_allocator&);
};

template<typename T>
void test_list()
{
    typedef listtest_allocator<T> int_allocator;
    typedef taapp::list<T, int_allocator> int_list;
    {
        int_list list;
        int_list list2;

        list.push_back(0);
        list.push_back(1);
        list.push_back(2);
        list2.push_front(6);
        list2.push_front(5);
        list2.push_front(4);
        list2.push_front(9);

        typename int_list::iterator pos(list2.begin());
        typename int_list::iterator dstpos(list.end());
        list.splice(dstpos, list2);
        assert(list2.empty() == true);
        assert(list.empty() == false);
        list.insert(pos, 3);
        list.erase(pos);

        typename int_list::const_iterator itr(list.begin());
        typename int_list::const_iterator end(list.end());
        typename int_list::const_iterator last(itr);
        int i = 0;
        while(itr != end)
        {
            assert(*itr == i);
            last = itr;
            ++i; 
            ++itr;
        }
        itr = last;
        while(itr != end)
        {
            --i;
            assert(*itr == i);
            --itr;
        }
        list.pop_front();
        list.pop_back();
        assert(list.empty() == false);
        assert(list.front() == 1);
        assert(list.back() == 5);
    }
    assert(listtest_instance_counter == 0);
    assert(listtest_allocate_counter == 0);
    assert(listtest_construct_counter == 0);
}

int main(int argc, char* argv[])
{
    printf("testing taapp::list<int>...");
    test_list<int>();
    printf("pass\n");
    printf("testing taapp::list<int_class>...");
    test_list<int_class>();
    printf("pass\n");
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtCheckMemory();
    _CrtDumpMemoryLeaks();
#endif
    return EXIT_SUCCESS;
}
