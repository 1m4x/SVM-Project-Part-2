#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include "pic.h"
namespace svm
{
    // Registers
    //
    // Names and number of registers can be changed
    struct Registers
    {
        int a;
        int b;
        int c;

        int flags;

        unsigned int ip;
        unsigned int sp;

        Registers();
    };

    // CPU
    class CPU
    {
        public:
            static const int MOVA_OPCODE = 0x10,
                             MOVB_OPCODE = 0x11,
                             MOVC_OPCODE = 0x12,
                             JMP_OPCODE  = 0x20,
                             INT_OPCODE  = 0x30;

            Registers registers; // Current state of the CPU

            CPU(Memory &memory, PIC &pic);
            virtual ~CPU();

            void Step(); // Executes one instruction, advances the instruction
                         //  pointer

        private:
            Memory &_memory;
            PIC &_pic;
    };
}

#endif
