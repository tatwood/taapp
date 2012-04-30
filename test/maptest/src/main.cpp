/**
 * @brief     unit test for taapp::map
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define taapp_MAP_INTERNAL_API
#include <taapp/map.h>
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
                    j, ((unsigned char*)NULL) + j
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
                validate_tree(map.root_);
            }
            validate_tree(map.root_);
            assert(static_cast<int>(map.size()) == size);

            // test iterator
            {
                typename imap::iterator itr(map.begin());
                typename imap::iterator end(map.end());
                int prev = -1;
                int c = 0;
                while(itr != end)
                {
                    assert(prev <= itr->first);
                    prev = itr->first;
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
                    validate_tree(map.root_);
                    --size;
                }
            }

            // insert randomly
            for(int i = 0; i < max; ++i)
            {
                int j = rand() % max;
                typename imap::value_type v =
                {
                    j, ((unsigned char*)NULL) + j
                };
                taapp::pair<typename imap::iterator, bool> ir;
                ir = map.insert(v);
                if(ir.second)
                {
                    ++size;
                }
                validate_tree(map.root_);
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

    struct icomp
    {
        bool operator()(const T& a, const T& b) const
        {
            return a < b;
        }
    };

    typedef test_alloc<T> ialloc;
    typedef taapp::map<T, U, icomp, ialloc> imap;

    static int validate_tree(typename imap::rbnode* n)
    {
        int lheight = 0;
        int rheight = 0;
        if(n->left != NULL)
        {
            // check for red violation (child of red cannot be red)
            assert(n->color != imap::RED || n->left->color != imap::RED);
            assert(n->left->value.first <= n->value.first);
            assert(n->left->parent == n);
            lheight = validate_tree(n->left);
        }
        if(n->right != NULL)
        {
            // check for red violation (child of red cannot be red)
            assert(n->color != imap::RED || n->right->color != imap::RED);
            assert(n->right->value.first >= n->value.first);
            assert(n->right->parent == n);
            rheight = validate_tree(n->right);
        }

        if(lheight != 0 && rheight != 0)
        {
            // check black violations (equal number of blacks on each branch)
            assert(lheight == rheight);
        }
        return lheight + ((n->color == imap::BLACK) ? 1 : 0);
    }
};

int main(int argc, char* argv[])
{
    printf("testing taapp::map<int, unsigned char*>...");
    fflush(stdout);
    map_test<int, unsigned char*>::execute();
    printf("pass\n");
    printf("testing taapp::map<int_class, ptr_class>...");
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
