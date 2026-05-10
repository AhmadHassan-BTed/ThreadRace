#ifndef IEXECUTION_STRATEGY_HPP
#define IEXECUTION_STRATEGY_HPP

#include "ITask.hpp"
#include <vector>
#include <memory>

namespace Engine {

class IExecutionStrategy {
public:
    virtual ~IExecutionStrategy() = default;

    virtual void run(std::shared_ptr<ITask> task, int iterations) = 0;

    virtual std::string getStrategyName() const = 0;
};

}

#endif
