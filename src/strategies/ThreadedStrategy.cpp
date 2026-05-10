#include "../../include/strategies/ThreadedStrategy.hpp"
#include <iostream>
#include <vector>
#include <thread>

namespace Engine {

void ThreadedStrategy::run(std::shared_ptr<ITask> task, int iterations) {
    std::cout << ">>> Executing [" << task->getName() << "] via " << getStrategyName() << "..." << std::endl;
    
    std::vector<std::thread> threads;
    threads.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        threads.emplace_back([task]() {
            task->execute();
        });
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

std::string ThreadedStrategy::getStrategyName() const {
    return "Parallel (std::thread) Strategy";
}

} // namespace Engine
