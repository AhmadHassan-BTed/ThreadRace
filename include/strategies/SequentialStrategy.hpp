#ifndef SEQUENTIAL_STRATEGY_HPP
#define SEQUENTIAL_STRATEGY_HPP

#include "../core/IExecutionStrategy.hpp"

namespace Engine {

class SequentialStrategy : public IExecutionStrategy {
public:
    void run(std::shared_ptr<ITask> task, int iterations) override;
    std::string getStrategyName() const override;
};

}

#endif
