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

#ifndef __ZTATOMICCOUNTIMPL_H__
#define __ZTATOMICCOUNTIMPL_H__

#include <windows.h>
#include <assert.h>

namespace ZThread {

typedef struct atomic_count_t {
  
  CRITICAL_SECTION cs;
  size_t count;

  atomic_count_t() : count(1) {}
  
} ATOMIC_COUNT;

AtomicCount::AtomicCount() {

  ATOMIC_COUNT* c = new ATOMIC_COUNT;
  _value = reinterpret_cast<void*>(c);
  ::InitializeCriticalSection(&c->cs);
  
}

AtomicCount::~AtomicCount() {

  ATOMIC_COUNT* c = reinterpret_cast<ATOMIC_COUNT*>(_value);
  assert(c->count == 0);  
  ::DeleteCriticalSection(&c->cs);
  delete c;

}

//! Postfix decrement and return the current value
size_t AtomicCount::operator--(int) {

  ATOMIC_COUNT* c = reinterpret_cast<ATOMIC_COUNT*>(_value);
  size_t value; 
  
  ::EnterCriticalSection(&c->cs);
  value = c->count--;
  ::LeaveCriticalSection(&c->cs);

  return value;

}
  
//! Postfix increment and return the current value
size_t AtomicCount::operator++(int) {

  ATOMIC_COUNT* c = reinterpret_cast<ATOMIC_COUNT*>(_value);
  size_t value; 
  
  ::EnterCriticalSection(&c->cs);
  value = c->count++;
  ::LeaveCriticalSection(&c->cs);

  return value;

}

//! Prefix decrement and return the current value
size_t AtomicCount::operator--() {

  ATOMIC_COUNT* c = reinterpret_cast<ATOMIC_COUNT*>(_value);
  size_t value; 
  
  ::EnterCriticalSection(&c->cs);
  value = --c->count;
  ::LeaveCriticalSection(&c->cs);

  return value;

}
  
//! Prefix increment and return the current value
size_t AtomicCount::operator++() {
  
  ATOMIC_COUNT* c = reinterpret_cast<ATOMIC_COUNT*>(_value);
  size_t value; 
  
  ::EnterCriticalSection(&c->cs);
  value = ++c->count;
  ::LeaveCriticalSection(&c->cs);

  return value;

}

};

#endif // __ZTATOMICCOUNTIMPL_H__
