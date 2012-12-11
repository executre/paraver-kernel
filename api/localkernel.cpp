/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                      API Library for libparaver-kernel                    *
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

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL$
 | @last_commit: $Date$
 | @version:     $Revision$
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#include <time.h>
#include <sys/stat.h>
#include "localkernel.h"
#include "kwindow.h"
#include "khistogram.h"
#include "histogramstatistic.h"
#include "semanticmanagement.h"
#include "filtermanagement.h"
#include "statisticmanagement.h"
#include "functionmanagement.h"
#include "krecordlist.h"
#include "kprogresscontroller.h"
#include "labelconstructor.h"
#include "previousfiles.h"


#include "ktraceoptions.h"
#include "ktracecutter.h"
#include "ktracefilter.h"
#include "ktracesoftwarecounters.h"
#include "tracestream.h"
#include <string.h>

#ifdef TRACING_ENABLED
#include "extrae_user_events.h"
#endif

using namespace std;

void LocalKernel::init()
{
  srand( time( NULL ) );

  createFilter();
  createSemantic();
  createStatistic();

  LabelConstructor::init();

#ifdef TRACING_ENABLED
  Extrae_init();
#endif
}

LocalKernel::LocalKernel( bool ( *messageFunction )( string ) ) :
    myMessageFunction( messageFunction )
{
  setPathSeparator( string( "/") ); // FIXME

  // FILTERS
  trace_names_table_last = 0;
}

LocalKernel::~LocalKernel()
{
//  deleted prevTracesNames;
#ifdef TRACING_ENABLED
  Extrae_fini();
#endif
}

bool LocalKernel::checkTraceSize( const string& filename, TTraceSize maxSize ) const
{
  TTraceSize size = TraceStream::getTraceFileSize( filename );

  if( size <= maxSize )
    return true;

  return false;
}

TTraceSize LocalKernel::getTraceSize( const string& filename ) const
{
  return TraceStream::getTraceFileSize( filename );
}

Trace *LocalKernel::newTrace( const string& whichFile, bool noLoad, ProgressController *progress ) const
{
  if ( progress == NULL )
    return new KTrace( whichFile, NULL, noLoad );

  return new KTrace( whichFile, ( KProgressController * ) progress->getConcrete(), noLoad );
}

string LocalKernel::getPCFFileLocation( const string& traceFile ) const
{
  string pcfFile;

  if ( traceFile.substr( traceFile.length() - 6 ) == "prv.gz" )
    pcfFile = traceFile.substr( 0, traceFile.length() - 6 );
  else
    pcfFile = traceFile.substr( 0, traceFile.length() - 3 );
  pcfFile.append( "pcf" );

  return pcfFile;
}

string LocalKernel::getROWFileLocation( const string& traceFile ) const
{
  string rowFile;

  if ( traceFile.substr( traceFile.length() - 6 ) == "prv.gz" )
    rowFile = traceFile.substr( 0, traceFile.length() - 6 );
  else
    rowFile = traceFile.substr( 0, traceFile.length() - 3 );
  rowFile.append( "row" );

  return rowFile;
}

Window *LocalKernel::newSingleWindow() const
{
  return new KSingleWindow();
}

Window *LocalKernel::newSingleWindow( Trace *whichTrace ) const
{
  return new KSingleWindow( ( KTrace * ) whichTrace->getConcrete() );
}

Window *LocalKernel::newDerivedWindow( Window *window1, Window * window2 ) const
{
  return new KDerivedWindow( ( KWindow * ) window1->getConcrete(),
                             ( KWindow * ) window2->getConcrete() );
}

Window *LocalKernel::newDerivedWindow() const
{
  return new KDerivedWindow();
}

Histogram *LocalKernel::newHistogram() const
{
  return new KHistogram();
}

/*RecordList *LocalKernel::newRecordList() const
{
  return new KRecordList();
}*/

ProgressController *LocalKernel::newProgressController() const
{
  return new KProgressController();
}

Filter *LocalKernel::newFilter( Filter *concreteFilter ) const
{
  FilterProxy *tmpFilter = new FilterProxy( this );
  tmpFilter->myFilter = concreteFilter;
  return ( Filter * ) tmpFilter;
}

string LocalKernel::getToolID( const string &toolName ) const
{
  string auxStr;

  if ( toolName == TraceCutter::getName() )
    auxStr = TraceCutter::getID();
  else if ( toolName == TraceFilter::getName() )
    auxStr = TraceFilter::getID();
  else if ( toolName == TraceSoftwareCounters::getName() )
    auxStr = TraceSoftwareCounters::getID();
  else
    auxStr = "";

  return auxStr;
}


std::string LocalKernel::getToolName( const string &toolID ) const
{
  string auxStr;

  if ( toolID == TraceCutter::getID() )
    auxStr = TraceCutter::getName();
  else if ( toolID == TraceFilter::getID() )
    auxStr = TraceFilter::getName();
  else if ( toolID == TraceSoftwareCounters::getID() )
    auxStr = TraceSoftwareCounters::getName();
  else
    auxStr = "";

  return auxStr;
}


TraceOptions *LocalKernel::newTraceOptions() const
{
  return new KTraceOptions( this );
}

TraceCutter *LocalKernel::newTraceCutter( char *trace_in,
                                          char *trace_out,
                                          TraceOptions *options,
                                          const vector< TEventType > &whichTypesWithValuesZero,
                                          ProgressController *progress ) const
{
  KProgressController *tmpKProgressControler = NULL;

  if ( progress != NULL )
    tmpKProgressControler = (KProgressController *)progress->getConcrete();

  return new KTraceCutter( trace_in, trace_out, options,  whichTypesWithValuesZero , tmpKProgressControler);
}


TraceFilter *LocalKernel::newTraceFilter( char *trace_in,
                                          char *trace_out,
                                          TraceOptions *options,
                                          const std::map< TTypeValuePair, TTypeValuePair >& whichTranslationTable,
                                          ProgressController *progress ) const
{
  KProgressController *tmpKProgressControler = NULL;

  if ( progress != NULL )
    tmpKProgressControler = (KProgressController *)progress->getConcrete();

  return new KTraceFilter( trace_in, trace_out, options, whichTranslationTable, tmpKProgressControler );
}


TraceSoftwareCounters *LocalKernel::newTraceSoftwareCounters( char *trace_in,
                                                              char *trace_out,
                                                              TraceOptions *options,
                                                              ProgressController *progress ) const
{
  KProgressController *tmpKProgressControler = NULL;

  if ( progress != NULL )
    tmpKProgressControler = (KProgressController *)progress->getConcrete();

  return new KTraceSoftwareCounters( trace_in, trace_out, options, tmpKProgressControler );
}

void LocalKernel::getAllStatistics( vector<string>& onVector ) const
{
  FunctionManagement<HistogramStatistic>::getInstance()->getAll( onVector );
}

void LocalKernel::getAllFilterFunctions( vector<string>& onVector ) const
{
  FunctionManagement<FilterFunction>::getInstance()->getAll( onVector );
}

void LocalKernel::getAllSemanticFunctions( TSemanticGroup whichGroup,
    vector<string>& onVector ) const
{
  FunctionManagement<SemanticFunction>::getInstance()->getAll( onVector,
      whichGroup );
}

bool LocalKernel::userMessage( const string& message ) const
{
  if( myMessageFunction == NULL )
    return true;

  return myMessageFunction( message );
}

std::string LocalKernel::composeName( const std::string& name,  const std::string& newExtension )
{
  string newFileName;

  if( name.find_last_of( '.' ) != std::string::npos )
  {
    if ( name.substr( name.length() - 3 ) == ".gz" )
      newFileName = name.substr( 0, name.length() - 6 );
    else
      newFileName = name.substr( 0, name.length() - 3 );
  }
  else
  {
    newFileName = name;
    newFileName.append( "." );
  }
  newFileName.append( newExtension );

  return newFileName;
}

void LocalKernel::copyFile( const std::string& in, const std::string& out )
{
  FILE *fileIn, *fileOut;
  char line[2048];

  if ( ( fileIn = fopen( in.c_str(), "r" ) ) == NULL )
    return;

  fileOut = fopen( out.c_str(), "w" );

  while ( fgets( line, sizeof( line ), fileIn ) != NULL )
    fputs( line, fileOut );

  fclose( fileIn );
  fclose( fileOut );
}

void LocalKernel::copyPCF( const std::string& name, const std::string& traceToLoad )
{
  string pcfIn  = composeName( name, string( "pcf" ) );
  string pcfOut = composeName( traceToLoad, string( "pcf" ) );

  copyFile( pcfIn, pcfOut );
}

void LocalKernel::copyROW( const std::string& name, const std::string& traceToLoad )
{
  string pcfIn  = composeName( name, string( "row" ) );
  string pcfOut = composeName( traceToLoad, string( "row" ) );

  copyFile( pcfIn, pcfOut );
}

void LocalKernel::getNewTraceName( char *name,
                                   char *new_trace_name,
                                   string action,
                                   bool saveNewNameInfo )
{
  /* Putting on table of names in order to generate new trace names */
  int i;
#ifndef WIN32
  struct stat file_info;
#else
  struct _stat64 file_info;
#endif
  char *c;
  char path_name_backup[1024];
  char traceToLoad[1024];
  char *traces_home;

  // Used when
  struct traces_table currentName;

  traceToLoad[0] = '\0';
  /* De forma provisional agafem var entorn */
 // if (( traces_home = getenv( "TRACES_HOME" )) != NULL)
  if (( traces_home = getenv( "PWD" )) != NULL)
    sprintf(traceToLoad,"%s",traces_home);

  strcpy( path_name_backup, new_trace_name );

  // first: look for name in the table
  // precond: trace_names_table_last < MAX_TRACES_HISTORY_LENGTH
  for ( i = 0; i < trace_names_table_last; i++ )
    if ( !strcmp( name, trace_names_table[i].name ) )
      break;

    // second: found? --> ok; not found? --> insert it!
  if ( i == trace_names_table_last )
  {
    if ( saveNewNameInfo )
    {
      // not found!
      if ( trace_names_table_last >= MAX_TRACES_HISTORY_LENGTH )
      {
        // we haven't room; insert as a circular buffer, from the beginning
        trace_names_table_last = 0;
        i = 0;
        free( trace_names_table[ trace_names_table_last ].name );
      }

      // insert it
      trace_names_table[ trace_names_table_last ].name = ( char * )strdup( name );
      trace_names_table[ trace_names_table_last ].num_chop = 0;
      trace_names_table[ trace_names_table_last ].num_sc = 0;
      trace_names_table[ trace_names_table_last ].num_filter = 0;

      // and advance counter
      trace_names_table_last++;
    }
    else
    {
      currentName.name = ( char * )strdup( name );
      currentName.num_chop = 0;
      currentName.num_sc = 0;
      currentName.num_filter = 0;
    }
  }
  else
  {
    currentName = trace_names_table[ i ];
  }

  /* Generating the new trace name and preventing to erase old chops */
  do
  {
    if ( action == TraceCutter::getID() )
    {
      currentName.num_chop++;
    }
    else if ( action == TraceFilter::getID() )
    {
      currentName.num_filter++;
    }
    else if ( action == TraceSoftwareCounters::getID() )
    {
      currentName.num_sc++;
    }
    else
    {
    }

    sprintf( new_trace_name, "%s", path_name_backup );

    if ( strlen( new_trace_name ) > 0 )
    {
      c = strrchr( name, '/' );

      if ( c != NULL )
      {
        c++;
        if ( new_trace_name[strlen( new_trace_name )-1] == '/' )
          sprintf( new_trace_name, "%s%s", new_trace_name, c );
        else
          sprintf( new_trace_name, "%s/%s", new_trace_name, c );
      }
      else
      {
        sprintf( new_trace_name, "%s", name );
      }
    }
    else
      sprintf( new_trace_name, "%s", name );

    c = &new_trace_name[strlen( new_trace_name )-3];

    if ( !strcmp( c, ".gz" ) )
      new_trace_name[strlen( new_trace_name )-6] = '\0';
    else
      new_trace_name[strlen( new_trace_name )-3] = '\0';

    string dotExtension(".");
    string toolInfix( "" );
    int numInfix = 0;
    if ( action == TraceCutter::getID() )
    {
      dotExtension += TraceCutter::getExtension();

      if ( strstr( new_trace_name, dotExtension.c_str() ) == NULL )
        toolInfix = TraceCutter::getExtension();

      numInfix = currentName.num_chop;
    }
    else if ( action == TraceFilter::getID() )
    {
      dotExtension += TraceFilter::getExtension();

      if ( strstr( new_trace_name, dotExtension.c_str() ) == NULL )
        toolInfix = TraceFilter::getExtension();

      numInfix = currentName.num_filter;
    }
    else if ( action == TraceSoftwareCounters::getID() )
    {
      dotExtension += TraceSoftwareCounters::getExtension();

      if ( strstr( new_trace_name, dotExtension.c_str() ) == NULL )
        toolInfix = TraceSoftwareCounters::getExtension();

      numInfix = currentName.num_sc;
    }
    else
    {
    }

    sprintf( new_trace_name, "%s%s%d.prv", new_trace_name, toolInfix.c_str(), numInfix );
  }
#ifndef WIN32
  while ( stat( new_trace_name, &file_info ) == 0 );
#else
  while ( _stat64( new_trace_name, &file_info ) == 0 );
#endif

  if ( saveNewNameInfo )
  {
    trace_names_table[ i ] = currentName;
  }
}


string LocalKernel::getNewTraceName( const string& fullPathTraceName,
                                     const string& traceFilterID,
                                     const bool commitName ) const
{
  vector< string > auxIDs;
  auxIDs.push_back( traceFilterID );

  return getNewTraceName( fullPathTraceName, auxIDs, commitName );
}


// precond1: fullPathTraceName is well formed ( ends with *.prv.gz or *.prv, and path exists)
// precond2: traceids belong to available tools and are inserted in order
string LocalKernel::getNewTraceName( const string& fullPathTraceName,
                                     const vector< string >& traceFilterID,
                                     const bool commitName ) const
{
  string newTraceName, newTraceNamePrv, newTraceNamePrvGz;

  map< string, int > toolNumberedSuffix;

#ifndef WIN32
  struct stat fileInfo;
#else
  struct _stat64 fileInfo;
#endif

  // Get current directory
  // Today, getenv( "TRACES_HOME" )) ) is almost deprecated
/*
  if (( tracesHome = getenv( "PWD" )) != NULL)
  {
    fullPath = string ( tracesHome );
  }
*/

  // Constants
  string PATHSEP = getPathSeparator();

  // Latest treated traces history
  PreviousFiles *recentTraces = PreviousFiles::createPreviousTreatedTraces();

  // Initialization of toolNumberedSuffix
  for( vector< string >::const_iterator id = traceFilterID.begin(); id != traceFilterID.end(); ++id )
  {
    toolNumberedSuffix[ *id ] = 1;
  }

  // Find original trace name, without extension
  size_t prvSuffixStartPos = fullPathTraceName.rfind( GZIPPED_PRV_SUFFIX,
                                                      fullPathTraceName.length() - GZIPPED_PRV_SUFFIX.length() );
  if ( prvSuffixStartPos == string::npos )
  {
    // by precond 1, it exists.
    prvSuffixStartPos = fullPathTraceName.rfind( PRV_SUFFIX,
                                                 fullPathTraceName.length() - PRV_SUFFIX.length() );
    if ( prvSuffixStartPos == string::npos )
    {
      prvSuffixStartPos = fullPathTraceName.length();
    }
  }

  // Blind build of the new trace name
  string originalTraceNameWithoutExtension = fullPathTraceName.substr( 0, prvSuffixStartPos );

  bool newName = false;
  while( !newName )
  {
    // New name prefix
    newTraceName = originalTraceNameWithoutExtension;
    for( vector< string >::const_iterator id = traceFilterID.begin(); id != traceFilterID.end(); ++id )
    {
      stringstream auxStrStrNumber;
      auxStrStrNumber << toolNumberedSuffix[ *id ];
      newTraceName = newTraceName +
                     FILTER_SEP +
                     TraceOptionsProxy::getTraceToolExtension( *id ) +
                     auxStrStrNumber.str();
    }

    // Build new names
    newTraceNamePrv = newTraceName + PRV_SUFFIX;
    newTraceNamePrvGz = newTraceName + GZIPPED_PRV_SUFFIX;

   // Do these new names exist (in current dir)?
 #ifndef WIN32
    newName = ( stat( newTraceNamePrv.c_str(), &fileInfo ) != 0 ) &&
              ( stat( newTraceNamePrvGz.c_str(), &fileInfo ) != 0 );
#else
    newName = ( _stat64( newTraceNamePrv.c_str(), &fileInfo ) != 0 ) &&
              ( _stat64( newTraceNamePrvGz.c_str(), &fileInfo ) != 0 );
#endif

    if ( newName )
    {
      // Seems that we got a new name, because it wasn't found in current dir
      // Now SEARCH IN TRACES HISTORY (idea: other recent directories?)
      vector< string > history = recentTraces->getFiles();
      for( vector< string >::iterator it = history.begin(); it != history.end(); ++it )
      {
        // Does path + trace match with current history line?
        if ( *it == newTraceNamePrv || *it == newTraceNamePrvGz )
        {
          // Found! => Not a new name; break loop through history
          newName = false;
          break;
        }
        else
        {
          // Does trace or trace.gz (without path) match with same part of current history line?

          // Extract history name
          size_t historyNameStartPos = it->rfind( PATHSEP );
          if ( historyNameStartPos == string::npos )
          {
            historyNameStartPos = 0;
          }
          string auxHistName = it->substr( historyNameStartPos );

          // Extract new name
          size_t newNameStartPos = newTraceNamePrv.rfind( PATHSEP );
          if ( newNameStartPos == string::npos )
          {
            newNameStartPos = 0;
          }
          string auxNewName = newTraceNamePrv.substr( newNameStartPos );

          // Extract new name with gz suffix
          size_t newNameGzStartPos = newTraceNamePrvGz.rfind( PATHSEP );
          if ( newNameGzStartPos == string::npos )
          {
            newNameGzStartPos = 0;
          }
          string auxNewGzName = newTraceNamePrvGz.substr( newNameGzStartPos );

          // and check them
          if( auxHistName == auxNewName || auxHistName == auxNewGzName )
          {
            // Found! => Not a new name; break loop through history
            newName = false;
            break;
          }
        }
      }
    }

    if ( !newName )
    {
      // Found! => Increment version
      toolNumberedSuffix[ traceFilterID[ traceFilterID.size() - 1 ] ]++;
    }
  }

  if ( commitName )
  {
    // Update traces history
    recentTraces->add( newTraceNamePrv );
  }

  delete recentTraces;

  return newTraceNamePrv;
}


void LocalKernel::commitNewTraceName( const string& newTraceName ) const
{
  PreviousFiles *recentTraces = PreviousFiles::createPreviousTreatedTraces();
  recentTraces->add( newTraceName );
  delete recentTraces;
}


string LocalKernel::getNewTraceName( const string& fullPathTraceName,
                                     const string& outputPath,
                                     const vector< string >& traceFilterID,
                                     const bool commitName ) const
{
  string newName;

  if ( outputPath != "" )
  {
    // Remove old path
    string PATHSEP = getPathSeparator();

    size_t pathSeparatorFinalPos = fullPathTraceName.rfind( PATHSEP );
    if ( pathSeparatorFinalPos == string::npos )
    {
      pathSeparatorFinalPos = 0;
    }

    string traceName;
    if ( pathSeparatorFinalPos == fullPathTraceName.length() - 1 )
    {
      traceName = "tmp.prv";
    }
    else
    {
      traceName = fullPathTraceName.substr( pathSeparatorFinalPos + 1, fullPathTraceName.length() );
    }

    string auxPath;
    if ( outputPath.substr( outputPath.length() - 1 , 1 ) != PATHSEP )
      auxPath = outputPath + PATHSEP + traceName;
    else
      auxPath = outputPath + traceName;

    newName = getNewTraceName( auxPath, traceFilterID, commitName );
  }
  else
  {
    newName = getNewTraceName( fullPathTraceName, traceFilterID, commitName );
  }

  return newName;
}
