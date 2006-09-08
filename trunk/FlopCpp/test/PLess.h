#ifndef PLESS_H
#define PLESS_H

const char * const PLess_header = \
"$Id: PLess.h,v 1.2 2005/01/19 03:26:56 hpwalton Exp $";
/*
Editors:
================================================
harry		H. Philip Walton
*/

// $Log: PLess.h,v $
// Revision 1.2  2005/01/19 03:26:56  hpwalton
// *** empty log message ***
//
// Revision 1.1.1.1  2000/08/08 22:50:38  hpwalton
// initial add
//
// Revision 1.1.1.1  1997/07/10 20:53:59  harry
// Initial import
//
#include <functional>

/** 
    \ingroup PWUTILS 
 */
template <class T>
struct PLess : std::binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return *x < *y; }
};

#endif
