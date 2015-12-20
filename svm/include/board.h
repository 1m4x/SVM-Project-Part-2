#ifndef BOARD_H
#define BOARD_H

#include "memory.h"
#include "pic.h"
#include "pit.h"
#include "cpu.h"

namespace svm
{
    // Virtual Machine
    //
    // Combines all components (CPU, memory, timer, interrupt controller)
    // Orchestrates their execution
    class Board
    {
        public:
            Memory memory;
            PIC pic;
            PIT pit;
            CPU cpu;

            Board();
            virtual ~Board();

            void Start(); // Starts the cpu, timer, etc.
            void Stop();  // Stops...

        private:
            bool _working;
    };
}

#endif
