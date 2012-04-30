/**
 * @brief     C++ pair template implemenation
 * @author    Thomas Atwood (tatwood.net)
 * @date      2010
 * @copyright unlicense / public domain
 ****************************************************************************/
#ifndef taapp_PAIR_H_
#define taapp_PAIR_H_

namespace taapp
{

/** 
 * @brief template for a pair struct required to satisfy container interfaces
 * @details This is a subset of std::pair. All programatically defined 
 * constructors have been eliminated in order to keep the pair a POD type if
 * possible.
 */
template <typename T, typename U> struct pair
{
    T first;
    U second;
};

}

#endif // taapp_PAIR_H_

