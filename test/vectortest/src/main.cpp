/**
 * @brief     unit test for taapp::vector
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#if defined(_DEBUG) && defined(_MSC_FULL_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define taapp_VECTOR_INTERNAL_API
#include <taapp/vector.h>
#include <taapp/allocator.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#ifdef NDEBUG
#error asserts are not enabled
#endif

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
    
    int_class& operator=(const int_class& b)
    {
        i_ = b.i_;
        return *this;
    }
};

int int_class::tracker;

template<typename T>
void test_vector(T& v)
{
    int ch;
    typename T::iterator itr;
    typename T::iterator end;
    assert(v.size() == 0);
    ch = 'a';
    // push the alphabet to the vector in order
    while(v.size() != 26)
    {
        v.push_back(ch);
        ++ch;
    }
    assert(*v.begin() == 'a');
    assert(v[25] == 'z');
    // remove 'e'
    itr = v.erase(v.begin() + ('e' - 'a'));
    assert(v.size() == 25);
    assert(*itr == 'f');
    // remove 'd'
    itr = v.erase(v.begin() + ('d' - 'a'));
    assert(v.size() == 24);
    assert(*itr == 'f');
    // insert d back in the correct position
    itr = v.insert(itr, 'd');
    assert(v.size() == 25);
    assert(*itr == 'd');
    // make sure contents of the vector is the ordered alphabet, excluding 'e'
    itr = v.begin();
    end = v.end();
    ch = 'a';
    while(itr != end)
    {
        if(ch == 'e')
        {
            assert(*itr == 'f');
            ++ch;
        }
        assert(*itr == ch);
        assert(v[itr - v.begin()] == ch);
        ++ch;
        ++itr;
    }
    v.pop_back();
    assert(v.size() == 24);
    v.resize(25);
    assert(v.size() == 25);
    v[24] = 'z';
}

int main(int argc, char* argv[])
{
    printf("testing taapp::vector<int>...");
    fflush(stdout);
    {
        // test int primitive (fast traits path)
        typedef taapp::allocator<int> int_allocator;
        typedef taapp::vector<int, int_allocator> int_vector;
        typedef int traits_check[int_vector::TRIVIAL*2 - 1];
        int_vector v;  
        test_vector(v);
    }
    printf("pass\n");
    printf("testing taapp::vector<int_class>...");
    fflush(stdout);
    {
        // test int object (slow traits path)
        typedef taapp::allocator<int_class> int_allocator;
        typedef taapp::vector<int_class, int_allocator> int_vector;
        typedef int traits_check[(!int_vector::TRIVIAL)*2 - 1];
        int_vector v;
        test_vector(v);
        assert(v.size() != 0);
        assert(int_class::tracker == static_cast<int>(v.size()));
        v.resize(v.size() + 30);
        assert(int_class::tracker == static_cast<int>(v.size()));
        v.resize(v.size() - 10);
        assert(int_class::tracker == static_cast<int>(v.size()));
        // verify that clear sets size to zero, but does not destroy reserve
        v.clear();
        assert(v.size() == 0);
        assert(v.capacity() != 0);
        assert(v.begin_ != NULL);
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
