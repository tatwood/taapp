/**
 * @brief     C++ list container template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_LIST_H_
#define taapp_LIST_H_

#include <cstddef>
#include <cassert>

namespace taapp
{

/** 
 * @brief double ended linked list.
 * @details This class is a subset of std::list. It is implemented using an
 * anchor node rather than head and tail pointers to avoid branches when
 * inserting and erasing items. This means that none of the next and prev
 * pointers will ever be NULL, and iterators should always be checked against
 * the results of end() to detect the end of the list. No size() functionality
 * is provided, as std::list<T>::size() often has O(N) complexity and
 * generally should be avoided when writing performance sensitive code.
 */
template<typename T, typename Allocator> class list
{
public:

    class iterator
    {
    public:

        inline iterator()
        {
        }

        inline iterator(const iterator& itr) : node_(itr.node_)
        {
        }

        inline operator T&()
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
            return *this;
        }

        inline iterator& operator--()
        {
            node_ = node_->node.tprev;
            return *this;
        }

        inline iterator& operator++()
        {
            node_ = node_->node.tnext;
            return *this;
        }

        inline T& operator*()
        {
            return node_->value;
        }

        inline T* operator->()
        {
            return &node_->value;
        }

    private:
        struct list::tnode* node_;

        inline iterator(struct list::tnode* node) : node_(node)
        {
        }

        friend class const_iterator;
        friend class list;
    };

    class const_iterator
    {
    public:

        inline const_iterator()
        {
        }

        inline const_iterator(const const_iterator& itr) : node_(itr.node_)
        {
        }

        inline const_iterator(const iterator& itr) : node_(itr.node_)
        {
        }

        inline operator const T&() const
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
            return *this;
        }

        inline const_iterator& operator=(const iterator& itr)
        {
            node_ = itr.node_;
            return *this;
        }

        inline const_iterator& operator--()
        {
            node_ = node_->node.tprev;
            return *this;
        }

        inline const_iterator& operator++()
        {
            node_ = node_->node.tnext;
            return *this;
        }

        inline const T& operator*()
        {
            return node_->value;
        }

        inline const T* operator->()
        {
            return &node_->value;
        }

    private:
        struct list::tnode* node_;

        inline const_iterator(struct list::tnode* node) : node_(node)
        {
        }

        friend class list;
    };

    list()
    {
        anchor_.aprev = &anchor_;
        anchor_.anext = &anchor_;
    }

    ~list()
    {
        clear();
    }

    inline const T& back() const
    {
        return anchor_.tprev->value;
    }

    inline T& back()
    {
        return anchor_.tprev->value;
    }

    inline const_iterator begin() const
    {
        return const_iterator(anchor_.tnext);
    }

    inline iterator begin()
    {
        return iterator(anchor_.tnext);
    }

    inline void clear()
    {
        tnode* n = anchor_.tnext;
        while(static_cast<void*>(n) != static_cast<void*>(&anchor_))
        {
            tnode* next = n->node.tnext;
            n->value.~T();
            allocator_.deallocate(n, 1);
            n = next;
        }
    }

    inline bool empty() const
    {
        return anchor_.anext == &anchor_;
    }

    inline const_iterator end() const
    {
        return const_iterator(reinterpret_cast<const tnode*>(&anchor_));
    }

    inline iterator end()
    {
        return iterator(reinterpret_cast<tnode*>(&anchor_));
    }

    inline iterator erase(iterator pos)
    {
        tnode* n = pos.node_;
        ++pos;
        n->node.aprev->anext = n->node.anext;
        n->node.anext->aprev = n->node.aprev;
        n->value.~T();
        allocator_.deallocate(n, 1);
        return pos;
    }

    inline const T& front() const
    {
        return anchor_.tnext->value;
    }

    inline T& front()
    {
        return anchor_.tnext->value;
    }

    /**
     * @brief inserts an item at the specified position.
     * @details the item that previously occupied the position will be placed
     * after the new item.
     */
    iterator insert(iterator pos, const T& t)
    {
        tnode* n = allocator_.allocate(1);
        tnode* p = pos.node_;
        new(static_cast<void*>(&n->value)) constructor(t);
        n->node.aprev = p->node.aprev;
        n->node.anext = &p->node;
        p->node.aprev->anext = &n->node;
        p->node.aprev = &n->node;
        return iterator(n);
    }

    inline void pop_back()
    {
        tnode* n = anchor_.tprev;
        n->node.aprev->anext = &anchor_;
        anchor_.aprev = n->node.aprev;
        n->value.~T();
        allocator_.deallocate(n, 1);
    }

    inline void pop_front()
    {
        tnode* n = anchor_.tnext;
        n->node.anext->aprev = &anchor_;
        anchor_.anext = n->node.anext;
        n->value.~T();
        allocator_.deallocate(n, 1);
    }

    inline void push_back(const T& t)
    {
        tnode* n = allocator_.allocate(1);
        new(static_cast<void*>(&n->value)) constructor(t);
        n->node.aprev = anchor_.aprev;
        n->node.anext = &anchor_;
        anchor_.aprev->anext = &n->node;
        anchor_.aprev = &n->node;
    }

    inline void push_front(const T& t)
    {
        tnode* n = allocator_.allocate(1);
        new(static_cast<void*>(&n->value)) constructor(t);
        n->node.anext = anchor_.anext;
        n->node.aprev = &anchor_;
        anchor_.anext->aprev = &n->node;
        anchor_.anext = &n->node;
    }

    /**
     * @details undefined behavior if this allocator != other allocator
     */
    void splice(iterator pos, list& other)
    {
        tnode* p = pos.node_;
        assert(allocator_ == other.allocator_);
        // insert the other list into this one (in front of pos)
        p->node.aprev->anext = other.anchor_.anext;
        other.anchor_.anext->aprev = p->node.aprev;
        p->node.aprev = other.anchor_.aprev;
        other.anchor_.aprev->anext = &p->node;
        // clear the other list
        other.anchor_.aprev = &other.anchor_;
        other.anchor_.anext = &other.anchor_;
    }

private:

    struct anode
    {
        union
        {
            struct
            {
                struct list::anode* aprev;
                struct list::anode* anext;
            };
            struct
            {
                struct list::tnode* tprev;
                struct list::tnode* tnext;
            };
        };
    };

    struct tnode
    {
        anode node;
        T value;
    };

    typedef typename Allocator::template rebind<tnode>::other allocator_type;
    typedef int OffsetTest[(offsetof(tnode, node) == 0) * 2 - 1];

    // define a custom placement new operator to remove dependency on
    // the std <new> header. cannot use allocator version because it expects
    // type tnode. could add constructor to tnode that accepts T,
    // but want tnode to remain POD if possible; so construction is done here
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

    anode anchor_;
    allocator_type allocator_;

private:
    // noncopyable
    list(const list&);
    list& operator=(const list&);
};

}

#endif // _TAAPP_LIST_H_
