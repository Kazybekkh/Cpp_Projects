#pragma once

#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

#include "Grid.hpp"

class Pathfinder {
public:
    enum class Status {
        Idle,
        Running,
        PathFound,
        NoPath
    };

    explicit Pathfinder(Grid& grid);

    void reset();
    bool start();
    Status status() const noexcept { return m_status; }
    Status step(int iterations = 1);

private:
    struct NodeRecord {
        float fScore;
        float gScore;
        int index;
        std::size_t sequence;
    };

    struct NodeCompare {
        bool operator()(const NodeRecord& lhs, const NodeRecord& rhs) const {
            if (lhs.fScore == rhs.fScore) {
                return lhs.sequence > rhs.sequence;
            }

            return lhs.fScore > rhs.fScore;
        }
    };

    void reconstructPath();
    float heuristic(int index) const;
    std::pair<int, int> toGrid(int index) const noexcept;

    Grid& m_grid;
    int m_width;
    int m_height;
    int m_startIndex = -1;
    int m_goalIndex = -1;

    std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> m_openSet;
    std::vector<float> m_gScores;
    std::vector<float> m_fScores;
    std::vector<int> m_parents;
    std::vector<bool> m_inOpen;
    std::vector<bool> m_inClosed;
    std::size_t m_sequenceCounter = 0;
    Status m_status = Status::Idle;
};
