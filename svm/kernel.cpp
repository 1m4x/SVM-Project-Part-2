#include "kernel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <limits>

namespace svm
{
    Kernel::Kernel(
    Scheduler scheduler,
    //std::vector<Memory::ram_type> executables_paths
    std::vector<std::string> executables_paths
    )
    : board(),
    processes(),
    priorities(),
    scheduler(scheduler),
    _last_issued_process_id(0),
    _last_ram_position(0),
    _cycles_passed_after_preemption(0),
    _current_process_index(0)
    {
        // Memory
        board.memory.ram[0] = _free_physical_memory_index = 0;
        board.memory.ram[1] = board.memory.ram.size() - 2;
        //Check for empty frame
        board.pic.isr_4 = [&]() {
            std::cout << "Kernel: page fault." << std::endl;

            Memory::page_entry_type page = board.cpu.registers.a;
            Memory::page_entry_type frame = board.memory.AcquireFrame();

            if(frame != Memory::INVALID_PAGE)
            {
                (*(board.memory.page_table))[page] = frame;

            }
            else
            {
                board.Stop();
            }
        };
        //Process Management
        std::for_each(executables_paths.begin(), executables_paths.end(), [&](const std::string &path) {
            CreateProcess(path);
        });

        if (!processes.empty()) {
            std::cout << "Kernel: set process: " << processes[_current_process_index].id << " for execution." << std::endl;

            board.cpu.registers = processes[_current_process_index].registers;
            board.memory.page_table = processes[_current_process_index].page_table;

            processes[_current_process_index].state = Process::Running;
        }
        if (scheduler == FirstComeFirstServed) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the FCFS
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the FCFS
                // Unload the current process
                std::cout << "Number of Processes left = " << processes.size() << std::endl;
                processes.pop_front();
                if (processes.empty()) {
                    board.Stop();
                }
                else {
                    board.cpu.registers = processes[0].registers;
                    processes[0].state = Process::States::Running;
                }
            };
            } else if (scheduler == ShortestJob) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Shortest
                //  Job scheduler
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Shortest
                //  Job scheduler
                // Unload the current process
                std::cout << "Number of Processes left = " << processes.size() << std::endl;

                processes.pop_front();

                if (processes.empty()) {
                    board.Stop();
                }
                else {
                    board.cpu.registers = processes[0].registers;
                    processes[0].state = Process::States::Running;
                }
            };
            } else if (scheduler == RoundRobin) {
            board.pic.isr_0 = [&]() {
                std::cout << "Kernel: processing the timer interrupt." << std::endl;

                if (!processes.empty()) {
                    if (_cycles_passed_after_preemption <= Kernel::_MAX_CYCLES_BEFORE_PREEMPTION)
                    {
                        std::cout << "Kernel: allowing the current process " << processes[_current_process_index].id << " to run." << std::endl;

                        ++_cycles_passed_after_preemption;

                        std::cout << "Kernel: the current cycle is " << _cycles_passed_after_preemption << std::endl;
                        } else {
                        if (processes.size() > 1) {
                            std::cout << "Kernel: switching the context from process " << processes[_current_process_index].id;

                            processes[_current_process_index].registers = board.cpu.registers;
                            processes[_current_process_index].state = Process::Ready;

                            _current_process_index = (_current_process_index + 1) % processes.size();

                            std::cout << " to process " << processes[_current_process_index].id << std::endl;

                            board.cpu.registers = processes[_current_process_index].registers;
                            board.memory.page_table = processes[_current_process_index].page_table;

                            processes[_current_process_index].state = Process::Running;
                        }

                        _cycles_passed_after_preemption = 0;
                    }
                }

                std::cout << std::endl;
            };
            board.pic.isr_3 = [&]() {
                std::cout << "Kernel: processing the first software interrupt." << std::endl;

                if (!processes.empty()) {
                    std::cout << "Kernel: unloading the process " << processes[_current_process_index].id << std::endl;
                    FreeMemory(processes[_current_process_index].memory_start_position);
                    processes.erase(processes.begin() + _current_process_index);

                    if (processes.empty()) {
                        _current_process_index = 0;

                        std::cout << "Kernel: no more processes. Stopping the board." << std::endl;

                        board.Stop();
                        } else {
                        if (_current_process_index >= processes.size()) {
                            _current_process_index %= processes.size();
                        }

                        std::cout << "Kernel: switching the context to process " << processes[_current_process_index].id << std::endl;

                        board.cpu.registers = processes[_current_process_index].registers;
                        board.memory.page_table = processes[_current_process_index].page_table;

                        processes[_current_process_index].state = Process::Running;

                        _cycles_passed_after_preemption = 0;
                    }
                }

                std::cout << std::endl;
            };
            } else if (scheduler == Priority) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Priority Queue
                //  scheduler
                ++_cycles_passed_after_preemption;
                if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) {
                    _cycles_passed_after_preemption = 0;

                    Process oldProcess = priorities.top();
                    priorities.pop();

                    oldProcess.state = Process::States::Ready;
                    --oldProcess.priority;

                    priorities.push(oldProcess);

                    Process newProcess = priorities.top();
                    priorities.pop();

                    board.cpu.registers = newProcess.registers;
                    newProcess.state = Process::States::Running;
                    ++newProcess.priority;

                    priorities.push(newProcess);
                }
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Priority
                //  Queue scheduler
                // Unload the current process
                std::cout << "Number of Processes left = " << processes.size() << std::endl;
                if (board.cpu.registers.a == 1) {
                    priorities.pop();
                    if (priorities.empty()) {
                        board.Stop();
                    }
                    else {
                        Process t = priorities.top();
                        board.cpu.registers = t.registers;
                        priorities.pop();
                        t.state = Process::States::Running;
                        priorities.push(t);
                    }
                    } else if (board.cpu.registers.a == 2) {
                    Process t = priorities.top();
                    priorities.pop();
                    t.priority = board.cpu.registers.b;
                    t.updateCycles();
                    priorities.push(t);
                }
            };
        }

        // ToDo

        // ---

        board.Start();
    }

    Kernel::~Kernel() { }

    void Kernel::CreateProcess(const std::string &name)
    {
        if (_last_issued_process_id == std::numeric_limits<Process::process_id_type>::max()) {
            std::cerr << "Kernel: failed to create a new process. The maximum number of processes has been reached." << std::endl;
            } else {
            std::ifstream input_stream(name, std::ios::in | std::ios::binary);
            if (!input_stream) {
                std::cerr << "Kernel: failed to open the program file." << std::endl;
                } else {
                Memory::ram_type ops;
                input_stream.seekg(0, std::ios::end);
                auto file_size = input_stream.tellg();
                input_stream.seekg(0, std::ios::beg);
                ops.resize(static_cast<Memory::ram_size_type>(file_size) / 4);
                input_stream.read(reinterpret_cast<char *>(&ops[0]), file_size);

                if (input_stream.bad()) {
                    std::cerr << "Kernel: failed to read the program file." << std::endl;
                    } else {
                    Memory::ram_size_type new_memory_position = AllocateMemory(ops.size()); // TODO: allocate memory for the process (AllocateMemory)
                    if (new_memory_position == -1) {
                        std::cerr << "Kernel: failed to allocate memory." << std::endl;
                        } else {
                        std::copy(ops.begin(), ops.end(), (board.memory.ram.begin() + new_memory_position));
                        Process process(_last_issued_process_id++, new_memory_position,
                        new_memory_position + ops.size());
                    }
                }
            }
        }
    }

        Memory::vmem_size_type Kernel::VirtualMemoryToPhysical(Memory::ram_size_type previous_index){
        Memory::page_index_offset_pair_type page_index_offset = board.memory.GetPageIndexAndOffsetForVirtualAddress(previous_index);
        Memory::page_entry_type page = board.memory.page_table->at(page_index_offset.first);
        Memory::page_entry_type frame = board.memory.AcquireFrame();

        if (page == Memory::INVALID_PAGE){
            processes[_current_process_index].page_table->at(previous_index) = frame;
            return frame + page_index_offset.second;
        }
        else
        {
            board.Stop();
        }
    }

    Memory::ram_size_type Kernel::AllocateMemory(Memory::ram_size_type units)
    {
        Memory::ram_size_type prev_index = _free_physical_memory_index;
        Memory::ram_size_type current_index;

        for(Memory::ram_size_type next_free_index = board.memory.ram[VirtualMemoryToPhysical(prev_index)]; ;  prev_index = next_free_index, next_free_index = board.memory.ram[next_free_index])
        {
            Memory::ram_size_type size = board.memory.ram[VirtualMemoryToPhysical(next_free_index+1)];
            if( size >= units)
            {
                if(size == units)
                {
                    board.memory.ram[VirtualMemoryToPhysical(prev_index)] = board.memory.ram[VirtualMemoryToPhysical(next_free_index)];
                }
                else
                {
                    board.memory.ram[VirtualMemoryToPhysical(next_free_index + 1)] -= units + 2;
                    next_free_index +=  board.memory.ram[VirtualMemoryToPhysical(next_free_index + 1)];
                    board.memory.ram[VirtualMemoryToPhysical(next_free_index + 1)] = units;
                }
                _free_physical_memory_index = prev_index;
                return next_free_index + 2;
            }
            if(next_free_index == _free_physical_memory_index)
            {
                return NULL;
            }
        }
        return -1;
    }

    void Kernel::FreeMemory(Memory::ram_size_type physical_memory_index)
    {
        Memory::ram_size_type previous_free_block_index = _free_physical_memory_index;
        Memory::ram_size_type current_block_index = physical_memory_index - 2;
        for(; !(current_block_index > previous_free_block_index && current_block_index < board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)]);
        previous_free_block_index =  board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)])    {
            if( previous_free_block_index >= board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)] &&
            ((current_block_index > previous_free_block_index || current_block_index < board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)]))){
                break;
            }
        }
        if(current_block_index + board.memory.ram[VirtualMemoryToPhysical(current_block_index + 1)] == board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)])
        {
            board.memory.ram[VirtualMemoryToPhysical(current_block_index)] == board.memory.ram[board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)]];
            board.memory.ram[VirtualMemoryToPhysical(current_block_index + 1)] +=  board.memory.ram[board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index + 1)]];
        }
        else
        {
            board.memory.ram[VirtualMemoryToPhysical(current_block_index)] = board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)];
        }
        if(previous_free_block_index + board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index + 1)] == current_block_index)
        {
            board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index + 1)] += board.memory.ram[VirtualMemoryToPhysical(current_block_index + 1)];
            board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)] = board.memory.ram[VirtualMemoryToPhysical(current_block_index)];
        }
        else
        {
            board.memory.ram[VirtualMemoryToPhysical(previous_free_block_index)] = current_block_index;
        }
        _free_physical_memory_index = previous_free_block_index;
    }
}
