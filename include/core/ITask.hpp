#ifndef ITASK_HPP
#define ITASK_HPP

#include <string>

namespace Engine {

class ITask {
public:
    virtual ~ITask() = default;

    virtual void execute() = 0;

    virtual std::string getName() const = 0;
};

}

#endif
