#include <vector>
#include <iostream>
#include <fstream>

#include "kernel.h"

namespace svm
{
    Memory::ram_type *LoadExecutable(const std::string &name)
    {
        Memory::ram_type *result =
            NULL;

        std::ifstream
            input_stream(
                name,
                std::ios::in |
                    std::ios::binary
            );

        if (input_stream) {
            result =
                new Memory::ram_type();

            input_stream.seekg(
                0, std::ios::end
            );
            auto file_size =
                input_stream.tellg();
            input_stream.seekg(
                0, std::ios::beg
            );

            result->resize(
                static_cast<Memory::ram_size_type>(file_size) / 4
            );
            input_stream.read(
                reinterpret_cast<char *>(&(*result)[0]),
                file_size
            );

            if (input_stream.bad()) {
                delete result;
                result = NULL;

                std::cerr << "SVM: failed to read the program file."
                          << std::endl;
            }
        } else {
            std::cerr << "SVM: failed to open the program file."
                      << std::endl;
        }

        return result;
    }
}

int main(int argc, char *argv[])
{
    using namespace svm;

    if (argc > 2) {
        std::string argument(argv[1]);

        Kernel::Scheduler scheduler;
        if (argument == "/scheduler:fcfs") {
            scheduler =
                Kernel::FirstComeFirstServed;
        } else if (argument == "/scheduler:sf") {
            scheduler =
                Kernel::ShortestJob;
        } else if (argument == "/scheduler:rr") {
            scheduler =
                Kernel::RoundRobin;
        } else if (argument == "/scheduler:priority") {
            scheduler =
                Kernel::Priority;
        } else {
            scheduler =
                Kernel::Undefined;
        }

        std::vector<Memory::ram_type> processes;
        for (int i = 2; i < argc; ++i) {
            Memory::ram_type *executable = LoadExecutable(argv[i]);
            if (executable) {
                processes.push_back(*executable);
                delete executable;
            }
        }

        if (scheduler == Kernel::Undefined) {
            std::cerr << "SVM: invalid scheduler selection. Exiting..."
                      << std::endl;
        } else if (processes.empty()) {
            std::cerr << "SVM: nothing to run. Exiting..."
                      << std::endl;
        } else {
            Kernel kernel(scheduler, processes);
        }
    }

    return 0;
}
