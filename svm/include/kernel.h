#ifndef KERNEL_H
#define KERNEL_H

#include <deque>
#include <queue>
#include <string>

#include "board.h"
#include "process.h"

namespace svm
{
    class Kernel
    {
    public:
        enum Scheduler
        {
            FirstComeFirstServed,
            ShortestJob,
            RoundRobin,
            Priority
        };

        typedef std::deque<Process> process_list_type;
        typedef std::priority_queue<Process> process_priorities_type;

        Board board;

        process_list_type processes;
        process_priorities_type priorities;

        Scheduler scheduler;

		Memory::page_table_type *page_table;

        Kernel(
          Scheduler scheduler,
          //std::vector<Memory::ram_type> executables_paths
          std::vector<std::string> executables_paths
        );
        virtual ~Kernel();

        void CreateProcess(const std::string &name);
        Memory::vmem_size_type VirtualMemoryToPhysical(Memory::ram_size_type previous_index);
        Memory::ram_size_type AllocateMemory(Memory::ram_size_type units);
        void FreeMemory(Memory::ram_size_type physical_memory_index);

    private:
        static const unsigned int _MAX_CYCLES_BEFORE_PREEMPTION = 5;

        Process::process_id_type _last_issued_process_id;
		    process_list_type::size_type _current_process_index;
        Memory::ram_type::size_type _last_ram_position;
        unsigned int _cycles_passed_after_preemption;

        Memory::ram_size_type _free_physical_memory_index;
    };
}

#endif
