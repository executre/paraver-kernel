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

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL$
 | @last_commit: $Date$
 | @version:     $Revision$
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef _KTRACECUTTER_H_
#define _KTRACECUTTER_H_

#include "ktraceoptions.h"
#include "zlib.h"

#include "tracecutter.h"
#include <set>

class KTraceCutter : public TraceCutter
{
  public:
    KTraceCutter( char *&trace_in,
                  char *&trace_out,
                  TraceOptions *options,
                  const vector< TEventType > &whichTypesWithValuesZero,
                  ProgressController *progress );
    virtual ~KTraceCutter();

    virtual void set_by_time( bool byTime );
    virtual void set_min_cutting_time( unsigned long long minCutTime );
    virtual void set_max_cutting_time( unsigned long long maxCutTime );
    virtual void set_minimum_time_percentage( unsigned long long minimumPercentage );
    virtual void set_maximum_time_percentage( unsigned long long maximumPercentage );
    virtual void set_tasks_list( char *tasksList );
    virtual void set_original_time( bool originalTime );
    virtual void set_max_trace_size( int traceSize );
    virtual void set_break_states( bool breakStates );
    virtual void set_remFirstStates( bool remStates );
    virtual void set_remLastStates( bool remStates );

    virtual void execute( char *trace_in,
                          char *trace_out,
                          ProgressController *progress );

  private:
    /* Buffer for reading trace records */
    char line[MAX_LINE_SIZE];

    /* Trace in and trace out */
    FILE *infile;
    FILE *outfile;
    gzFile gzInfile;

    unsigned int min_perc;
    unsigned int max_perc;
    bool by_time;
    bool originalTime;
    unsigned int max_size;
    bool is_zip;
    unsigned int cut_tasks;
    bool break_states;
    unsigned long long time_min;
    unsigned long long time_max;
    unsigned long long total_time;
    unsigned long long first_record_time;
    unsigned long long last_record_time;
    unsigned long long current_size;
    unsigned long long total_size;
    unsigned long long trace_time;
    int num_tasks;
    int useful_tasks;
    int init_task_counter;
    bool remFirstStates;
    bool remLastStates;
    bool first_time_caught;

    /* Parameters for showing percentage */
    unsigned long long total_trace_size;
    unsigned long long current_read_size;
    unsigned long total_cutter_iters;

    /* Vars for saving the HC that will appear on the trace */
    unsigned long long counters[500];
    int last_counter;

    // Event types scanned from .pcf file with declared value 0.
    set< TEventType > PCFEventTypesWithValuesZero;

    /* Struct for the case of MAX_TRACE_SIZE */
    class thread_info
    {
      public:
        unsigned long long last_time;
        bool finished;
        TCPUOrder lastCPU; // last CPU to be able to write trailing records.
        set< TEventType >      eventTypesWithoutPCFZeros; //
        multiset< TEventType > eventTypesWithPCFZeros; //
    };

    /* struct for cutting only selected tasks */
    thread_info *tasks[MAX_APPL][MAX_TASK][MAX_THREAD];

    struct selected_tasks
    {
      int min_task_id;
      int max_task_id;
      int range;
    };

    struct selected_tasks wanted_tasks[MAX_SELECTED_TASKS];
    KTraceOptions *exec_options;

    void read_cutter_params();
    void proces_cutter_header( char *header,
                               char *trace_in_name,
                               char *trace_out_name );
    void adjust_to_final_time();
    void ini_cutter_progress_bar( char *file_name, ProgressController *progress );
    void show_cutter_progress_bar( ProgressController *progress );
    void update_queue( int appl, int task, int thread,
                       unsigned long long type,
                       unsigned long long value );
    void load_counters_of_pcf( char *trace_name );
    void shift_trace_to_zero( char *nameIn, char *nameOut );
    bool is_selected_task( int task_id );
};

#endif // _KTRACECUTTER_H_
