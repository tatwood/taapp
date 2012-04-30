/**
 * @brief     C++ priority queue container template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_PRIORITY_QUEUE_H_
#define taapp_PRIORITY_QUEUE_H_

#include <cassert>
#include <cstddef>

namespace taapp
{

template<typename T, typename Container, typename Compare>
class priority_queue
{
public:

    priority_queue()
    {
    }

    inline bool empty() const
    {
        return container_.empty();
    }

    void pop()
    {
        assert(container_.size() > 0);
        T node(container_.back());
        container_.pop_back();
        if(container_.size() > 0)
        {
            ptrdiff_t index = 0;
            ptrdiff_t childindex = (index << 1) + 1;
            ptrdiff_t end = container_.size();

            T* child;
            T* rightchild;
            T* heap = container_.begin();
            // replace the item at the front of the heap
            // with the item at the end of the heap
            *heap = node;

            while(childindex < end)
            {
                child = heap + childindex;
                if(childindex + 1 < end)
                {
                    rightchild = heap + (childindex+1);
                    // choose the greater of either left child or right child
                    // if((*child) < (*rightchild))
                    if(compare_(*child, *rightchild))
                    {
                        ++childindex;
                        child = rightchild;
                    }
                }
                // if the node is less than the child, swap them
                // (moving the child closer to the front of the queue)
                // if(node < (*child))
                if(compare_(node, *child))
                {
                    heap[index] = *child;
                    heap[childindex] = node;

                    index = childindex;
                    childindex = (index << 1) + 1;
                }
                else
                {
                    break;
                }
            }
        }
    }

    void push(const T& n)
    {
        ptrdiff_t index = container_.size();
        ptrdiff_t parentindex = (index-1) >> 1;

        container_.push_back(n);
        T* heap = container_.begin();
        T* node = heap + index;

        T* parent;
        while(parentindex >= 0)
        {
            parent = heap + parentindex;
            // if the parent is less than node, swap them
            // (moving node closer to the front of the queue)
            // if((*parent) < (*node))
            if(compare_(*parent, *node))
            {
                *node = *parent;
                node = parent;
                *node = n;
                index = parentindex;
                parentindex = (index-1) >> 1;
            }
            else
            {
                break;
            }
        }
    }

    inline size_t size() const
    {
        return container_.size();
    }

    inline const T& top()
    {
        return container_.front();
    }

#ifndef taapp_PRIORITY_QUEUE_INTERNAL_API
private:
#endif // taapp_PRIORITY_QUEUE_INTERNAL_API

    Container container_;
    Compare compare_;

private:
    // noncopyable
    priority_queue(const priority_queue&);
    priority_queue& operator=(const priority_queue&);
};

}

#endif // taapp_PRIORITY_QUEUE_H_

