/**
 * @brief     unit test for taapp::unordered_map
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <taapp/unordered_map.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>

static int maptest_allocate_counter = 0;
static int maptest_construct_counter = 0;
static int maptest_instance_counter = 0;

template<typename T>
class prim_wrap
{
public:
    T i_;
    
    prim_wrap()
    {
        ++maptest_construct_counter;
    }

    prim_wrap(const prim_wrap& b) : i_(b.i_)
    {
        ++maptest_construct_counter;
    }
    
    prim_wrap(T b) : i_(b)
    {
        ++maptest_construct_counter;
    }
    
    ~prim_wrap()
    {
        --maptest_construct_counter;
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
typedef prim_wrap<unsigned char*> ptr_class;

template<typename T> class test_alloc
{
public:

    template<typename U> struct rebind
    {
        typedef test_alloc<U> other;
    };

    test_alloc()
    {
        ++maptest_instance_counter;
    }

    ~test_alloc()
    {
        --maptest_instance_counter;
    }

    inline bool operator==(const test_alloc&) const
    {
        return true;
    }

    inline T* allocate (size_t n, const void* = 0) 
    {
        maptest_allocate_counter += n;
        T* p = static_cast<T*>(malloc(n * sizeof(T)));
        return p;
    }

    inline void deallocate(T* p, size_t n)
    {
        maptest_allocate_counter -= n;
        free(p);
    }

    inline void construct (T* p, const T& v)
    {
        new(static_cast<void*>(p)) constructor(v);
        ++maptest_construct_counter;
    }

    inline void destroy (T* p)
    {
        p->~T();
        --maptest_construct_counter;
    }

private:
    // noncopyable
    test_alloc(const test_alloc&);
    test_alloc& operator=(const test_alloc&);

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

template<typename T, typename U>
class map_test
{
public:

    static void execute()
    {
        {
            imap map;
            int size = 0;
            int max = 10000;
            // test insert
            for(int i = 0; i < max; ++i)
            {
                int j = i;
                typename imap::value_type v = 
                {
                    j, 
                    ((unsigned char*)NULL) + j
                };
                taapp::pair<typename imap::iterator, bool> ir;
                ir = map.insert(v);
                assert(ir.first->first == j);
                assert(ir.first->second == v.second);
                assert(ir.second);
                ir = map.insert(v);
                assert((*ir.first).first == j);
                assert((*ir.first).second == v.second);
                assert(!ir.second);
                ++size;
            }
            assert(static_cast<int>(map.size()) == size);

            // test iterator
            {
                typename imap::iterator itr(map.begin());
                typename imap::iterator end(map.end());
                int c = 0;
                while(itr != end)
                {
                    assert(itr->first < max);
                    assert(itr->second < ((unsigned char*)NULL) + max);
                    ++itr;
                    ++c;
                }
                assert(c == size);
            }

            // test erase
            while(map.size() > 1)
            {
                int j = rand() % max;
                typename imap::iterator itr = map.find(j);
                if(itr != map.end())
                {
                    assert(itr->first == j);
                    if(rand() % 2 == 0)
                    {
                        map.erase(itr);
                    }
                    else
                    {
                        size_t n = map.erase(j);
                        assert(n == 1);
                    }
                    --size;
                }
            }
            // insert randomly
            for(int i = 0; i < max; ++i)
            {
                int j = rand() % max;
                typename imap::value_type v =
                {
                    j,
                    ((unsigned char*)NULL) + j
                };
                taapp::pair<typename imap::iterator, bool> ir;
                ir = map.insert(v);
                if(ir.second)
                {
                    ++size;
                }
            }
            assert(size == static_cast<int>(map.size()));
            // test clear
            map.clear();
            assert(0 == map.size());
            // insert again to test destruction
            typename imap::value_type v = { 0, NULL };
            map.insert(v);
        }
        assert(maptest_instance_counter == 0);
        assert(maptest_allocate_counter == 0);
        assert(maptest_construct_counter == 0);
    }

private:

    struct iequal
    {
        bool operator()(const T& a, const T& b) const
        {
            return a == b;
        }
    };

    struct ihash
    {
        unsigned int operator()(const T& a) const
        {
            return a;
        }
    };

    typedef test_alloc<T> ialloc;
    typedef taapp::unordered_map<T,U,ihash,iequal,ialloc> imap;
};

int main(int argc, char* argv[])
{
    printf("testing taapp::unordered_map<int, unsigned char*>...");
    fflush(stdout);
    map_test<int, unsigned char*>::execute();
    printf("pass\n");
    printf("testing taapp::unordered_map<int_class, ptr_class>...");
    fflush(stdout);
    map_test<int_class, ptr_class>::execute();
    printf("pass\n");
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtCheckMemory();
    _CrtDumpMemoryLeaks();
#endif
    return EXIT_SUCCESS;
}
