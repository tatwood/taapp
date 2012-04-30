/**
 * @brief     unit test for taapp::set
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define taapp_SET_INTERNAL_API
#include <taapp/set.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#ifdef NDEBUG
#error asserts are not enabled
#endif

static int settest_allocate_counter = 0;
static int settest_construct_counter = 0;
static int settest_instance_counter = 0;

template<typename T>
class prim_wrap
{
public:
    T i_;
    
    prim_wrap()
    {
        ++settest_construct_counter;
    }

    prim_wrap(const prim_wrap& b) : i_(b.i_)
    {
        ++settest_construct_counter;
    }
    
    prim_wrap(T b) : i_(b)
    {
        ++settest_construct_counter;
    }
    
    ~prim_wrap()
    {
        --settest_construct_counter;
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

template<typename T> class test_alloc
{
public:

    template<typename U> struct rebind
    {
        typedef test_alloc<U> other;
    };

    test_alloc()
    {
        ++settest_instance_counter;
    }

    ~test_alloc()
    {
        --settest_instance_counter;
    }

    inline bool operator==(const test_alloc&) const
    {
        return true;
    }

    inline T* allocate (size_t n, const void* = 0) 
    {
        settest_allocate_counter += n;
        T* p = static_cast<T*>(malloc(n * sizeof(T)));
        return p;
    }

    inline void deallocate(T* p, size_t n)
    {
        settest_allocate_counter -= n;
        free(p);
    }

    inline void construct (T* p, const T& v)
    {
        new(static_cast<void*>(p)) constructor(v);
        ++settest_construct_counter;
    }

    inline void destroy (T* p)
    {
        p->~T();
        --settest_construct_counter;
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

template<typename T>
class set_test
{
public:

    static void execute()
    {
        {
            iset set;
            int size = 0;
            int max = 10000;
            // test insert
            for(int i = 0; i < max; ++i)
            {
                int j = i;
                taapp::pair<typename iset::iterator, bool> ir;
                ir = set.insert(j);
                assert(*ir.first == j);
                assert(ir.second);
                ir = set.insert(j);
                assert(*ir.first == j);
                assert(!ir.second);
                ++size;
                validate_tree(set.root_);
            }
            validate_tree(set.root_);
            assert(static_cast<int>(set.size()) == size);

            // test iterator
            {
                typename iset::iterator itr(set.begin());
                typename iset::iterator end(set.end());
                int prev = -1;
                int c = 0;
                while(itr != end)
                {
                    assert(prev <= *itr);
                    prev = *itr;
                    ++itr;
                    ++c;
                }
                assert(c == size);
            }

            // test erase
            while(set.size() > 1)
            {
                int j = rand() % max;
                typename iset::iterator itr = set.find(j);
                if(itr != set.end())
                {
                    assert(*itr == j);
                    if(rand() % 2 == 0)
                    {
                        set.erase(itr);
                    }
                    else
                    {
                        size_t n = set.erase(j);
                        assert(n == 1);
                    }
                    validate_tree(set.root_);
                    --size;
                }
            }

            // insert randomly
            for(int i = 0; i < max; ++i)
            {
                int j = rand() % max;
                taapp::pair<typename iset::iterator, bool> ir;
                ir = set.insert(j);
                if(ir.second)
                {
                    ++size;
                }
                validate_tree(set.root_);
            }
            assert(size == static_cast<int>(set.size()));
            // test clear
            set.clear();
            assert(set.empty());
            assert(0 == set.size());

            // insert again to test destruction
            set.insert(0);
        }
        assert(settest_instance_counter == 0);
        assert(settest_allocate_counter == 0);
        assert(settest_construct_counter == 0);
    }

private:

    struct icmp
    {
        bool operator()(const T& a, const T& b) const
        {
            return a < b;
        }
    };

    typedef test_alloc<T> ialloc;
    typedef taapp::set<T, icmp, ialloc> iset;

    static int validate_tree(typename iset::rbnode* n)
    {
        int lheight = 0;
        int rheight = 0;
        if(n->left != NULL)
        {
            // check for red violation (child of red cannot be red)
            assert(n->color != iset::RED || n->left->color != iset::RED);
            assert(n->left->value <= n->value);
            assert(n->left->parent == n);
            lheight = validate_tree(n->left);
        }
        if(n->right != NULL)
        {
            // check for red violation (child of red cannot be red)
            assert(n->color != iset::RED || n->right->color != iset::RED);
            assert(n->right->value >= n->value);
            assert(n->right->parent == n);
            rheight = validate_tree(n->right);
        }

        if(lheight != 0 && rheight != 0)
        {
            // check black violations (equal number of blacks on each branch)
            assert(lheight == rheight);
        }
        return lheight + ((n->color == iset::BLACK) ? 1 : 0);
    }
};

int main(int argc, char* argv[])
{
    printf("testing taapp::set<int>...");
    fflush(stdout);
    set_test<int>::execute();
    printf("pass\n");
    printf("testing taapp::set<int_class>...");
    fflush(stdout);
    set_test<int_class>::execute();
    printf("pass\n");
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtCheckMemory();
    _CrtDumpMemoryLeaks();
#endif
    return EXIT_SUCCESS;
}
