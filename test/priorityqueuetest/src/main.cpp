/**
 * @brief     unit test for taapp::priority_queue
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define taapp_PRIORITY_QUEUE_INTERNAL_API
#include <taapp/priority_queue.h>
#include <taapp/allocator.h>
#include <taapp/vector.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#ifdef NDEBUG
#error asserts are not enabled
#endif

enum
{
    POT_SIZE = 768
};

class int_class
{
public:
    int i_;
    static int tracker;
    
    int_class()
    {
        ++tracker;
    }

    int_class(const int_class& b) : i_(b.i_)
    {
        ++tracker;
    }
    
    int_class(int b) : i_(b)
    {
        ++tracker;
    }
    
    ~int_class()
    {
        --tracker;
    }
    
    bool operator==(int b) const
    {
        return i_ == b;
    }

    operator int() const
    {
        return i_;
    }
    
    int_class& operator=(const int_class& b)
    {
        i_ = b.i_;
        return *this;
    }

};

int int_class::tracker;

template<typename T>
class priority_queue_test
{
public:

    static void execute()
    {
        typedef taapp::allocator<T> ialloc;
        typedef taapp::vector<T, ialloc> ivector;
        typedef taapp::priority_queue<T, ivector, icmp> ipot;
        ipot pot;
        int vec[POT_SIZE];
        int i;
        int prev;
        size_t size;
        size = 0;
        for(i = 0; i < POT_SIZE; ++i)
        {
            int val = rand() % POT_SIZE;
            pot.push(val);
            vec[i] = val;
            ++size;
        }
        assert(pot.size() == size);
        assert(!pot.empty());
        // make sure all the values inserted into the pot correctly
        for(i = 0; i < POT_SIZE; ++i)
        {
            int j;
            for(j = 0; j < POT_SIZE; ++j)
            {
                if(pot.container_[j] == vec[i])
                {
                    vec[i] = -1;
                    break;
                }
            }
            assert(j != POT_SIZE);
        }
        // make sure the pot is ordered correctly
        prev = 1024;
        for(i = 0; i < POT_SIZE; ++i)
        {
            printf("%4u", ((int) pot.top()));
            assert(prev >= pot.container_[0]);
            pot.pop();
            prev = pot.top();
            --size;
        }
        assert(pot.empty());
        assert(pot.size() == 0);
        printf("\n");
    }

    struct icmp
    {
        bool operator()(const T& a, const T& b) const
        {
            return a < b;
        }
    };

};

int main(int argc, char* argv[])
{
    printf("testing taapp::priority_queue<int>...\n");
    fflush(stdout);
    {
        // test int primitive
        priority_queue_test<int>::execute();        
    }
    printf("pass\n");
    printf("testing taapp::priority_queue<int_class>...\n");
    fflush(stdout);
    {
        // test int object
        priority_queue_test<int_class>::execute();
        assert(int_class::tracker == 0);
    }
    printf("pass\n");
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtCheckMemory();
    _CrtDumpMemoryLeaks();
#endif
    return EXIT_SUCCESS;
}
