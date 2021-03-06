/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                       Paraver Main Computing Library                      *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/


#ifndef PLAINTRACE_H_INCLUDED
#define PLAINTRACE_H_INCLUDED

#include "memorytrace.h"
#include "index.h"
#include "plaintypes.h"

class ProcessModel;
class ResourceModel;

namespace Plain
{
  class PlainBlocks;

  struct ltrecord
  {
    bool operator()( TRecord *r1, TRecord *r2 ) const
    {
      if ( r1->time < r2->time )
        return true;
      if ( getTypeOrdered( r1 ) < getTypeOrdered( r2 ) )
        return true;
      return false;
    }
  };

  class PlainTrace: public MemoryTrace
  {
    public:
    class iterator: public MemoryTrace::iterator
      {
        public:
          iterator()
          {}

          iterator( PlainBlocks *whichBlocks, const Trace *whichTrace );

          virtual ~iterator()
          {}

          virtual void operator++();
          virtual void operator--();
          virtual MemoryTrace::iterator& operator=( const MemoryTrace::iterator& copy )
          {
            return *this;
          }

          virtual iterator *clone() const;

          virtual TRecordType    getType() const;
          virtual TRecordTime    getTime() const;
          virtual TThreadOrder   getThread() const;
          virtual TCPUOrder      getCPU() const;
          virtual TObjectOrder   getOrder() const;
          virtual TEventType     getEventType() const;
          virtual TSemanticValue getEventValue() const;
          virtual TEventValue    getEventValueAsIs() const;
          virtual TState         getState() const;
          virtual TRecordTime    getStateEndTime() const;
          virtual TCommID        getCommIndex() const;

          virtual void           setTime( const TRecordTime whichTime );
          virtual void           setType( const TRecordType whichType );
          virtual void           setStateEndTime( const TRecordTime whichEndTime );

        protected:
          PlainBlocks *blocks;
      };

    class ThreadIterator : public PlainTrace::iterator
      {
        public:
          ThreadIterator()
          {}

          ThreadIterator( PlainBlocks *whichBlocks, const Trace *whichTrace, PRV_UINT32 whichBlock, PRV_UINT32 whichPos,
                          TThreadOrder whichThread );

          virtual ~ThreadIterator()
          {}

          virtual TThreadOrder getThread() const;
          virtual TObjectOrder getOrder() const;

          virtual void operator++();
          virtual void operator--();
          virtual MemoryTrace::iterator& operator=( const MemoryTrace::iterator& copy );

          virtual ThreadIterator *clone() const;
        private:
          TThreadOrder thread;
          PRV_UINT32 block;
          PRV_UINT32 pos;
          PRV_UINT32 lastBlock;
          PRV_UINT32 lastPos;

      };

    class CPUIterator : public PlainTrace::iterator
      {
        public:
          CPUIterator()
          {}

          CPUIterator( PlainBlocks *whichBlocks, const Trace *whichTrace , std::vector<PRV_UINT32>& whichBlock, std::vector<PRV_UINT32>& whichPos,
                       TThreadOrder whichNumThreads, std::vector<TThreadOrder>& whichThreads, TCPUOrder whichCPU );

          virtual ~CPUIterator();

          virtual TThreadOrder getThread() const;
          virtual TObjectOrder getOrder() const;

          virtual void operator++();
          virtual void operator--();
          virtual MemoryTrace::iterator& operator=( const MemoryTrace::iterator& copy );

          virtual CPUIterator *clone() const;
        private:
          TCPUOrder cpu;
          TThreadOrder numThreads;
          std::vector<TThreadOrder> threads;
          std::vector<PRV_UINT32> block;
          std::vector<PRV_UINT32> pos;
          std::vector<PRV_UINT32> lastBlock;
          std::vector<PRV_UINT32> lastPos;
          TThreadOrder lastThread;

          TThreadOrder minThread();
          TThreadOrder maxThread();
          void setToMyCPUForward();
          void setToMyCPUBackward();
      };

      PlainTrace( const Trace *whichTrace,
                  const ProcessModel& whichProcessModel,
                  const ResourceModel& whichResourceModel );
      virtual ~PlainTrace()
      {}

      virtual void insert( MemoryBlocks *blocks );
      virtual TTime finish( TTime headerTime, Trace *whichTrace );
      virtual MemoryTrace::iterator* empty() const;
      virtual MemoryTrace::iterator* begin() const;
      virtual MemoryTrace::iterator* end() const;

      virtual MemoryTrace::iterator* threadBegin( TThreadOrder whichThread ) const;
      virtual MemoryTrace::iterator* threadEnd( TThreadOrder whichThread ) const;
      virtual MemoryTrace::iterator* CPUBegin( TCPUOrder whichCPU ) const;
      virtual MemoryTrace::iterator* CPUEnd( TCPUOrder whichCPU ) const;


      virtual void getRecordByTimeThread( std::vector<MemoryTrace::iterator *>& listIter,
                                          TRecordTime whichTime ) const;
      virtual void getRecordByTimeCPU( std::vector<MemoryTrace::iterator *>& listIter,
                                       TRecordTime whichTime ) const;

    protected:

    private:
      const Trace *myTrace;
      const ProcessModel& processModel;
      const ResourceModel& resourceModel;
      TThreadOrder numThreads;
      TCPUOrder numCPUs;
      std::vector<Index<std::pair<PRV_UINT32, PRV_UINT32> > > traceIndex;
      PlainBlocks *myBlocks;
  };
}

#endif // PLAINTRACE_H_INCLUDED
