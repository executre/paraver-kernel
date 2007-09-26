#include "kwindow.h"
#include "semanticcomposefunctions.h"
#include "semanticderived.h"

/**********************************************************************
 *  KSingleWindow implementation
 **********************************************************************/

KSingleWindow::KSingleWindow( Trace *whichTrace ): KWindow( whichTrace )
{
  functions[ TOPCOMPOSE1 ] = new ComposeAsIs();
  functions[ TOPCOMPOSE2 ] = new ComposeAsIs();

  functions[ COMPOSEWORKLOAD ] = new ComposeAsIs();
  functions[ WORKLOAD ] = NULL;
  functions[ COMPOSEAPPLICATION ] = new ComposeAsIs();
  functions[ APPLICATION ] = NULL;
  functions[ COMPOSETASK ] = new ComposeAsIs();
  functions[ TASK ] = NULL;
  functions[ COMPOSETHREAD ] = new ComposeAsIs();
  functions[ THREAD ] = NULL;

  functions[ COMPOSESYSTEM ] = new ComposeAsIs();
  functions[ SYSTEM ] = NULL;
  functions[ COMPOSENODE ] = new ComposeAsIs();
  functions[ NODE ] = NULL;
  functions[ COMPOSECPU ] = new ComposeAsIs();
  functions[ CPU ] = NULL;

  if ( myTrace->totalThreads() > myTrace->totalCPUs() )
  {
    recordsByTime.reserve( myTrace->totalThreads() );
    intervalTopCompose1.reserve( myTrace->totalThreads() );
    intervalTopCompose2.reserve( myTrace->totalThreads() );
    for ( TThreadOrder i = 0; i < myTrace->totalThreads(); i++ )
    {
      recordsByTime.push_back( NULL );
      intervalTopCompose1.push_back( IntervalCompose( this, TOPCOMPOSE1, i ) );
      intervalTopCompose2.push_back( IntervalCompose( this, TOPCOMPOSE2, i ) );
    }
  }
  else
  {
    recordsByTime.reserve( myTrace->totalCPUs() );
    intervalTopCompose1.reserve( myTrace->totalCPUs() );
    intervalTopCompose2.reserve( myTrace->totalCPUs() );
    for ( TCPUOrder i = 0; i < myTrace->totalCPUs(); i++ )
    {
      recordsByTime.push_back( NULL );
      intervalTopCompose1.push_back( IntervalCompose( this, TOPCOMPOSE1, i ) );
      intervalTopCompose2.push_back( IntervalCompose( this, TOPCOMPOSE2, i ) );
    }
  }

  intervalWorkload.push_back( IntervalNotThread( this, WORKLOAD, 0 ) );
  intervalComposeWorkload.push_back( IntervalCompose( this, COMPOSEWORKLOAD, 0 ) );

  intervalApplication.reserve( myTrace->totalApplications() );
  intervalComposeApplication.reserve( myTrace->totalApplications() );
  for ( TApplOrder i = 0; i < myTrace->totalApplications(); i++ )
  {
    intervalApplication.push_back( IntervalNotThread( this, APPLICATION, i ) );
    intervalComposeApplication.push_back( IntervalCompose( this, COMPOSEAPPLICATION, i ) );
  }

  intervalTask.reserve( myTrace->totalTasks() );
  intervalComposeTask.reserve( myTrace->totalTasks() );
  for ( TTaskOrder i = 0; i < myTrace->totalTasks(); i++ )
  {
    intervalTask.push_back( IntervalNotThread( this, TASK, i ) );
    intervalComposeTask.push_back( IntervalCompose( this, COMPOSETASK, i ) );
  }

  intervalThread.reserve( myTrace->totalThreads() );
  intervalComposeThread.reserve( myTrace->totalThreads() );
  for ( TThreadOrder i = 0; i < myTrace->totalThreads(); i++ )
  {
    intervalThread.push_back( IntervalThread( this, THREAD, i ) );
    intervalComposeThread.push_back( IntervalCompose( this, COMPOSETHREAD, i ) );
  }

  intervalSystem.push_back( IntervalNotThread( this, SYSTEM, 0 ) );
  intervalComposeSystem.push_back( IntervalCompose( this, COMPOSESYSTEM, 0 ) );

  intervalNode.reserve( myTrace->totalNodes() );
  intervalComposeNode.reserve( myTrace->totalNodes() );
  for ( TNodeOrder i = 0; i < myTrace->totalNodes(); i++ )
  {
    intervalNode.push_back( IntervalNotThread( this, NODE, i ) );
    intervalComposeNode.push_back( IntervalCompose( this, COMPOSENODE, i ) );
  }

  intervalCPU.reserve( myTrace->totalCPUs() );
  intervalComposeCPU.reserve( myTrace->totalCPUs() );
  for ( TCPUOrder i = 0; i < myTrace->totalCPUs(); i++ )
  {
    intervalCPU.push_back( IntervalCPU( this, CPU, i ) );
    intervalComposeCPU.push_back( IntervalCompose( this, COMPOSECPU, i ) );
  }

  myFilter = new Filter( this );
}


RecordList *KSingleWindow::init( TRecordTime initialTime, TCreateList create )
{
  TObjectOrder objectSize = 0;

  for( UINT8 i = WORKLOAD; i <= COMPOSECPU; i++ )
  {
    if( functions[ i ] != NULL )
      functions[ i ]->init( this );
  }

  if ( level >= SYSTEM )
  {
    if ( initialTime > 0 && !initFromBegin() )
      myTrace->getRecordByTimeCPU( recordsByTime, initialTime );
    else
    {
      for ( TCPUOrder i = 0; i < myTrace->totalCPUs(); i++ )
      {
        if ( recordsByTime[ i ] != NULL )
          delete recordsByTime[ i ];
        recordsByTime[ i ] = myTrace->CPUBegin( i );
      }
    }
  }
  else
  {
    if ( initialTime > 0 && !initFromBegin() )
      myTrace->getRecordByTimeThread( recordsByTime, initialTime );
    else
    {
      for ( TThreadOrder i = 0; i < myTrace->totalThreads(); i++ )
      {
        if ( recordsByTime[ i ] != NULL )
          delete recordsByTime[ i ];
        recordsByTime[ i ] = myTrace->threadBegin( i );
      }
    }
  }

  if ( level == WORKLOAD )
    objectSize = 1;
  else if ( level == APPLICATION )
    objectSize = intervalApplication.size();
  else if ( level == TASK )
    objectSize = intervalTask.size();
  else if ( level == THREAD )
    objectSize = intervalThread.size();
  else if ( level == SYSTEM )
    objectSize = 1;
  else if ( level == NODE )
    objectSize = intervalNode.size();
  else if ( level == CPU )
    objectSize = intervalCPU.size();

  for ( TObjectOrder i = 0; i < objectSize; i++ )
    intervalTopCompose1[ i ].init( initialTime, create );

  return NULL;
}


void KSingleWindow::setLevelFunction( TWindowLevel whichLevel,
                                      SemanticFunction *whichFunction )
{
  if ( functions[ whichLevel ] != NULL )
    delete functions[ whichLevel ];
  functions[ whichLevel ] = whichFunction;
}


SemanticFunction *KSingleWindow::getLevelFunction( TWindowLevel whichLevel )
{
  return functions[ whichLevel ];
}


void KSingleWindow::setFunctionParam( TWindowLevel whichLevel,
                                      TParamIndex whichParam,
                                      const TParamValue& newValue )
{
  functions[ whichLevel ]->setParam( whichParam, newValue );
}


bool KSingleWindow::initFromBegin() const
{
  bool tmp = false;

  tmp = tmp || functions[ TOPCOMPOSE1 ]->getInitFromBegin();
  tmp = tmp || functions[ TOPCOMPOSE2 ]->getInitFromBegin();

  if ( level == WORKLOAD )
  {
    tmp = tmp || functions[ COMPOSEWORKLOAD ]->getInitFromBegin();
    tmp = tmp || functions[ WORKLOAD ]->getInitFromBegin();
    tmp = tmp || functions[ COMPOSEAPPLICATION ]->getInitFromBegin();
    tmp = tmp || functions[ APPLICATION ]->getInitFromBegin();
    tmp = tmp || functions[ COMPOSETASK ]->getInitFromBegin();
    tmp = tmp || functions[ TASK ]->getInitFromBegin();
  }
  else if ( level == APPLICATION )
  {
    tmp = tmp || functions[ COMPOSEAPPLICATION ]->getInitFromBegin();
    tmp = tmp || functions[ APPLICATION ]->getInitFromBegin();
    tmp = tmp || functions[ COMPOSETASK ]->getInitFromBegin();
    tmp = tmp || functions[ TASK ]->getInitFromBegin();
  }
  else if ( level == TASK )
  {
    tmp = tmp || functions[ COMPOSETASK ]->getInitFromBegin();
    tmp = tmp || functions[ TASK ]->getInitFromBegin();
  }
  else if ( level == SYSTEM )
  {
    tmp = tmp || functions[ COMPOSESYSTEM ]->getInitFromBegin();
    tmp = tmp || functions[ SYSTEM ]->getInitFromBegin();
    tmp = tmp || functions[ COMPOSENODE ]->getInitFromBegin();
    tmp = tmp || functions[ NODE ]->getInitFromBegin();
    tmp = tmp || functions[ COMPOSECPU ]->getInitFromBegin();
    tmp = tmp || functions[ CPU ]->getInitFromBegin();
  }
  else if ( level == NODE )
  {
    tmp = tmp || functions[ COMPOSENODE ]->getInitFromBegin();
    tmp = tmp || functions[ NODE ]->getInitFromBegin();
    tmp = tmp || functions[ COMPOSECPU ]->getInitFromBegin();
    tmp = tmp || functions[ CPU ]->getInitFromBegin();
  }
  else if ( level == CPU )
  {
    tmp = tmp || functions[ COMPOSECPU ]->getInitFromBegin();
    tmp = tmp || functions[ CPU ]->getInitFromBegin();
  }

  tmp = tmp || functions[ COMPOSETHREAD ]->getInitFromBegin();
  tmp = tmp || functions[ THREAD ]->getInitFromBegin();

  return tmp;
}


RecordList *KSingleWindow::calcNext( TObjectOrder whichObject )
{
  return intervalTopCompose1[ whichObject ].calcNext();
}


RecordList *KSingleWindow::calcPrev( TObjectOrder whichObject )
{
  return intervalTopCompose1[ whichObject ].calcPrev();
}


TRecordTime KSingleWindow::getBeginTime( TObjectOrder whichObject ) const
{
  return intervalTopCompose1[ whichObject ].getBeginTime();
}


TRecordTime KSingleWindow::getEndTime( TObjectOrder whichObject ) const
{
  return intervalTopCompose1[ whichObject ].getEndTime();
}


TSemanticValue KSingleWindow::getValue( TObjectOrder whichObject ) const
{
  return intervalTopCompose1[ whichObject ].getValue();
}


Interval *KSingleWindow::getLevelInterval( TWindowLevel whichLevel,
    TObjectOrder whichOrder )
{
  if ( whichLevel == TOPCOMPOSE1 )
    return &intervalTopCompose1[ whichOrder ];
  else if ( whichLevel == TOPCOMPOSE2 )
    return &intervalTopCompose2[ whichOrder ];
  else if ( whichLevel == COMPOSEWORKLOAD )
    return &intervalComposeWorkload[ whichOrder ];
  else if ( whichLevel == WORKLOAD )
    return &intervalWorkload[ whichOrder ];
  else if ( whichLevel == COMPOSEAPPLICATION )
    return &intervalComposeApplication[ whichOrder ];
  else if ( whichLevel == APPLICATION )
    return &intervalApplication[ whichOrder ];
  else if ( whichLevel == COMPOSETASK )
    return &intervalComposeTask[ whichOrder ];
  else if ( whichLevel == TASK )
    return &intervalTask[ whichOrder ];
  else if ( whichLevel == COMPOSETHREAD )
    return &intervalComposeThread[ whichOrder ];
  else if ( whichLevel == THREAD )
    return &intervalThread[ whichOrder ];
  else if ( whichLevel == COMPOSESYSTEM )
    return &intervalComposeSystem[ whichOrder ];
  else if ( whichLevel == SYSTEM )
    return &intervalSystem[ whichOrder ];
  else if ( whichLevel == COMPOSENODE )
    return &intervalComposeNode[ whichOrder ];
  else if ( whichLevel == NODE )
    return &intervalNode[ whichOrder ];
  else if ( whichLevel == COMPOSECPU )
    return &intervalComposeCPU[ whichOrder ];
  else if ( whichLevel == CPU )
    return &intervalCPU[ whichOrder ];
  return NULL;
}


/**********************************************************************
 *  KDerivedWindow implementation
 **********************************************************************/

void KDerivedWindow::setup()
{
  factor.clear();
  factor.push_back( 1.0 );
  factor.push_back( 1.0 );

  myTrace = parents[ 0 ]->getTrace();

  if( functions[ 0 ] != NULL )
    delete functions[ 0 ];
  if( functions[ 1 ] != NULL )
    delete functions[ 1 ];
  if( functions[ 2 ] != NULL )
    delete functions[ 2 ];

  functions[ 0 ] = new ComposeAsIs();
  functions[ 1 ] = new ComposeAsIs();
  functions[ 2 ] = NULL;

  intervalTopCompose1.clear();
  intervalTopCompose2.clear();
  intervalDerived.clear();
  intervalControlDerived.clear();
  if ( myTrace->totalThreads() > myTrace->totalCPUs() )
  {
    intervalTopCompose1.reserve( myTrace->totalThreads() );
    intervalTopCompose2.reserve( myTrace->totalThreads() );
    intervalDerived.reserve( myTrace->totalThreads() );
    intervalControlDerived.reserve( myTrace->totalThreads() );
    for ( TThreadOrder i = 0; i < myTrace->totalThreads(); i++ )
    {
      intervalTopCompose1.push_back( IntervalCompose( this, TOPCOMPOSE1, i ) );
      intervalTopCompose2.push_back( IntervalCompose( this, TOPCOMPOSE2, i ) );
      intervalDerived.push_back( IntervalDerived( this, DERIVED, i ) );
      intervalControlDerived.push_back( IntervalControlDerived( this, DERIVED, i ) );
    }
  }
  else
  {
    intervalTopCompose1.reserve( myTrace->totalCPUs() );
    intervalTopCompose2.reserve( myTrace->totalCPUs() );
    intervalDerived.reserve( myTrace->totalCPUs() );
    intervalControlDerived.reserve( myTrace->totalCPUs() );
    for ( TCPUOrder i = 0; i < myTrace->totalCPUs(); i++ )
    {
      intervalTopCompose1.push_back( IntervalCompose( this, TOPCOMPOSE1, i ) );
      intervalTopCompose2.push_back( IntervalCompose( this, TOPCOMPOSE2, i ) );
      intervalDerived.push_back( IntervalDerived( this, DERIVED, i ) );
      intervalControlDerived.push_back( IntervalControlDerived( this, DERIVED, i ) );
    }
  }
}


void KDerivedWindow::setLevelFunction( TWindowLevel whichLevel,
                                       SemanticFunction *whichFunction )
{
  if( whichLevel == TOPCOMPOSE1 )
    whichLevel = (TWindowLevel) 0;
  else if( whichLevel == TOPCOMPOSE2 )
    whichLevel = (TWindowLevel) 1;
  else if( whichLevel == DERIVED )
    whichLevel = (TWindowLevel) 2;

  if ( functions[ whichLevel ] != NULL )
    delete functions[ whichLevel ];
  functions[ whichLevel ] = whichFunction;
}


SemanticFunction *KDerivedWindow::getLevelFunction( TWindowLevel whichLevel )
{
  if( whichLevel == TOPCOMPOSE1 )
    whichLevel = (TWindowLevel) 0;
  else if( whichLevel == TOPCOMPOSE2 )
    whichLevel = (TWindowLevel) 1;
  else if( whichLevel == DERIVED )
    whichLevel = (TWindowLevel) 2;

  return functions[ whichLevel ];
}


void KDerivedWindow::setFunctionParam( TWindowLevel whichLevel,
                                       TParamIndex whichParam,
                                       const TParamValue& newValue )
{
  if( whichLevel == TOPCOMPOSE1 )
    whichLevel = (TWindowLevel) 0;
  else if( whichLevel == TOPCOMPOSE2 )
    whichLevel = (TWindowLevel) 1;
  else if( whichLevel == DERIVED )
    whichLevel = (TWindowLevel) 2;

  functions[ whichLevel ]->setParam( whichParam, newValue );
}


bool KDerivedWindow::initFromBegin() const
{
  bool tmp = false;

  tmp = tmp || functions[ 0 ]->getInitFromBegin();
  tmp = tmp || functions[ 1 ]->getInitFromBegin();
  tmp = tmp || functions[ 2 ]->getInitFromBegin();

  return tmp;
}


RecordList *KDerivedWindow::init( TRecordTime initialTime, TCreateList create )
{
  TObjectOrder objectSize = 0;
  TWindowLevel tmpLevel = getLevel();

  functions[ 0 ]->init( this );
  functions[ 1 ]->init( this );
  functions[ 2 ]->init( this );

  if ( tmpLevel == WORKLOAD )
    objectSize = 1;
  else if ( tmpLevel == APPLICATION )
    objectSize = myTrace->totalApplications();
  else if ( tmpLevel == TASK )
    objectSize = myTrace->totalTasks();
  else if ( tmpLevel == THREAD )
    objectSize = myTrace->totalThreads();
  else if ( tmpLevel == SYSTEM )
    objectSize = 1;
  else if ( tmpLevel == NODE )
    objectSize = myTrace->totalNodes();
  else if ( tmpLevel == CPU )
    objectSize = myTrace->totalCPUs();

  for( UINT8 i = 0; i < parents.size(); i++ )
    parents[ i ]->init( initialTime, create );

  for ( TObjectOrder i = 0; i < objectSize; i++ )
    intervalTopCompose1[ i ].init( initialTime, create );

  return NULL;
}


RecordList *KDerivedWindow::calcNext( TObjectOrder whichObject )
{
  return intervalTopCompose1[ whichObject ].calcNext();
}


RecordList *KDerivedWindow::calcPrev( TObjectOrder whichObject )
{
  return intervalTopCompose1[ whichObject ].calcPrev();
}


TRecordTime KDerivedWindow::getBeginTime( TObjectOrder whichObject ) const
{
  return intervalTopCompose1[ whichObject ].getBeginTime();
}


TRecordTime KDerivedWindow::getEndTime( TObjectOrder whichObject ) const
{
  return intervalTopCompose1[ whichObject ].getEndTime();
}


TSemanticValue KDerivedWindow::getValue( TObjectOrder whichObject ) const
{
  return intervalTopCompose1[ whichObject ].getValue();
}


Interval *KDerivedWindow::getLevelInterval( TWindowLevel whichLevel,
    TObjectOrder whichOrder )
{
  if ( whichLevel == TOPCOMPOSE1 )
    return &intervalTopCompose1[ whichOrder ];
  else if ( whichLevel == TOPCOMPOSE2 )
    return &intervalTopCompose2[ whichOrder ];
  else if( whichLevel == DERIVED )
  {
    if( ( (SemanticDerived *)functions[ 2 ] )->isControlDerived() )
      return &intervalControlDerived[ whichOrder ];
    else
      return &intervalDerived[ whichOrder ];
  }
  return NULL;
}