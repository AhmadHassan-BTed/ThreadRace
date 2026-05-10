#include "../../include/tasks/AckermannTask.hpp"
#include <iostream>

namespace Engine {

AckermannTask::AckermannTask(int m, int n) : m_m(m), m_n(n) {}

int AckermannTask::ackermann(int m, int n) {
    if (m == 0) return n + 1;
    if (m > 0 && n == 0) return ackermann(m - 1, 1);
    return ackermann(m - 1, ackermann(m, n - 1));
}

void AckermannTask::execute() {
    int result = ackermann(m_m, m_n);
    std::cout << "[Ackermann] Result for (" << m_m << ", " << m_n << ") = " << result << std::endl;
}

std::string AckermannTask::getName() const {
    return "Ackermann Function (" + std::to_string(m_m) + ", " + std::to_string(m_n) + ")";
}

}
