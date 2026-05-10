#ifndef THREADED_STRATEGY_HPP
#define THREADED_STRATEGY_HPP

#include "../core/IExecutionStrategy.hpp"

namespace Engine {

class ThreadedStrategy : public IExecutionStrategy {
public:
    void run(std::shared_ptr<ITask> task, int iterations) override;
    std::string getStrategyName() const override;
};

}

#endif
