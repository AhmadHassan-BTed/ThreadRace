#ifndef ACKERMANN_STACK_ENGINE_HPP
#define ACKERMANN_STACK_ENGINE_HPP

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>

namespace Engine {

enum class NodeStatus { ACTIVE, RESOLVED };
enum class FrameStage { INIT, WAITING_INNER, WAITING_OUTER };

struct AckermannNode {
    int id;
    int parentId;
    int m;
    int n;
    int depth;
    NodeStatus status;
    int result;
    std::vector<int> children;
    float x;
    float y;
    float targetX;
    float targetY;
    float subWidth;

    AckermannNode() : id(0), parentId(-1), m(0), n(0), depth(0),
                      status(NodeStatus::ACTIVE), result(0),
                      x(0.0f), y(0.0f), targetX(0.0f), targetY(0.0f), subWidth(50.0f) {}
};

struct StackFrame {
    int nodeId;
    int m;
    int n;
    FrameStage stage;
    int innerResult;
};

struct Metrics {
    int m;
    int n;
    long long stepCount;
    int currentDepth;
    int maxDepth;
    int totalNodesCount;
    bool isCompleted;
    int result;
    std::string hyperOpName;
};

enum class EngineEventType { START, PUSH, RESOLVE, COMPLETE };

struct EngineEvent {
    EngineEventType type;
    int nodeId;
    int parentId;
    int depth;
    int result;
};

class AckermannStackEngine {
public:
    AckermannStackEngine();
    
    void start(int m, int n);
    bool step();
    int stepN(int count);
    void reset();

    Metrics getMetrics() const;
    const std::map<int, AckermannNode>& getNodes() const { return m_nodes; }
    const std::vector<StackFrame>& getStack() const { return m_stack; }
    int getRootNodeId() const { return m_rootNodeId; }

    typedef std::function<void(const EngineEvent&)> EventCallback;
    void setEventCallback(EventCallback cb) { m_eventCallback = cb; }

private:
    void receiveResult(StackFrame& parentFrame, int childResult);
    void notify(EngineEventType type, int nodeId = -1, int parentId = -1, int depth = 0, int result = 0);
    std::string getHyperOpName(int m) const;

    int m_m;
    int m_n;
    std::vector<StackFrame> m_stack;
    std::map<int, AckermannNode> m_nodes;
    int m_rootNodeId;
    int m_nextId;
    long long m_stepCount;
    int m_maxDepth;
    bool m_isCompleted;
    int m_finalResult;
    EventCallback m_eventCallback;
};

}

#endif
