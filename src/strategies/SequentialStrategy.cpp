#include "../../include/strategies/SequentialStrategy.hpp"
#include <iostream>

namespace Engine {

void SequentialStrategy::run(std::shared_ptr<ITask> task, int iterations) {
    std::cout << ">>> Executing [" << task->getName() << "] via " << getStrategyName() << "..." << std::endl;
    for (int i = 0; i < iterations; ++i) {
        task->execute();
    }
}

std::string SequentialStrategy::getStrategyName() const {
    return "Sequential (Single-Threaded) Strategy";
}

}
