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

#include "ThreadOps.h"
#include "zthread/Runnable.h"
#include <process.h>

namespace ZThread {

const ThreadOps ThreadOps::INVALID(0); 

/**
 * Detect OS at runtime and attempt to locate the SwitchToThread
 * function, which will assist in making the spin lock implementation
 * which use ThreadOps::yield() a bit fairer.
 */
class YieldOps {

  typedef BOOL (*Yield)(void);
  Yield _fnYield;

public:

  YieldOps() : _fnYield(NULL) {

    OSVERSIONINFO v;
    v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(::GetVersionEx(&v) && (v.dwPlatformId == VER_PLATFORM_WIN32_NT)) {
    
       // Uses GetModuleHandle() so the reference count on the dll is
       // not affected. There is a warning about race conditions involving
       // this function being called as FreeLibrary() completes; however 
       // nearly all win32 applications load this particular and will keep it
       // in memory until the process exits.
       HINSTANCE hInst = ::GetModuleHandle("Kernel32.dll");
       if(hInst != NULL)
         _fnYield = (Yield)::GetProcAddress(hInst, "SwitchToThread");

       // REMIND: possibly need to use _T() macro for these strings
    }

  }

  bool operator()() {
  
    // Attempt to yield using the best function available
    if(!_fnYield || !_fnYield()) 
      ::Sleep(0);  

    return true;

  }

};

bool ThreadOps::join(ThreadOps* ops) {

  assert(ops);
  assert(ops->_tid != 0);
  assert(ops->_hThread != NULL);
  
  if(::WaitForSingleObjectEx(ops->_hThread, INFINITE, FALSE) != WAIT_OBJECT_0) 
    return false;

  ::CloseHandle(ops->_hThread);
  ops->_hThread = NULL;

  return true;

}

bool ThreadOps::yield() {

  static YieldOps yielder;

  yielder();

  return true;

}

bool ThreadOps::setPriority(ThreadOps* impl, Priority p) {

  assert(impl);
 
#if !defined(ZTHREAD_DISABLE_PRIORITY)

  bool result;
  
  // Convert
  int n;
  switch(p) {
  case Low:
    n = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case High:
    n = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case Medium:
  default:
    n = THREAD_PRIORITY_NORMAL;
  }

  
  result = (::SetThreadPriority(impl->_hThread, n) != THREAD_PRIORITY_ERROR_RETURN);
  return result;

#else

  return true;

#endif

}

bool ThreadOps::getPriority(ThreadOps* impl, Priority& p) {

  assert(impl);
  bool result = true;
  
#if !defined(ZTHREAD_DISABLE_PRIORITY)

  // Convert to one of the PRIORITY values
  switch(::GetThreadPriority(impl->_hThread)) {
    case THREAD_PRIORITY_ERROR_RETURN:
      result = false;
    case THREAD_PRIORITY_BELOW_NORMAL:
      p = Low;
      break;
    case THREAD_PRIORITY_ABOVE_NORMAL:
      p = High;
      break;
    case THREAD_PRIORITY_NORMAL:
    default:
      p = Medium;
  }
  
#endif

  return result;

}


bool ThreadOps::spawn(Runnable* task) {

// Start the thread.
#if defined(HAVE_BEGINTHREADEX)
  _hThread = (HANDLE)::_beginthreadex(0, 0, &_dispatch, task, 0, (unsigned int*)&_tid);
#else
  _hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&_dispatch, task, 0, (DWORD*)&_tid);
#endif

  return _hThread != NULL;

}

unsigned int __stdcall ThreadOps::_dispatch(void *arg) {

  Runnable* task = reinterpret_cast<Runnable*>(arg);
  assert(task);

  // Run the task from the correct context
  task->run();

  // Exit the thread
#if defined(HAVE_BEGINTHREADEX)
  ::_endthreadex(0);
#else
  ExitThread(0);
#endif

  return 0;
  
}

}
