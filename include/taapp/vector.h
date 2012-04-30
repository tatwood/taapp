/**
 * @brief     C++ vector container template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_VECTOR_H_
#define taapp_VECTOR_H_

#include <cstddef>
#include <cassert>
#include <cstring>

namespace taapp
{

/**
 * @brief a dynamically sized vector template
 * @details This class is a subset of std::vector. It is dependent upon
 * compiler extensions for support of type trait intrisincs and will only
 * build on compilers that provide them. Microsoft Visual C++ provides this
 * support on versions 2005+. GCC provides this support on versions
 * 4.3.03+.
 */
template<typename T, typename Allocator> class vector
{
public:

    typedef T* iterator;
    typedef const T* const_iterator;

    vector() : begin_(NULL), end_(NULL), capacity_(NULL)
    {
    }

    ~vector()
    {
        if(begin_ != NULL)
        {
            destroy_range(begin_, end_);
            allocator_.deallocate(begin_, capacity());
        }
    }

    inline const T& operator[](size_t index) const
    {
        assert(begin_ + index < end_);
        return begin_[index];
    }

    inline T& operator[](size_t index)
    {
        assert(begin_ + index < end_);
        return begin_[index];
    }

    inline const T& back() const
    {
        return *(end_ - 1);
    }

    inline T& back()
    {
        return *(end_ - 1);
    }

    inline const_iterator begin() const
    {
        return begin_;
    }

    inline iterator begin()
    {
        return begin_;
    }

    inline size_t capacity()
    {
        return static_cast<size_t>(capacity_ - begin_);
    }

    inline void clear()
    {
        destroy_range(begin_, end_);
        end_ = begin_;
    }

    inline bool empty() const
    {
        return begin_ == end_;
    }

    inline const_iterator end() const
    {
        return end_;
    }

    inline iterator end()
    {
        return end_;
    }

    iterator erase(iterator it)
    {
        assert(it >= begin_);
        assert(it < end_);
        T* end = --end_;
        if(TRIVIAL_ASSIGN)
        {
            memmove(
                it,
                it + 1,
                reinterpret_cast<size_t>(end)-reinterpret_cast<size_t>(it));
        }
        else
        {
            T* itr = it;
            while(itr != end)
            {
                *itr = *(itr + 1);
                ++itr;
            }           
        }
        allocator_.destroy(end_);
        return it;
    }

    inline const T& front() const
    {
        return *begin_;
    }

    inline T& front()
    {
        return *begin_;
    }

    iterator insert(iterator pos, const T& t)
    {
        assert(pos >= begin_);
        assert(pos <= end_);
        size_t off;
        off = reinterpret_cast<size_t>(pos) - reinterpret_cast<size_t>(begin_);
        resize(size() + 1);
        // resize may invalidate pos, so need to recalculate it
        pos = reinterpret_cast<T*>(reinterpret_cast<size_t>(begin_)+off);
        if(TRIVIAL_ASSIGN)
        {
            memmove(
                pos + 1,
                pos,
                reinterpret_cast<size_t>(end_) -
                reinterpret_cast<size_t>(pos+1));
        }
        else
        {
            T* itr = end_ - 1;
            while(itr != pos)
            {
                *itr = *(itr - 1);
                --itr;
            }
        }
        *pos = t;
        return pos;
    }

    void pop_back()
    {
        assert(begin_ != end_);
        --end_;
        allocator_.destroy(end_);
    }

    void push_back(const T& t)
    {
        if(capacity_ == end_)
        {
            size_t c = capacity();
            reallocate(c, increment_capacity(c));
        }
        allocator_.construct(end_++, t);
    }

    void reserve(size_t c)
    {
        size_t old_capacity = capacity();
        if(c > old_capacity)
        {
            reallocate(old_capacity, c);
        }
    }

    void resize(size_t size)
    {
        size_t c = capacity();
        if(size > c)
        {
            size_t new_capacity = increment_capacity(c);
            if(new_capacity < size)
            {
                new_capacity = size;
            }
            reallocate(c, new_capacity);
        }
        T* end = begin_ + size;
        construct_range(end_, end);
        destroy_range(end, end_);
        end_ = end;
    }

    void resize(size_t size, const T& t)
    {
        size_t c = capacity();
        if(size > c)
        {
            size_t new_capacity = increment_capacity(c);
            if(new_capacity < size)
            {
                new_capacity = size;
            }
            reallocate(c, new_capacity);
        }
        T* itr = end_;
        T* end = begin_ + size;
        while(itr < end)
        {
            allocator_.construct(itr, t);
            ++itr;
        }
        destroy_range(end, end_);
        end_ = end;
    }

    inline size_t size() const
    {
        return static_cast<size_t>(end_ - begin_);
    }

#ifndef taapp_VECTOR_INTERNAL_API
private:
#endif // taapp_VECTOR_INTERNAL_API

    enum
    {
        TRIVIAL_ASSIGN = __has_trivial_assign(T) | !__is_class(T),
        TRIVIAL_COPY = __has_trivial_copy(T) | !__is_class(T),        
        TRIVIAL_CONSTRUCTOR = __has_trivial_constructor(T) | !__is_class(T),
        TRIVIAL_DESTRUCTOR = __has_trivial_destructor(T) | !__is_class(T),
    };

    enum
    {
        TRIVIAL = 
            TRIVIAL_ASSIGN &
            TRIVIAL_COPY &
            TRIVIAL_CONSTRUCTOR &
            TRIVIAL_DESTRUCTOR    
    };

    // Turn this on if you only want to support trivial types:
    // typedef int TrivialOnlyCheck[TRIVIAL*2 - 1];

    T* begin_;
    T* end_;
    T* capacity_;
    Allocator allocator_;
    
    inline size_t increment_capacity(size_t c)
    {
        return (c < 64) ? ((c == 0) ? 8 : c << 1) : c + 64;
    }    

    inline void construct_range(
        iterator begin,
        iterator end)
    {
        if(!TRIVIAL_CONSTRUCTOR)
        {
            T t;
            while(begin < end)
            {
                allocator_.construct(begin, t);
                ++begin;
            }
        }
    }
    
    inline void copyconstruct_range(
        iterator begin,
        iterator end,
        const_iterator src)
    {
        if(TRIVIAL_COPY)
        {
            memcpy(
                begin,
                src,
                reinterpret_cast<size_t>(end) -
                reinterpret_cast<size_t>(begin));
        }
        else
        {
            while(begin < end)
            {
                allocator_.construct(begin, *src);
                ++begin;
                ++src;
            }
        }
    }    

    inline void destroy_range(iterator begin, iterator end)
    {
        if(!TRIVIAL_DESTRUCTOR)
        {
            while(begin < end)
            {
                allocator_.destroy(begin);
                ++begin;
            }
        }
    }
    
    void reallocate(
        size_t old_capacity,
        size_t new_capacity)
    {
        iterator buffer;
        size_t sz = size();
        if(TRIVIAL_COPY && TRIVIAL_ASSIGN)
        {
            buffer = allocator_.reallocate(begin_, new_capacity);
        }
        else
        {
            buffer = allocator_.allocate(new_capacity, begin_);
            if(begin_ != NULL)
            {
                copyconstruct_range(buffer, buffer + sz, begin_);
                destroy_range(begin_, end_);
                allocator_.deallocate(begin_, old_capacity);
            }
        }
        begin_ = buffer;
        end_ = buffer + sz;
        capacity_ = buffer + new_capacity;
    }

private:
    // noncopyable
    vector(const vector&);
    vector& operator=(const vector&);
};

}

#endif // taa_VECTOR_H_

