#ifndef ACKERMANN_TASK_HPP
#define ACKERMANN_TASK_HPP

#include "../core/ITask.hpp"

namespace Engine {

/**
 * Ackermann function - A classic example of a highly recursive function.
 * Used here to simulate heavy computational load.
 */
class AckermannTask : public ITask {
public:
    AckermannTask(int m, int n);
    void execute() override;
    std::string getName() const override;

private:
    int m_m;
    int m_n;
    int ackermann(int m, int n);
};

}

#endif
