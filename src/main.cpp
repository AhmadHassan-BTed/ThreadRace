#include "../include/Benchmarker.hpp"
#include "../include/tasks/AckermannTask.hpp"
#include "../include/strategies/SequentialStrategy.hpp"
#include "../include/strategies/ThreadedStrategy.hpp"
#include <iostream>
#include <memory>
#include <string>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <mode> [iterations]" << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  1: Sequential Execution" << std::endl;
    std::cout << "  2: Threaded Execution" << std::endl;
    std::cout << "Default iterations: 3" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    int mode = std::stoi(argv[1]);
    int iterations = (argc >= 3) ? std::stoi(argv[2]) : 3;

    std::shared_ptr<Engine::IExecutionStrategy> strategy;

    if (mode == 1) {
        strategy = std::make_shared<Engine::SequentialStrategy>();
    } else if (mode == 2) {
        strategy = std::make_shared<Engine::ThreadedStrategy>();
    } else {
        std::cerr << "Error: Invalid mode " << mode << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    auto task = std::make_shared<Engine::AckermannTask>(4, 1);
    Engine::Benchmarker bench(strategy);
    
    bench.runBenchmark(task, iterations);

    return 0;
}
