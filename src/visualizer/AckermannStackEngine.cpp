#include "../../include/visualizer/AckermannStackEngine.hpp"
#include <algorithm>

namespace Engine {

AckermannStackEngine::AckermannStackEngine() {
    reset();
}

void AckermannStackEngine::reset() {
    m_m = 0;
    m_n = 0;
    m_stack.clear();
    m_nodes.clear();
    m_rootNodeId = -1;
    m_nextId = 1;
    m_stepCount = 0;
    m_maxDepth = 0;
    m_isCompleted = false;
    m_finalResult = 0;
}

std::string AckermannStackEngine::getHyperOpName(int m) const {
    switch (m) {
        case 0: return "Successor (n + 1)";
        case 1: return "Addition (n + 2)";
        case 2: return "Multiplication (2n + 3)";
        case 3: return "Exponentiation (2^(n+3) - 3)";
        case 4: return "Tetration (2 ^^ (n+3) - 3)";
        default: return "Hyper-Operation Level " + std::to_string(m);
    }
}

void AckermannStackEngine::start(int m, int n) {
    reset();
    m_m = m;
    m_n = n;

    int rootId = m_nextId++;
    AckermannNode rootNode;
    rootNode.id = rootId;
    rootNode.parentId = -1;
    rootNode.m = m;
    rootNode.n = n;
    rootNode.depth = 0;
    rootNode.status = NodeStatus::ACTIVE;
    rootNode.result = 0;
    rootNode.subWidth = 50.0f;

    m_nodes[rootId] = rootNode;
    m_rootNodeId = rootId;

    StackFrame initialFrame;
    initialFrame.nodeId = rootId;
    initialFrame.m = m;
    initialFrame.n = n;
    initialFrame.stage = FrameStage::INIT;
    initialFrame.innerResult = 0;

    m_stack.push_back(initialFrame);
    m_maxDepth = 1;

    notify(EngineEventType::START, rootId, -1, 0, 0);
}

void AckermannStackEngine::notify(EngineEventType type, int nodeId, int parentId, int depth, int result) {
    if (m_eventCallback) {
        EngineEvent ev{ type, nodeId, parentId, depth, result };
        m_eventCallback(ev);
    }
}

bool AckermannStackEngine::step() {
    if (m_isCompleted || m_stack.empty()) return false;

    m_stepCount++;
    StackFrame& frame = m_stack.back();
    AckermannNode& node = m_nodes[frame.nodeId];

    int currentDepth = static_cast<int>(m_stack.size());
    if (currentDepth > m_maxDepth) m_maxDepth = currentDepth;

    if (frame.stage == FrameStage::INIT) {
        // Base case: m == 0 -> n + 1
        if (frame.m == 0) {
            int res = frame.n + 1;
            node.result = res;
            node.status = NodeStatus::RESOLVED;
            int nodeDepth = node.depth;
            int nodeId = node.id;

            m_stack.pop_back();
            notify(EngineEventType::RESOLVE, nodeId, -1, nodeDepth, res);

            if (!m_stack.empty()) {
                receiveResult(m_stack.back(), res);
            } else {
                m_isCompleted = true;
                m_finalResult = res;
                notify(EngineEventType::COMPLETE, nodeId, -1, nodeDepth, res);
            }
            return true;
        }

        // Base case: m > 0 && n == 0 -> A(m - 1, 1)
        if (frame.n == 0) {
            frame.stage = FrameStage::WAITING_INNER;
            int childId = m_nextId++;
            AckermannNode childNode;
            childNode.id = childId;
            childNode.parentId = node.id;
            childNode.m = frame.m - 1;
            childNode.n = 1;
            childNode.depth = node.depth + 1;
            childNode.status = NodeStatus::ACTIVE;

            node.children.push_back(childId);
            m_nodes[childId] = childNode;

            StackFrame childFrame;
            childFrame.nodeId = childId;
            childFrame.m = frame.m - 1;
            childFrame.n = 1;
            childFrame.stage = FrameStage::INIT;
            childFrame.innerResult = 0;

            m_stack.push_back(childFrame);
            notify(EngineEventType::PUSH, childId, node.id, childNode.depth, 0);
            return true;
        }

        // Recursive case: m > 0 && n > 0 -> A(m, n - 1)
        frame.stage = FrameStage::WAITING_INNER;
        int childId = m_nextId++;
        AckermannNode childNode;
        childNode.id = childId;
        childNode.parentId = node.id;
        childNode.m = frame.m;
        childNode.n = frame.n - 1;
        childNode.depth = node.depth + 1;
        childNode.status = NodeStatus::ACTIVE;

        node.children.push_back(childId);
        m_nodes[childId] = childNode;

        StackFrame childFrame;
        childFrame.nodeId = childId;
        childFrame.m = frame.m;
        childFrame.n = frame.n - 1;
        childFrame.stage = FrameStage::INIT;
        childFrame.innerResult = 0;

        m_stack.push_back(childFrame);
        notify(EngineEventType::PUSH, childId, node.id, childNode.depth, 0);
        return true;
    }

    return true;
}

void AckermannStackEngine::receiveResult(StackFrame& parentFrame, int childResult) {
    AckermannNode& parentNode = m_nodes[parentFrame.nodeId];

    if (parentFrame.m > 0 && parentFrame.n == 0) {
        parentNode.result = childResult;
        parentNode.status = NodeStatus::RESOLVED;
        int pId = parentNode.id;
        int pDepth = parentNode.depth;

        m_stack.pop_back();
        notify(EngineEventType::RESOLVE, pId, -1, pDepth, childResult);

        if (!m_stack.empty()) {
            receiveResult(m_stack.back(), childResult);
        } else {
            m_isCompleted = true;
            m_finalResult = childResult;
            notify(EngineEventType::COMPLETE, pId, -1, pDepth, childResult);
        }
    } else if (parentFrame.stage == FrameStage::WAITING_INNER) {
        parentFrame.stage = FrameStage::WAITING_OUTER;
        parentFrame.innerResult = childResult;

        int outerId = m_nextId++;
        AckermannNode outerNode;
        outerNode.id = outerId;
        outerNode.parentId = parentNode.id;
        outerNode.m = parentFrame.m - 1;
        outerNode.n = childResult;
        outerNode.depth = parentNode.depth + 1;
        outerNode.status = NodeStatus::ACTIVE;

        parentNode.children.push_back(outerId);
        m_nodes[outerId] = outerNode;

        StackFrame outerFrame;
        outerFrame.nodeId = outerId;
        outerFrame.m = parentFrame.m - 1;
        outerFrame.n = childResult;
        outerFrame.stage = FrameStage::INIT;
        outerFrame.innerResult = 0;

        m_stack.push_back(outerFrame);
        notify(EngineEventType::PUSH, outerId, parentNode.id, outerNode.depth, 0);
    } else if (parentFrame.stage == FrameStage::WAITING_OUTER) {
        parentNode.result = childResult;
        parentNode.status = NodeStatus::RESOLVED;
        int pId = parentNode.id;
        int pDepth = parentNode.depth;

        m_stack.pop_back();
        notify(EngineEventType::RESOLVE, pId, -1, pDepth, childResult);

        if (!m_stack.empty()) {
            receiveResult(m_stack.back(), childResult);
        } else {
            m_isCompleted = true;
            m_finalResult = childResult;
            notify(EngineEventType::COMPLETE, pId, -1, pDepth, childResult);
        }
    }
}

int AckermannStackEngine::stepN(int count) {
    int executed = 0;
    for (int i = 0; i < count; ++i) {
        if (m_isCompleted) break;
        if (step()) executed++;
        else break;
    }
    return executed;
}

Metrics AckermannStackEngine::getMetrics() const {
    Metrics m;
    m.m = m_m;
    m.n = m_n;
    m.stepCount = m_stepCount;
    m.currentDepth = static_cast<int>(m_stack.size());
    m.maxDepth = m_maxDepth;
    m.totalNodesCount = static_cast<int>(m_nodes.size());
    m.isCompleted = m_isCompleted;
    m.result = m_finalResult;
    m.hyperOpName = getHyperOpName(m_m);
    return m;
}

}
