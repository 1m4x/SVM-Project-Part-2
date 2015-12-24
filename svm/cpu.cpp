
#include "cpu.h"

#include <iostream>

namespace svm
{
    Registers::Registers()
    : a(0), b(0), c(0), flags(0), ip(0), sp(0) { }

    CPU::CPU(Memory &memory, PIC &pic)
    : registers(),
    _memory(memory),
    _pic(pic) { }

    CPU::~CPU() { }

    void CPU::Step()
    {
        int ip =
        registers.ip;

        int instruction =
        _memory.ram[ip];
        int data =
        _memory.ram[ip + 1];

        if (instruction ==
        CPU::MOVA_OPCODE) {
            registers.a = data;
            registers.ip += 2;
        } else if (instruction ==
        CPU::MOVB_OPCODE) {
            registers.b = data;
            registers.ip += 2;
        } else if (instruction ==
        CPU::MOVC_OPCODE) {
            registers.c = data;
            registers.ip += 2;
        } else if (instruction ==
        CPU::JMP_OPCODE) {
            registers.ip += data;
        } else if (instruction ==
        CPU::INT_OPCODE) {
          switch (data)
              {
                  case 1:
                      _pic.isr_3();
                      break;
                      //case 2:
                      //  _pic.isr_5(); // `isr_4` is reserved for page fault
                      //                // exceptions
                      //  break;
                      // ...
              }
            } else if (instruction == CPU::LDA_BASE_OPCODE)
            {
                    Memory::page_index_offset_pair_type pageOffset_i = _memory.PageOffsetForVirtual(data); //We get offset and index
                    Memory::page_entry_type frame = _memory.page_table->at(pageOffset_i.first);
                    if(frame == Memory::INVALID_PAGE)
                    {
                        int temp = registers.a; //Save register value
                        registers.a = pageOffset_i.first;
                        _pic.isr_4(); //Page fault handler call
                        registers.a = temp; //Restore register value
                    }
                    else
                    {
                        registers.a = _memory.ram[frame + pageOffset_i.second];
                        registers.ip += 2; //Increment instruction ptr
                    }
            } else if (instruction == CPU::LDB_BASE_OPCODE)
            {
                    Memory::page_index_offset_pair_type pageOffset_i = _memory.PageOffsetForVirtual(data);
                    Memory::page_entry_type frame = _memory.page_table->at(pageOffset_i.first);
                    if(frame == Memory::INVALID_PAGE)
                    {
                        int temp = registers.b;
                        registers.a = pageOffset_i.first;
                        _pic.isr_4();
                        registers.b = temp;
                    }
                    else
                    {
                        registers.b = _memory.ram[frame + pageOffset_i.second];
                        registers.ip += 2;
                    }
              } else if (instruction == CPU::LDC_BASE_OPCODE)
              {
                    Memory::page_index_offset_pair_type pageOffset_i = _memory.PageOffsetForVirtual(data);
                    Memory::page_entry_type frame = _memory.page_table->at(pageOffset_i.first);
                    if(frame == Memory::INVALID_PAGE)
                    {
                        int temp = registers.c;
                        registers.c = pageOffset_i.first;
                        _pic.isr_4();
                        registers.c = temp;
                    }
                    else
                    {
                        registers.c = _memory.ram[frame + pageOffset_i.second];
                        registers.ip += 2;
                    }
                }
              } else if (instruction == CPU::STA_BASE_OPCODE)
              {
                    Memory::page_index_offset_pair_type pageOffset_i = _memory.PageOffsetForVirtual(data);
                    Memory::page_entry_type frame =    _memory.page_table->at(pageOffset_i.first);
                    if(frame == Memory::INVALID_PAGE)
                    {
                        int temp = registers.a;
                        registers.a = pageOffset_i.first;
                        _pic.isr_4();
                        registers.a = temp;
                    }
                    else
                    {
                        _memory.ram[frame + pageOffset_i.second] = registers.a;
                        registers.ip += 2;
                    }
              } else if (instruction == CPU::STB_BASE_OPCODE)
              {
                    Memory::page_index_offset_pair_type pageOffset_i = _memory.PageOffsetForVirtual(data);
                    Memory::page_entry_type frame = _memory.page_table->at(pageOffset_i.first);
                    if(frame == Memory::INVALID_PAGE)
                    {
                        int temp = registers.b;
                        registers.b = pageOffset_i.first;
                        _pic.isr_4();
                        registers.b = temp;
                    }
                    else
                    {
                        _memory.ram[frame + pageOffset_i.second] = registers.b;
                        registers.ip += 2;
                    }
              } else if (instruction == CPU::STC_BASE_OPCODE)
              {
                    Memory::page_index_offset_pair_type pageOffset_i = _memory.PageOffsetForVirtual(data);
                    Memory::page_entry_type frame = _memory.page_table->at(pageOffset_i.first);
                    if(frame == Memory::INVALID_PAGE)
                    {
                        int temp = registers.c;
                        registers.c = pageOffset_i.first;
                        _pic.isr_4();
                        registers.c = temp;
                    }
                    else
                    {
                        _memory.ram[frame + pageOffset_i.second] = registers.c;
                        registers.ip += 2;
                    }
              } else {
            std::cerr << "CPU: invalid opcode data. Skipping..."
            << std::endl;
            registers.ip += 2;
        }
    }
}
