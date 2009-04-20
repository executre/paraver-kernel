#include <string>
#include <sstream>
#include <iostream>
#include "tracebodyio_v2.h"

using namespace std;

bool TraceBodyIO_v2::ordered() const
{
  return true;
}

void TraceBodyIO_v2::read( TraceStream *file, MemoryBlocks& records,
                           hash_set<TEventType>& events ) const
{
  string line;

  file->getline( line );

  if ( line[0] == '#' || line.size() == 0 )
    return;

  switch ( line[0] )
  {
    case StateBeginRecord:
    case StateEndRecord:
      readState( line, records );
      break;

    case EventRecord:
      readEvent( line, records, events );
      break;

    case CommRecord:
    case LogicalSendRecord:
    case LogicalRecvRecord:
    case PhysicalSendRecord:
    case PhysicalRecvRecord:
      readComm( line, records );
      break;

    case GlobalCommRecord:
      //readGlobalComm( line, records );
      break;

    default:
      cerr << "Falta sistema de logging TraceBodyIO_v2::read()" << endl;
      cerr << "Tipo de record desconocido" << endl;
      break;
  };
}


void TraceBodyIO_v2::write( fstream& whichStream,
                            const KTrace& whichTrace,
                            MemoryTrace::iterator *record ) const
{
  string line;
  bool writeReady;
  TRecordType type = record->getType();

  if ( type == EMPTYREC )
    return;
  else if ( type & STATE )
    writeReady = writeState( line, whichTrace, record );
  else if ( type & EVENT )
    writeReady = writeEvent( line, whichTrace, record );
  else if ( type & COMM )
    writeReady = writeCommRecord( line, whichTrace, record );
  else if ( type & GLOBCOMM )
    writeReady = writeGlobalComm( line, whichTrace, record );
  else if ( type & RSEND || type & RRECV )
    writeReady = false;
  else
  {
    writeReady = false;
    cerr << "Falta sistema de logging TraceBodyIO_v2::write()" << endl;
    cerr << "Tipo de record desconocido en memoria" << endl;
  }

  if ( !writeReady )
    return;

  whichStream << line << endl;
}


void TraceBodyIO_v2::writeEvents( fstream& whichStream,
                                  const KTrace& whichTrace,
                                  vector<MemoryTrace::iterator *>& recordList ) const
{
  string line;

  for ( UINT16 i = 0; i < recordList.size(); i++ )
  {
    if ( i > 0 )
    {
      line += ':';
      writeEvent( line, whichTrace, recordList[i], false );
    }
    else // i == 0
      writeEvent( line, whichTrace, recordList[i], true );
  }

  whichStream << line << endl;
}


void TraceBodyIO_v2::writeCommInfo( fstream& whichStream,
                                    const KTrace& whichTrace ) const
{
  for ( TCommID id = 0; id < whichTrace.getTotalComms(); ++id )
  {
    ostringstream ostr;
    ostr << fixed;
    ostr << dec;
    ostr.precision( 0 );

    ostr << CommRecord << ':';
    if ( whichTrace.existResourceInfo() )
      ostr << whichTrace.getSenderCPU( id ) + 1 << ':';
    else
      ostr << '0' << ':';
    ostr << whichTrace.getSenderThread( id ) + 1 << ':';
    ostr << whichTrace.getLogicalSend( id ) << ':';
    ostr << whichTrace.getPhysicalSend( id ) << ':';
    if ( whichTrace.existResourceInfo() )
      ostr << whichTrace.getReceiverCPU( id ) + 1 << ':';
    else
      ostr << '0' << ':';
    ostr << whichTrace.getReceiverThread( id ) + 1 << ':';
    ostr << whichTrace.getLogicalReceive( id ) << ':';
    ostr << whichTrace.getPhysicalReceive( id ) << ':';
    ostr << whichTrace.getCommSize( id ) << ':';
    ostr << whichTrace.getCommTag( id );

    whichStream << ostr.str();
    whichStream << endl;
  }
}


/**********************
  Read line functions
***********************/
void TraceBodyIO_v2::readState( const string& line, MemoryBlocks& records ) const
{
  string tmpstring;
  TCPUOrder CPU;
  TThreadOrder thread;
  TRecordTime time;
  TRecordTime endtime;
  TState state;

  istringstream strLine( line );

  // Discarding record type
  std::getline( strLine, tmpstring, ':' );

  // Read the common info
  if ( !readCommon( strLine, CPU, thread, time ) )
  {
    cerr << "Falta sistema de logging TraceBodyIO_v2::readState()" << endl;
    cerr << "Error leyendo record de estado" << endl;
    cerr << line << endl;
    return;
  }

  std::getline( strLine, tmpstring, ':' );
  istringstream endtimeStream( tmpstring );
  if ( !( endtimeStream >> endtime ) )
  {
    cerr << "Falta sistema de logging TraceBodyIO_v2::readState()" << endl;
    cerr << "Error leyendo record de estado" << endl;
    cerr << line << endl;
    return;
  }

  std::getline( strLine, tmpstring );
  istringstream stateStream( tmpstring );
  if ( !( stateStream >> state ) )
  {
    cerr << "Falta sistema de logging TraceBodyIO_v2::readState()" << endl;
    cerr << "Error leyendo record de estado" << endl;
    cerr << line << endl;
    return;
  }

  if ( time == endtime ) return;

  records.newRecord();
  if ( line[0] == StateBeginRecord )
    records.setType( STATE + BEGIN );
  else
    records.setType( STATE + END );
  records.setTime( time );
  if ( CPU == 0 )
    records.setCPU( CPU );
  else
    records.setCPU( CPU - 1 );
  records.setThread( thread - 1 );
  records.setState( state );
  records.setStateEndTime( endtime );
}


void TraceBodyIO_v2::readEvent( const string& line, MemoryBlocks& records,
                                hash_set<TEventType>& events ) const
{
  string tmpstring;
  TCPUOrder CPU;
  TThreadOrder thread;
  TRecordTime time;
  TEventType eventtype;
  TEventValue eventvalue;

  istringstream strLine( line );

  // Discarding record type
  std::getline( strLine, tmpstring, ':' );

  // Read the common info
  if ( !readCommon( strLine, CPU, thread, time ) )
  {
    cerr << "Falta sistema de logging TraceBodyIO_v2::readEvent()" << endl;
    cerr << "Error leyendo record de evento" << endl;
    cerr << line << endl;
    return;
  }

  while ( !strLine.eof() )
  {
    std::getline( strLine, tmpstring, ':' );
    istringstream eventtypeStream( tmpstring );
    if ( !( eventtypeStream >> eventtype ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readEvent()" << endl;
      cerr << "Error leyendo record de evento" << endl;
      cerr << line << endl;
      return;
    }

    std::getline( strLine, tmpstring, ':' );
    istringstream eventvalueStream( tmpstring );
    if ( !( eventvalueStream >> eventvalue ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readEvent()" << endl;
      cerr << "Error leyendo record de evento" << endl;
      cerr << line << endl;
      return;
    }

    records.newRecord();
    records.setType( EVENT );
    records.setTime( time );
    if ( CPU == 0 )
      records.setCPU( CPU );
    else
      records.setCPU( CPU - 1 );
    records.setThread( thread - 1 );
    records.setEventType( eventtype );
    records.setEventValue( eventvalue );

    events.insert( eventtype );
  }
}


void TraceBodyIO_v2::readComm( const string& line, MemoryBlocks& records ) const
{
  string tmpstring;
  TCPUOrder CPU;
  TThreadOrder thread;
  TRecordTime logSend;
  TRecordTime phySend;
  TRecordTime logReceive;
  TRecordTime phyReceive;
  TCPUOrder remoteCPU;
  TThreadOrder remoteThread;
  TCommSize size;
  TCommTag tag;
  TCommID commid;

  istringstream strLine( line );

  // Discarding record type
  std::getline( strLine, tmpstring, ':' );

  if ( line[0] == CommRecord )
  {
    // Read the common info
    if ( !readCommon( strLine, CPU, thread, logSend ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de communicacion" << endl;
      cerr << line << endl;
      return;
    }

    std::getline( strLine, tmpstring, ':' );
    istringstream phySendStream( tmpstring );
    if ( !( phySendStream >> phySend ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de comunicacion" << endl;
      cerr << line << endl;
      return;
    }

    if ( !readCommon( strLine, remoteCPU, remoteThread,
                      logReceive ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de comunicacion" << endl;
      cerr << line << endl;
      return;
    }

    std::getline( strLine, tmpstring, ':' );
    istringstream phyReceiveStream( tmpstring );
    if ( !( phyReceiveStream >> phyReceive ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de comunicacion" << endl;
      cerr << line << endl;
      return;
    }

    std::getline( strLine, tmpstring, ':' );
    istringstream sizeStream( tmpstring );
    if ( !( sizeStream >> size ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de comunicacion" << endl;
      cerr << line << endl;
      return;
    }

    std::getline( strLine, tmpstring, ':' );
    istringstream tagStream( tmpstring );
    if ( !( tagStream >> tag ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de comunicacion" << endl;
      cerr << line << endl;
      return;
    }

    records.newComm( false );
    if ( CPU == 0 )
      records.setSenderCPU( CPU );
    else
      records.setSenderCPU( CPU - 1 );
    records.setSenderThread( thread - 1 );
    if ( remoteCPU == 0 )
      records.setReceiverCPU( remoteCPU );
    else
      records.setReceiverCPU( remoteCPU - 1 );
    records.setReceiverThread( remoteThread - 1 );
    records.setLogicalSend( logSend );
    records.setPhysicalSend( phySend );
    records.setLogicalReceive( logReceive );
    records.setPhysicalReceive( phyReceive );
    records.setCommSize( size );
    records.setCommTag( tag );
  }
  else
  {
    std::getline( strLine, tmpstring, ':' );
    istringstream commidStream( tmpstring );
    if ( !( commidStream >> commid ) )
    {
      cerr << "Falta sistema de logging TraceBodyIO_v2::readComm()" << endl;
      cerr << "Error leyendo record de comunicacion" << endl;
      cerr << line << endl;
      return;
    }

    records.newRecord();
    switch ( line[0] )
    {
      case LogicalSendRecord:
        records.setType( COMM + LOG + SEND );
        records.setTime( records.getLogicalSend( commid ) );
        records.setCPU( records.getSenderCPU( commid ) );
        records.setThread( records.getSenderThread( commid ) );
        break;
      case LogicalRecvRecord:
        records.setType( COMM + LOG + RECV );
        records.setTime( records.getLogicalReceive( commid ) );
        records.setCPU( records.getReceiverCPU( commid ) );
        records.setThread( records.getReceiverThread( commid ) );
        break;
      case PhysicalSendRecord:
        records.setType( COMM + PHY + SEND );
        records.setTime( records.getPhysicalSend( commid ) );
        records.setCPU( records.getSenderCPU( commid ) );
        records.setThread( records.getSenderThread( commid ) );
        break;
      case PhysicalRecvRecord:
        records.setType( COMM + PHY + RECV );
        records.setTime( records.getPhysicalReceive( commid ) );
        records.setCPU( records.getReceiverCPU( commid ) );
        records.setThread( records.getReceiverThread( commid ) );
        break;
    }
    records.setCommIndex( commid );

    records.newRecord();
    switch ( line[0] )
    {
      case LogicalSendRecord:
        records.setType( RSEND + LOG );
        records.setTime( records.getLogicalSend( commid ) );
        records.setCPU( records.getReceiverCPU( commid ) );
        records.setThread( records.getReceiverThread( commid ) );
        break;
      case LogicalRecvRecord:
        records.setType( RRECV + LOG );
        records.setTime( records.getLogicalReceive( commid ) );
        records.setCPU( records.getSenderCPU( commid ) );
        records.setThread( records.getSenderThread( commid ) );
        break;
      case PhysicalSendRecord:
        records.setType( RSEND + PHY );
        records.setTime( records.getPhysicalSend( commid ) );
        records.setCPU( records.getReceiverCPU( commid ) );
        records.setThread( records.getReceiverThread( commid ) );
        break;
      case PhysicalRecvRecord:
        records.setType( RRECV + PHY );
        records.setTime( records.getPhysicalReceive( commid ) );
        records.setCPU( records.getSenderCPU( commid ) );
        records.setThread( records.getSenderThread( commid ) );
        break;
    }
    records.setCommIndex( commid );
  }
}


void TraceBodyIO_v2::readGlobalComm( const string& line, MemoryBlocks& records ) const
{}


bool TraceBodyIO_v2::readCommon( istringstream& line,
                                 TCPUOrder& CPU,
                                 TThreadOrder& thread,
                                 TRecordTime& time ) const
{
  string tmpstring;

  std::getline( line, tmpstring, ':' );
  istringstream CPUStream( tmpstring );
  if ( !( CPUStream >> CPU ) )
  {
    return false;
  }

  std::getline( line, tmpstring, ':' );
  istringstream threadStream( tmpstring );
  if ( !( threadStream >> thread ) )
  {
    return false;
  }

  std::getline( line, tmpstring, ':' );
  istringstream timeStream( tmpstring );
  if ( !( timeStream >> time ) )
  {
    return false;
  }

  return true;
}


/**************************
  Write records functions
***************************/
bool TraceBodyIO_v2::writeState( string& line,
                                 const KTrace& whichTrace,
                                 MemoryTrace::iterator *record ) const
{
  ostringstream ostr;
  ostr << fixed;
  ostr << dec;
  ostr.precision( 0 );

  if ( record->getType() == ( STATE + BEGIN ) )
    ostr << StateBeginRecord << ':';
  else if ( record->getType() == ( STATE + END ) )
    ostr << StateEndRecord << ':';
  writeCommon( ostr, whichTrace, record );
  ostr << record->getStateEndTime() << ':' << record->getState();

  line += ostr.str();
  return true;
}


bool TraceBodyIO_v2::writeEvent( string& line,
                                 const KTrace& whichTrace,
                                 MemoryTrace::iterator *record,
                                 bool needCommons ) const
{
  TRecordType firstType;
  TRecordTime firstTime;
  TThreadOrder firstThread;
  ostringstream ostr;
  ostr << fixed;
  ostr << dec;
  ostr.precision( 0 );

  if ( needCommons )
  {
    ostr << EventRecord << ':';
    writeCommon( ostr, whichTrace, record );
  }
  ostr << record->getEventType() << ':' << record->getEventValue();
  firstType = record->getType();
  firstTime = record->getTime();
  firstThread = record->getThread();
  ++(*record);
  while( !record->isNull() && record->getType() == firstType &&
         record->getTime() == firstTime && record->getThread() == firstThread )
  {
    ostr << ':' << record->getEventType() << ':' << record->getEventValue();
    ++(*record);
  }
  if( !record->isNull() )
    --(*record);

  line += ostr.str();
  return true;
}


bool TraceBodyIO_v2::writeCommRecord( string& line,
                                      const KTrace& whichTrace,
                                      MemoryTrace::iterator *record ) const
{
  ostringstream ostr;
  TCommID commID;
  TRecordType type;
  ostr << fixed;
  ostr << dec;
  ostr.precision( 0 );

  commID = record->getCommIndex();
  type = record->getType();

  if ( type == ( COMM + LOG + SEND ) )
    ostr << LogicalSendRecord << ':';
  else if ( type == ( COMM + LOG + RECV ) )
    ostr << LogicalRecvRecord << ':';
  else if ( type == ( COMM + PHY + SEND ) )
    ostr << PhysicalSendRecord << ':';
  else if ( type == ( COMM + PHY + RECV ) )
    ostr << PhysicalRecvRecord << ':';
  ostr << record->getCommIndex();

  line += ostr.str();
  return true;
}


bool TraceBodyIO_v2::writeGlobalComm( string& line,
                                      const KTrace& whichTrace,
                                      MemoryTrace::iterator *record ) const
{
  return true;
}


void TraceBodyIO_v2::writeCommon( ostringstream& line,
                                  const KTrace& whichTrace,
                                  MemoryTrace::iterator *record ) const
{
  if ( whichTrace.existResourceInfo() )
    line << record->getCPU() + 1 << ':';
  else
    line << '0' << ':';

  line << record->getThread() + 1 << ':';
  line << record->getTime() << ':';
}

