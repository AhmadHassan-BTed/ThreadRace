#include "../include/Benchmarker.hpp"
#include <iostream>
#include <iomanip>

namespace Engine {

Benchmarker::Benchmarker(std::shared_ptr<IExecutionStrategy> strategy) 
    : m_strategy(strategy) {}

void Benchmarker::runBenchmark(std::shared_ptr<ITask> task, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    m_strategy->run(task, iterations);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << " BENCHMARK RESULTS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " Task:     " << task->getName() << std::endl;
    std::cout << " Strategy: " << m_strategy->getStrategyName() << std::endl;
    std::cout << " Iterations: " << iterations << std::endl;
    std::cout << " Total Time: " << std::fixed << std::setprecision(4) << elapsed.count() << " seconds" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

} // namespace Engine
