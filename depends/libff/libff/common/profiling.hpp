/** @file
 *****************************************************************************

 Declaration of functions for profiling code blocks.

 Reports time, operation counts, memory usage, and others.

 *****************************************************************************
 * @author     This file is part of libff, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef PROFILING_HPP_
#define PROFILING_HPP_

#include <cstddef>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "utils.hpp"

using namespace std ;

#ifndef SILENT_BUILD

#define snprintf_buffer_size 4096

#define profile_printf(...)                                 \
    {                                                       \
        char buf[ snprintf_buffer_size ] ;                  \
        snprintf(buf, snprintf_buffer_size,__VA_ARGS__);    \
        profile.get_profiling_logs() << buf ;               \
    }

#else
    #define profile_printf(...)
#endif


namespace libff {

#ifndef SILENT_BUILD

    

    class profiling {

    private:
        
        long long start_time;
        long long last_time;
        long long start_cpu_time;
        long long last_cpu_time;

        std::stringstream profiling_logs ;

        char snprintf_buffer[ snprintf_buffer_size ] ;

        std::map<std::string, size_t>    invocation_counts;
        std::map<std::string, long long> enter_times;
        std::map<std::string, long long> last_times;
        std::map<std::string, long long> cumulative_times;
        
        //TODO: Instead of analogous maps for time and cpu_time, use a single struct-valued map
        std::map<std::string, long long> enter_cpu_times;
        std::map<std::string, long long> last_cpu_times;
        
        size_t indentation = 0;

        std::vector<std::string> block_names;

        void print_cumulative_time_entry(const std::string &key, const long long factor=1);
        void print_cumulative_times(const long long factor=1);

        public:
            profiling();
            ~profiling();
            void print_indent();
            void print_header(const char* msg);
            void enter_block(const std::string &msg, const bool indent=true);
            void leave_block(const std::string &msg, const bool indent=true);
            std::stringstream & get_profiling_logs() { return profiling_logs ; }
    
    };
#else
        
    class profiling {
        std::stringstream profiling_logs ;
    public:
        profiling(){}
        void print_indent(){}
        void enter_block(const std::string &msg, const bool indent=true){ libff::UNUSED(msg,indent) ; }
        void leave_block(const std::string &msg, const bool indent=true){ libff::UNUSED(msg,indent) ; }
        std::stringstream & get_profiling_logs() { return profiling_logs ; }
    };

#endif

} // libff

#endif // PROFILING_HPP_
