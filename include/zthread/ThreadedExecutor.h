/*
 *  ZThreads, a platform-independant, multithreading and 
 *  synchroniation library
 *
 *  Copyright (C) 2000-2002, Eric Crahen, See LGPL.TXT for details
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

#ifndef __ZTTHREADEDEXECUTOR_H__
#define __ZTTHREADEDEXECUTOR_H__

#include "zthread/Executor.h"
#include "zthread/Guard.h"
#include "zthread/Singleton.h"
#include "zthread/Mutex.h"

namespace ZThread {


  /**
   * @class ThreadedExecutor
   *
   * @author Eric Crahen <crahen@cse.buffalo.edu>
   * @date <2003-07-07T22:20:19-0400>
   * @version 2.3.0
   *
   * A ThreadedExecutor runs each task in a different thread.
   *
   * Submitting a NullTask will allow you to wait() for all real tasks 
   * being executed to complete; and not just to be serviced (started).
   *
   * @see Executor.
   */
  template <
    class LockType = Mutex
    >
    class ThreadedExecutor : public Executor {
  
      //! Serialize access
      LockType _lock;

      //! Cancellation flag
      bool _canceled;

      public:

      //! Create a new ThreadedExecutor
      ThreadedExecutor() : _canceled(false) {}


      //! Destroy a ThreadedExecutor
      virtual ~ThreadedExecutor() {}

      /**
       * Submit a light wieght task to an Executor. This will not
       * block the calling thread very long. The submitted task will
       * be executed by another thread.
       * 
       * @exception Cancellation_Exception thrown if a task is submited when 
       * the executor has been canceled.
       * @exception Synchronization_Exception thrown is some other error occurs.
       *
       * @see Executor::execute(RunnableHandle&)
       */
      virtual void execute(const Task& task) {
     
        // Canceled Executors will not accept new tasks, quick 
        // check to avoid excessive locking in the canceled state
        if(_canceled) 
          throw Cancellation_Exception();

        Guard<LockType> g(_lock);

        if(_canceled) // Double check
          throw Cancellation_Exception();

        Thread(task);

      }  

      /**
       * @see Executor::cancel()
       */
      virtual void cancel() {
      
        Guard<LockType> g(_lock);
        _canceled = true;
    
      }
  
      /**
       * @see Executor::isCanceled()
       */
      virtual bool isCanceled() {
    
        Guard<LockType> g(_lock);
        return _canceled;

      }
 
      /**
       * Since a ThreadedExecutor starts a new Thread for 
       * each task, tasks are always being serviced and
       * there is nothing to wait for.
       *
       * @see Executor::wait(unsigned long)
       */
      virtual void wait() {
  
        if(Thread::interrupted())
          throw Interrupted_Exception();

      }

      /**
       * Since a ThreadedExecutor starts a new Thread for 
       * each task, tasks are always being serviced and
       * there is nothing to wait for.
       *
       * @see Executor::wait(unsigned long)
       */
      virtual bool wait(unsigned long) { 

        if(Thread::interrupted())
          throw Interrupted_Exception();

        return true;

      }


    }; /* ThreadedExecutor */

} // namespace ZThread

#endif // __ZTTHREADEDEXECUTOR_H__
