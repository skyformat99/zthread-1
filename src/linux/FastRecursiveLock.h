/*
 *  ZThreads, a platform-independent, multi-threading and 
 *  synchronization library
 *
 *  Copyright (C) 2000-2003 Eric Crahen, See LGPL.TXT for details
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef __ZTFASTRECURSIVELOCK_H__
#define __ZTFASTRECURSIVELOCK_H__

#include "zthread/NonCopyable.h"
#include <pthread.h>

namespace ZThread {

/**
 * @class FastRecursiveLock
 *
 * @author Eric Crahen <crahen@cse.buffalo.edu>
 * @date <2003-07-16T23:27:14-0400>
 * @version 2.2.0
 *
 * This implementation of a FastRecursiveLock uses the recursive mutex
 * that linux pthreads provides.  
 */ 
class FastRecursiveLock : private NonCopyable {
    
  pthread_mutex_t _mtx;
  
public:
  
  inline FastRecursiveLock() {
    
    static const pthread_mutexattr_t attr = { PTHREAD_MUTEX_RECURSIVE_NP };
    pthread_mutex_init(&_mtx, &attr);

  }
  
  inline ~FastRecursiveLock() {

    pthread_mutex_destroy(&_mtx);

  }
  
  inline void acquire() {
    
    pthread_mutex_lock(&_mtx);

  }

  inline void release() {
    
    pthread_mutex_unlock(&_mtx);
    
  }
  
  inline bool tryAcquire(unsigned long timeout=0) {

    return (pthread_mutex_trylock(&_mtx) == 0);

  }
  
}; /* FastRecursiveLock */


} // namespace ZThread

#endif
