/**
 * @brief     C++ hash map container template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_UNORDERED_MAP_H_
#define taapp_UNORDERED_MAP_H_

#include "pair.h"
#include <cassert>
#include <cstddef>

namespace taapp
{

/**
 * @brief hash map
 * @details This class is a subset of std::tr1::unordered_map. It is
 * implemented as a table of double ended linked lists.
 */
template<typename Key,
         typename T,
         typename Hash,
         typename Pred,
         typename Alloc>
class unordered_map
{
public:

    typedef pair<Key, T> value_type;

    class iterator
    {
    public:

        inline iterator() : node_(NULL), bucket_(NULL), bucketend_(NULL)
        {
        }

        inline iterator(const iterator& itr) :
            node_(itr.node_),
            bucket_(itr.bucket_),
            bucketend_(itr.bucketend_)
        {
        }

        inline operator value_type&()
        {
            return node_->value;
        }

        inline bool operator==(const iterator& itr) const
        {
            return node_ == itr.node_;
        }

        inline bool operator!=(const iterator& itr) const
        {
            return node_ != itr.node_;
        }

        inline iterator& operator=(const iterator& itr)
        {
            node_ = itr.node_;
            bucket_ = itr.bucket_;
            bucketend_ = itr.bucketend_;
            return *this;
        }

        inline iterator& operator++()
        {
            struct unordered_map::tnode* n = node_;
            n = n->node.tnext;
            if(static_cast<const void*>(n)==static_cast<const void*>(bucket_))
            {
                struct unordered_map::anode* b = bucket_;
                struct unordered_map::anode* bend = bucketend_;
                n = NULL;
                ++b;
                while(b != bend)
                {
                    if(b->anext != b)
                    {
                        n = b->tnext;
                        break;
                    }
                    ++b;
                }
                bucket_ = b;
            }
            node_ = n;
            return *this;
        }

        inline value_type& operator*()
        {
            return node_->value;
        }

        inline value_type* operator->()
        {
            return &node_->value;
        }

    private:

        struct unordered_map::tnode* node_;
        struct unordered_map::anode* bucket_;
        struct unordered_map::anode* bucketend_;

        inline explicit iterator(
            struct unordered_map::tnode* node,
            struct unordered_map::anode* bucket,
            struct unordered_map::anode* bucketend)
            :
            node_(node),
            bucket_(bucket),
            bucketend_(bucketend)
        {
        }

        inline explicit iterator(
            struct unordered_map::anode* bucket,
            struct unordered_map::anode* bucketend)
        {
            node_ = NULL;
            while(bucket != bucketend)
            {
                if(bucket->anext != bucket)
                {
                    node_ = bucket->tnext;
                    break;
                }
                ++bucket;
            }
            bucket_ = bucket;
            bucketend_ = bucketend;
        }

        friend class const_iterator;
        friend class unordered_map;
    };

    class const_iterator
    {
    public:

        inline const_iterator() : node_(NULL), bucket_(NULL), bucketend_(NULL)
        {
        }

        inline const_iterator(const const_iterator& itr) :
            node_(itr.node_),
            bucket_(itr.bucket_),
            bucketend_(itr.bucketend_)
        {
        }

        inline const_iterator(const iterator& itr) :
            node_(itr.node_),
            bucket_(itr.bucket_),
            bucketend_(itr.bucketend_)
        {
        }

        inline operator const value_type&()
        {
            return node_->value;
        }

        inline bool operator==(const const_iterator& itr) const
        {
            return node_ == itr.node_;
        }

        inline bool operator==(const iterator& itr) const
        {
            return node_ == itr.node_;
        }

        inline bool operator!=(const const_iterator& itr) const
        {
            return node_ != itr.node_;
        }

        inline bool operator!=(const iterator& itr) const
        {
            return node_ != itr.node_;
        }

        inline const_iterator& operator=(const const_iterator& itr)
        {
            node_ = itr.node_;
            bucket_ = itr.bucket_;
            bucketend_ = itr.bucketend_;
            return *this;
        }

        inline const_iterator& operator=(const iterator& itr)
        {
            node_ = itr.node_;
            bucket_ = itr.bucket_;
            bucketend_ = itr.bucketend_;
            return *this;
        }

        inline iterator& operator++()
        {
            const struct unordered_map::tnode* n = node_;
            n = n->tnext;
            if(static_cast<const void*>(n)==static_cast<const void*>(bucket_))
            {
                const struct unordered_map::anode* b = bucket_;
                const struct unordered_map::anode* bend = bucketend_;
                n = NULL;
                ++b;
                while(b != bend)
                {
                    if(b->anext != b)
                    {
                        n = b->tnext;
                        break;
                    }
                    ++b;
                }
                bucket_ = b;
            }
            node_ = n;
            return *this;
        }

        inline const value_type& operator*()
        {
            return node_->value;
        }

        inline const value_type* operator->()
        {
            return &node_->value;
        }

    private:

        const struct unordered_map::tnode* node_;
        const struct unordered_map::anode* bucket_;
        const struct unordered_map::anode* bucketend_;

        inline explicit const_iterator(
            const struct unordered_map::tnode* node,
            const struct unordered_map::anode* bucket,
            const struct unordered_map::anode* bucketend)
            :
            node_(node),
            bucket_(bucket),
            bucketend_(bucketend)
        {
        }

        inline explicit const_iterator(
            const struct unordered_map::anode* bucket,
            const struct unordered_map::anode* bucketend)
        {
            node_ = NULL;
            while(bucket != bucketend)
            {
                if(bucket->anext != bucket)
                {
                    node_ = bucket->tnext;
                    break;
                }
                ++bucket;
            }
            bucket_ = bucket;
            bucketend_ = bucketend;
        }

        friend class unordered_map;
    };

    unordered_map() :
        buckets_(NULL), numbuckets_(0), size_(0), max_load_factor_(1.0f)
    {
    }

    ~unordered_map()
    {
        if(buckets_ != NULL)
        {
            clear();
            bucketallocator_.deallocate(buckets_, numbuckets_);
        }
    }

    const_iterator begin() const
    {
        return const_iterator(buckets_, buckets_+numbuckets_);
    }

    iterator begin()
    {
        return iterator(buckets_, buckets_+numbuckets_);
    }

    void clear()
    {
        bucket_type* bitr = buckets_;
        bucket_type* bend = bitr + numbuckets_;
        while(bitr != bend)
        {
            tnode* n = bitr->tnext;
            while(static_cast<void*>(n) != static_cast<void*>(bitr))
            {
                tnode* next = n->node.tnext;
                n->value.~value_type();
                allocator_.deallocate(n, 1);
                n = next;
            }
            bitr->aprev = bitr;
            bitr->anext = bitr;
            ++bitr;
        }
        size_ = 0;
    }

    const_iterator end() const
    {
        return const_iterator();
    }

    iterator end()
    {
        return iterator();
    }

    inline size_t erase(const Key& k)
    {
        iterator itr(find(k));
        bool found = itr.node_ != NULL;
        if(found)
        {
            erase(itr);
        }
        return found;
    }

    void erase(iterator itr)
    {
        bucket_erase(itr.bucket_, itr.node_);
        itr.node_->value.~value_type();
        allocator_.deallocate(itr.node_, 1);
        --size_;
    }

    const_iterator find(const Key& k) const
    {
        const_iterator result;
        if(buckets_ != NULL)
        {
            const bucket_type* b = get_bucket(k);
            const tnode* n = b->tnext;
            while(static_cast<const void*>(n) != static_cast<const void*>(b))
            {
                if(equals_(k, n->value.first))
                {
                    result.node_ = n;
                    result.bucket_ = b;
                    result.bucketend_ = buckets_ + numbuckets_;
                    break;
                }
                n = n->node.tnext;
            }
        }
        return result;
    }

    iterator find(const Key& k)
    {
        iterator result;
        if(buckets_ != NULL)
        {
            bucket_type* b = get_bucket(k);
            tnode* n = b->tnext;
            while(static_cast<const void*>(n) != static_cast<const void*>(b))
            {
                if(equals_(k, n->value.first))
                {
                    result.node_ = n;
                    result.bucket_ = b;
                    result.bucketend_ = buckets_ + numbuckets_;
                    break;
                }
                n = n->node.tnext;
            }
        }
        return result;
    }

    pair<iterator, bool> insert(const value_type& v)
    {
        pair<iterator, bool> result = { find(v.first), false };
        if(result.first.node_ == NULL)
        {
            // key does not exist in the map
            if(size_ == 0 || load_factor() >= max_load_factor_)
            {
                rehash(calc_table_size(numbuckets_ + 1));
            }
            bucket_type* b = get_bucket(v.first);
            tnode* n = allocator_.allocate(1);
            new(static_cast<void*>(&n->value)) constructor(v);
            bucket_push(b, n);
            result.first.node_ = n;
            result.first.bucket_ = b;
            result.first.bucketend_ = buckets_ + numbuckets_;
            result.second = true;
            ++size_;
        }
        return result;
    }

    float load_factor() const
    {
        return static_cast<float>(size_)/static_cast<float>(numbuckets_);
    }

    void max_load_factor(float z)
    {
        max_load_factor_ = z;
        if(load_factor() > max_load_factor_)
        {
            rehash(calc_table_size(numbuckets_ + 1));
        }
    }

    void rehash(size_t count)
    {
        bucket_type* oldbuckets = buckets_;
        size_t oldnumbuckets = numbuckets_;
        if(count > oldnumbuckets)
        {
            buckets_ = bucketallocator_.allocate(count);
            numbuckets_ = count;
            // initialize the new buckets
            bucket_type* b = buckets_;
            bucket_type* bend = buckets_ + count;
            while(b != bend)
            {
                b->aprev = b;
                b->anext = b;
                ++b;
            }
            if(size_ > 0)
            {
                bucket_type* b = oldbuckets;
                bucket_type* bend = oldbuckets + oldnumbuckets;
                while(b != bend)
                {
                    // move everything from the old table to the new one
                    while(b->anext != b) 
                    {
                        tnode* n = b->tnext;
                        bucket_erase(b, n);
                        bucket_type* newbucket = get_bucket(n->value.first);
                        bucket_push(newbucket, n);
                    }
                    ++b;
                }
            }
            if(oldnumbuckets > 0)
            {
                // free the old memory
                bucketallocator_.deallocate(oldbuckets, oldnumbuckets);
            }
        }
    }

    size_t size() const
    {
        return size_;
    }

#ifndef taapp_UNORDERED_MAP_INTERNAL_API
private:
#endif // taapp_UNORDERED_MAP_INTERNAL_API

    struct anode
    {
        union
        {
            struct
            {
                struct unordered_map::anode* aprev;
                struct unordered_map::anode* anext;
            };
            struct
            {
                struct unordered_map::tnode* tprev;
                struct unordered_map::tnode* tnext;
            };
        };
    };

    struct tnode
    {
        anode node;
        value_type value;
    };

    // define a custom placement new operator to remove dependency on
    // the std <new> header. cannot use allocator version because it expects
    // type tnode. could add constructor to tnode that accepts value_type,
    // but want tnode to remain POD if possible; so construction is done here
    class constructor
    {
    public:
        value_type t_;

        inline constructor(const value_type& t) : t_(t)
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

    typedef anode bucket_type;
    typedef typename Alloc::template rebind<tnode>::other allocator_type;
    typedef typename Alloc::template rebind<anode>::other bucket_allocator;

    bucket_type* buckets_;
    size_t numbuckets_;
    size_t size_;
    float max_load_factor_;
    Hash hasher_;
    Pred equals_;
    allocator_type allocator_;
    bucket_allocator bucketallocator_;

    size_t calc_table_size(size_t size)
    {
        static const size_t table[] =
        {
                     13,         31,         61,        127,        251,
                    509,       1021,       2039,       4093,       8191,
                  16381,      32749,      65521,     131071,     262139,
                 524287,    1048573,    2097143,    4194301,    8388593,
               16777199,   33554393,   67108859,  134217689,  201326611,
              402653189,  805306457, 1610612741
        };
        const size_t* titr = table;
        const size_t* tend = table + sizeof(table)/sizeof(table[0]) - 1;
        while(titr != tend)
        {
            if(*titr >= size)
            {
                break;
            }
            ++titr;
        }
        return *titr;
    }

    inline void bucket_erase(bucket_type* bucket, tnode* n)
    {
        n->node.aprev->anext = n->node.anext;
        n->node.anext->aprev = n->node.aprev;
    }

    // pushes to front of bucket
    inline void bucket_push(bucket_type* bucket, tnode* n)
    {
        n->node.anext = bucket->anext;
        n->node.aprev = bucket;
        bucket->anext->aprev = &n->node;
        bucket->anext = &n->node;
    }

    inline const bucket_type* get_bucket(const Key& k) const
    {
        return buckets_ + (hasher_(k) % numbuckets_);
    }

    inline bucket_type* get_bucket(const Key& k)
    {
        return buckets_ + (hasher_(k) % numbuckets_);
    }

private:
    // noncopyable
    unordered_map(const unordered_map&);
    unordered_map& operator=(const unordered_map&);
};

}

#endif // taapp_UNORDERED_MAP_H_

