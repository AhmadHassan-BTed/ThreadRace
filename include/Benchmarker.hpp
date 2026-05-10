#ifndef BENCHMARKER_HPP
#define BENCHMARKER_HPP

#include "core/IExecutionStrategy.hpp"
#include <chrono>

namespace Engine {

class Benchmarker {
public:
    Benchmarker(std::shared_ptr<IExecutionStrategy> strategy);

    void runBenchmark(std::shared_ptr<ITask> task, int iterations);

private:
    std::shared_ptr<IExecutionStrategy> m_strategy;
};

} // namespace Engine

#endif // BENCHMARKER_HPP
