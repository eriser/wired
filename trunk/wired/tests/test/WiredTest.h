// Copyright (C) 2004-2010 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WIREDTEST_H__
#define __WIREDTEST_H__

#include <iostream>

/**
 * Define all needs for unit testing
 */
#define TEST_ASSERT(x) \
    try { \
        if(!(x)) { \
            std::cerr << "Assert failed at " << __FILE__ << ":" << __LINE__ <<  "(" << __FUNCTION__ << ")" << std::endl; \
            exit(-1); \
        } \
    } \
    catch(...) { \
        std::cerr << "An exception was throw at " << __FILE__ << ":" << __LINE__ <<  "(" << __FUNCTION__ << ")" << std::endl; \
        exit(-1); \
    }

#endif // __WIREDTEST_H__
