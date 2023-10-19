/** @file
 *****************************************************************************

Implementation of functions for profiling code blocks.

See profiling.hpp .

*****************************************************************************
* @author     This file is part of libff, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#include <cassert>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <list>
#include <stdexcept>
#include <vector>

// #include <libff/common/default_types/ec_pp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#ifndef NO_PROCPS
#include <proc/readproc.h>
#endif

#include <logging.hpp>


#undef printf 
#undef fflush
#define printf(...) snprintf(snprintf_buffer, snprintf_buffer_size,__VA_ARGS__); \
                    profiling_logs << snprintf_buffer ;



namespace libff {

#ifndef SILENT_BUILD

    long long get_nsec_time(){
        auto timepoint = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(timepoint.time_since_epoch()).count();
    }

    /* Return total CPU time consumsed by all threads of the process, in nanoseconds. */
    long long get_nsec_cpu_time(){
    #if _MSC_VER
        return 0;
    #else
        ::timespec ts;
        if ( ::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) )
            throw ::std::runtime_error("clock_gettime(CLOCK_PROCESS_CPUTIME_ID) failed");
            // If we expected this to work, don't silently ignore failures, because that would hide the problem and incur an unnecessarily system-call overhead. So if we ever observe this exception, we should probably add a suitable #ifdef .
            //TODO: clock_gettime(CLOCK_PROCESS_CPUTIME_ID) is not supported by native Windows. What about Cygwin? Should we #ifdef on CLOCK_PROCESS_CPUTIME_ID or on __linux__?
        return ts.tv_sec * 1000000000ll + ts.tv_nsec;
    #endif
    }


    profiling::profiling (){    
        last_time = start_time = get_nsec_time();
        last_cpu_time = start_cpu_time = get_nsec_cpu_time();
    }


    profiling::~profiling(){
        invocation_counts.clear();
        last_times.clear();
        last_cpu_times.clear();
        cumulative_times.clear();
        profiling_logs.clear();
    }


    void profiling::print_cumulative_time_entry(const std::string &key, const long long factor)
    {
        const double total_ms = (cumulative_times.at(key) * 1e-6);
        const size_t cnt = invocation_counts.at(key);
        const double avg_ms = total_ms / cnt;
    
        printf(  "   %-45s: %12.5fms = %lld * %0.5fms (%zu invocations, %0.5fms = %lld * %0.5fms per invocation)\n", key.c_str(), total_ms, factor, total_ms/factor, cnt, avg_ms, factor, avg_ms/factor);
    }


    void profiling::print_cumulative_times(const long long factor)
    {
        printf("Dumping times:\n");
        for (auto& kv : cumulative_times)
        {
            print_cumulative_time_entry(kv.first, factor);
        }
    }


    static void print_times_from_last_and_start(long long now, long long last,
                                                long long cpu_now, long long cpu_last ,
                                                long long start_time , long long start_cpu_time ,
                                                char * snprintf_buffer , std::stringstream & profiling_logs )
    {   
        long long time_from_start = now - start_time;
        long long time_from_last = now - last;

        long long cpu_time_from_start = cpu_now - start_cpu_time;
        long long cpu_time_from_last = cpu_now - cpu_last;

        if (time_from_last != 0) {
            double parallelism_from_last = 1.0 * cpu_time_from_last / time_from_last;
            printf("[%0.4fs x%0.2f]", time_from_last * 1e-9, parallelism_from_last);
        } else {
            printf("[             ]");
        }
        if (time_from_start != 0) {
            double parallelism_from_start = 1.0 * cpu_time_from_start / time_from_start;
            printf("\t(%0.4fs x%0.2f from start)", time_from_start * 1e-9, parallelism_from_start);
        }
    }


    void profiling::print_header(const char *msg)
    {
        printf("\n================================================================================\n");
        printf("%s\n", msg);
        printf("================================================================================\n\n");
    }

    void profiling::print_indent()
    {
        for (size_t i = 0; i < indentation; ++i)
        {
            printf("  ");
        }
    }

     
    void profiling::enter_block(const std::string &msg, const bool indent){

        block_names.emplace_back(msg);
        long long t = get_nsec_time();
        enter_times[msg] = t;
        long long cpu_t = get_nsec_cpu_time();
        enter_cpu_times[msg] = cpu_t;


        print_indent();
        printf("(enter) %-35s\t", msg.c_str());
        print_times_from_last_and_start(t, t, cpu_t, cpu_t , start_time , start_cpu_time , snprintf_buffer , profiling_logs );
        printf("\n");


        if (indent)
        {
            ++indentation;
        }

    }

    void profiling::leave_block(const std::string &msg, const bool indent){

        block_names.pop_back();

        ++invocation_counts[msg];

        long long t = get_nsec_time();
        last_times[msg] = (t - enter_times[msg]);
        cumulative_times[msg] += (t - enter_times[msg]);

        long long cpu_t = get_nsec_cpu_time();
        last_cpu_times[msg] = (cpu_t - enter_cpu_times[msg]);


        if (indent)
        {
            --indentation;
        }

        print_indent();
        printf("(leave) %-35s\t", msg.c_str());
        print_times_from_last_and_start(t, enter_times[msg], cpu_t, enter_cpu_times[msg] , start_time , start_cpu_time , snprintf_buffer , profiling_logs );
        printf("\n");
        
    }
     
#endif

} // libff
