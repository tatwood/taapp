/**
 * @brief     C++ set container template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_SET_H_
#define taapp_SET_H_

#include "pair.h"
#include <cassert>
#include <cstddef>

namespace taapp
{

/*
   1. A node is either red or black.
   2. The root is black.
   3. All leaves are black.
   4. Both children of every red node are black.
   5. Every simple path from a given node to any of its descendant leaves
      contains the same number of black nodes.
*/
template<typename Key, typename Compare, typename Allocator>
class set
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

        inline operator Key&()
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

        inline iterator& operator++()
        {
            // first try to traverse down the right subtree
            rbnode* predeccessor = node_;
            rbnode* n = predeccessor->right;
            if(n != NULL)
            {
                // a right child exists, traverse down to it's leftmost value
                predeccessor = n->left;
                while(predeccessor != NULL)
                {
                    n = predeccessor;
                    predeccessor = n->left;
                }
            }
            else
            {
                // no right child exists, move up the tree
                n = predeccessor->parent;
                while(n != NULL)
                {
                    // move up until predeccessor is left child of its parent
                    if(n->left == predeccessor)
                    {
                        break;
                    }
                    predeccessor = n;
                    n = predeccessor->parent;
                }
            }
            node_ = n;
            return *this;
        }

        inline Key& operator*()
        {
            return node_->value;
        }

        inline Key* operator->()
        {
            return &node_->value;
        }

    private: 
        struct set::rbnode* node_;

        inline explicit iterator(struct set::rbnode* n) : node_(n)
        {
        }

        friend class const_iterator;
        friend class set;
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

        inline operator const Key&()
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

        inline const_iterator& operator++()
        {
            // first try to traverse down the right subtree
            rbnode* predeccessor = node_;
            rbnode* n = predeccessor->right;
            if(n != NULL)
            {
                // a right child exists, traverse down to it's leftmost value
                predeccessor = n->left;
                while(predeccessor != NULL)
                {
                    n = predeccessor;
                    predeccessor = n->left;
                }
            }
            else
            {
                // no right child exists, move up the tree
                n = predeccessor->parent;
                while(n != NULL)
                {
                    // move up until predeccessor is left child of its parent
                    if(n->left == predeccessor)
                    {
                        break;
                    }
                    predeccessor = n;
                    n = predeccessor->parent;
                }
            }
            node_ = n;
            return *this;
        }

        inline const Key& operator*()
        {
            return node_->value;
        }

        inline const Key* operator->()
        {
            return &node_->value;
        }

    private:
        struct set::rbnode* node_;

        inline explicit const_iterator(struct set::rbnode* n) : node_(n)
        {
        }

        friend class set;
    };

    set() : root_(0), size_(0)
    {
    }

    ~set()
    {
        clear();
    }

    const_iterator begin() const
    {
        rbnode* n = NULL;
        rbnode* l = root_;
        while(l != NULL)
        {
            n = l;
            l = n->left;
        }
        return const_iterator(n);
    }

    iterator begin()
    {
        rbnode* n = NULL;
        rbnode* l = root_;
        while(l != NULL)
        {
            n = l;
            l = n->left;
        }
        return iterator(n);
    }

    void clear()
    {
        rbnode* n = root_;
        while(n != NULL)
        {
            if(n->left != NULL)
            {
                n = n->left;
            }
            else if(n->right != NULL)
            {
                n = n->right;
            }
            else
            {
                // n is a leaf
                rbnode* parent = n->parent;
                n->value.~Key();
                allocator_.deallocate(n, 1);
                if(parent != NULL)
                {
                    if(parent->left == n)
                    {
                        parent->left = NULL;
                    }
                    else
                    {
                        parent->right = NULL;
                    }
                }
                n = parent;
            }
        }
        root_ = NULL;
        size_ = 0;
    }

    inline bool empty() const
    {
        return root_ == NULL;
    }

    inline const_iterator end() const
    {
        return const_iterator(NULL);
    }

    inline iterator end()
    {
        return iterator(NULL);
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

    iterator erase(iterator itr)
    {
        // standard BST deletion
        rbnode* n = itr.node_;
        ++itr;
        rbnode* root = n;
        rbnode* parent = root->parent;
        rbnode* child = root->left;
        bool color = root->color;
        if(child == NULL)
        {
            // the node does not have a left child
            child = root->right;
        }
        else if(root->right != NULL)
        {
            // the node has two children, so it must be swapped with its
            // predecessor or successor

            // find its successor
            rbnode* successor = root->right;
            if(successor->left != NULL)
            {
                do
                {
                    successor = successor->left;
                }
                while(successor->left != NULL);
            }
            replace_child(parent, root, successor);
            successor->left = child;
            child->parent = successor;
            child = successor->right;
            successor->right = root->right;
            root->right->parent = successor;
            parent = successor->parent;
            successor->parent = root->parent;
            color = successor->color;
            successor->color = root->color;
            root = successor;
        }
        replace_child(parent, root, child);
        if(child != NULL)
        {
            child->parent = parent;
        }

        // rebalancing
        if(color == BLACK)
        {
            if(is_red(child))
            {
                child->color = BLACK;
            }
            else
            {
                // case 1: if child is the tree parent, we are done
                while(parent != NULL)
                {
                    if(parent->left == child)
                    {
                        parent = balance_erase<LEFT>(parent, child);
                    }
                    else
                    {
                        parent = balance_erase<RIGHT>(parent, child);
                    }
                    child = parent;
                    parent = parent->parent;
                }
            }
        }

        // clean up
        --size_;
        if(root_ != NULL)
        {
            root_->color = BLACK;
        }
        n->value.~Key();
        allocator_.deallocate(n, 1);

        return itr;
    }

    iterator find(const Key& t)
    {
        rbnode* n = root_;
        while(n != NULL)
        {
            if(compare_(t, n->value))
            {
                n = n->left;
            }
            else if(compare_(n->value, t))
            {
                n = n->right;
            }
            else
            {
                break;
            }
        }
        return iterator(n);
    }

    inline pair<iterator, bool> insert(const Key& t)
    {
        rbnode* pos = root_;
        rbnode* n = NULL;

        // try to find a duplicate
        rbnode** child_link = &pos;
        while(*child_link != NULL)
        {
            pos = *child_link;
            if(compare_(t, pos->value))
            {
                child_link = &pos->left;
            }
            else if(compare_(pos->value, t))
            {
                child_link = &pos->right;
            }
            else
            {
                n = pos;
                break;
            }
        }

        if(n == NULL)
        {
            // need to insert a new value
            n = allocator_.allocate(1);
            new(static_cast<void*>(&n->value)) constructor(t);
            n->color = RED;
            n->left = NULL;
            n->right = NULL;

            if(root_ == NULL)
            {
                // special case: empty tree
                root_ = n;
                n->parent = NULL;
            }
            else
            {
               // standard BST insertion
                rbnode*  parent = pos;
                bool less = compare_(n->value, parent->value);
                child_link = less ? &parent->left : &parent->right;
                n->parent = parent;
                *child_link = n;

                // Walk back up the tree and re-balance
                rbnode* child = parent;
                parent = child->parent;
                // after the insert the grandchild of parent is red, so if the
                // child of parent is also red, there is a red violation
                while(parent != NULL && child->color == RED)
                {
                    // determine which direction we came from
                    if(parent->left == child)
                    {
                        parent = balance_insert<LEFT>(parent, child);
                    }
                    else
                    {
                        parent = balance_insert<RIGHT>(parent, child);
                    }
                    // move up to the next level
                    child = parent;
                    parent = child->parent;
                }
            }
            root_->color = BLACK;
            ++size_;
        }
        pair<iterator, bool> result = { iterator(n), n != pos };
        return result;
    }

    inline size_t size() const
    {
        return size_;
    }

#ifndef taapp_SET_INTERNAL_API
private:
#endif // taapp_SET_INTERNAL_API

    enum Colors
    {
        BLACK = false,
        RED = true
    };

    enum Directions
    {
        LEFT = false,
        RIGHT = true
    };

    // define a custom placement new operator to remove dependency on
    // the std <new> header. cannot use allocator version because it expects
    // type tnode. could add constructor to rbnode that accepts Key, but want
    // rbnode to remain POD if possible; so construction is done here
    class constructor
    {
    public:
        Key t_;

        inline constructor(const Key& t) : t_(t)
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
    
    struct rbnode
    {
        Key value;
        bool color;
        rbnode* parent;
        rbnode* left;
        rbnode* right;
    };   

    typedef typename Allocator::template rebind<rbnode>::other allocator_type;

    rbnode* root_;
    size_t size_;
    Compare compare_;
    allocator_type allocator_;

    template<bool Direction>
    rbnode* balance_erase(rbnode* root, rbnode* child)
    {
        enum { Opposite = !Direction };
        rbnode* parent = root;
        rbnode* sibling = get_child<Opposite>(root);

        if(sibling->color == RED) // sibling cannot be NULL
        {
            // case 2: sibling is red
            root = rotate<Direction>(root);
            sibling = get_child<Opposite>(parent);
        }
        if(sibling != NULL)
        {
            if (!is_red(get_child<Direction>(sibling)) && 
                !is_red(get_child<Opposite>(sibling)))
            {
                // case 3: sibling, and both children of sibling are black
                sibling->color = RED;
                if(parent->color == RED)
                {
                    // case 4:
                    // parent is red, sibling and both its children are black
                    parent->color = BLACK;
                    root = root_; // done
                }
            }
            else
            {
                // case 5 & 6
                bool color = parent->color;
                if(is_red(get_child<Opposite>(sibling)))
                {
                    parent = rotate<Direction>(parent);
                }
                else
                {
                    parent = double_rotate<Direction>(parent);
                }
                parent->color = color;
                parent->left->color = BLACK;
                parent->right->color = BLACK;
                root = root_; // done
            }
        }

        return root;
    }

    template<bool Direction>
    rbnode* balance_insert(rbnode* root, rbnode* child)
    {
        enum { Opposite = !Direction };
        rbnode* sibling = get_child<Opposite>(root);
        if(is_red(sibling))
        {
            // both children of root are red, so do simple color change
            root->color = RED;
            child->color = BLACK;
            sibling->color = BLACK;
        }
        else if(is_red(get_child<Direction>(child)))
        {
            root = rotate<Opposite>(root);
        }
        else if(is_red(get_child<Opposite>(child)))
        {
            root = double_rotate<Opposite>(root);
        }
        return root;
    }

    template<bool Direction>
    inline rbnode* double_rotate(rbnode* root)
    {
        enum { Opposite = !Direction };
        rbnode* other = get_child<Opposite>(root);
        rotate<Opposite>(other);
        return rotate<Direction>(root);
    }
    
    template<bool Direction>
    inline rbnode* get_child(rbnode* p)
    {
        return (Direction == LEFT) ? p->left : p->right;
    }
    
    inline bool is_red(rbnode* n)
    {
        return n != NULL && n->color == RED;
    } 

    inline void replace_child(rbnode* root, rbnode* child, rbnode* new_child)
    {
        if(root != NULL)
        {
            if(root->left == child)
            {
                root->left = new_child;
            }
            else
            {
                root->right = new_child;
            }
        }
        else
        {
            root_ = new_child;
        }
    }

    template<bool Direction>
    rbnode* rotate(rbnode* root)
    {
        enum { Opposite = !Direction };
        // relocate pivot at the original position of root
        rbnode* pivot = get_child<Opposite>(root);
        rbnode* gp = root->parent;
        pivot->parent = gp;
        replace_child(gp, root, pivot);

        // child of pivot becomes opposite child of root
        rbnode* pivot_child = get_child<Direction>(pivot);
        set_child<Opposite>(root, pivot_child);
        if(pivot_child != NULL)
        {
            pivot_child->parent = root;
        }

        // root becomes child of pivot
        set_child<Direction>(pivot, root);
        root->parent = pivot;

        root->color = RED;
        pivot->color = BLACK;

        return pivot;
    }

    template<bool Direction>
    void set_child(rbnode* root, rbnode* child)
    {
        if(Direction == LEFT)
        {
            root->left = child;
        }
        else
        {
            root->right = child;
        }
    }

private:
    // noncopyable
    set(const set&);
    set& operator=(const set&);
};

}

#endif // TAAPP_SET_H_

