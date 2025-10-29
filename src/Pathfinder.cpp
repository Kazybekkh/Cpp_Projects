#include "Pathfinder.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr float INF = std::numeric_limits<float>::infinity();
} // namespace

Pathfinder::Pathfinder(Grid& grid)
    : m_grid(grid),
      m_width(grid.width()),
      m_height(grid.height()),
      m_gScores(static_cast<std::size_t>(grid.cellCount()), INF),
      m_fScores(static_cast<std::size_t>(grid.cellCount()), INF),
      m_parents(static_cast<std::size_t>(grid.cellCount()), -1),
      m_inOpen(static_cast<std::size_t>(grid.cellCount()), false),
      m_inClosed(static_cast<std::size_t>(grid.cellCount()), false) {}

void Pathfinder::reset() {
    const int cellCount = m_grid.cellCount();
    if (static_cast<int>(m_gScores.size()) != cellCount) {
        m_gScores.resize(static_cast<std::size_t>(cellCount), INF);
        m_fScores.resize(static_cast<std::size_t>(cellCount), INF);
        m_parents.resize(static_cast<std::size_t>(cellCount), -1);
        m_inOpen.resize(static_cast<std::size_t>(cellCount), false);
        m_inClosed.resize(static_cast<std::size_t>(cellCount), false);
    }

    for (int i = 0; i < cellCount; ++i) {
        m_gScores[static_cast<std::size_t>(i)] = INF;
        m_fScores[static_cast<std::size_t>(i)] = INF;
        m_parents[static_cast<std::size_t>(i)] = -1;
        m_inOpen[static_cast<std::size_t>(i)] = false;
        m_inClosed[static_cast<std::size_t>(i)] = false;
    }

    m_openSet = {};
    m_sequenceCounter = 0;
    m_startIndex = -1;
    m_goalIndex = -1;
    m_status = Status::Idle;
    m_grid.clearOverlays();
}

bool Pathfinder::start() {
    const auto startCell = m_grid.start();
    const auto goalCell = m_grid.goal();

    if (!startCell.has_value() || !goalCell.has_value()) {
        return false;
    }

    reset();
    m_grid.clearOverlays();

    if (startCell->x == goalCell->x && startCell->y == goalCell->y) {
        m_status = Status::PathFound;
        return true;
    }

    m_startIndex = startCell->y * m_width + startCell->x;
    m_goalIndex = goalCell->y * m_width + goalCell->x;

    m_gScores[m_startIndex] = 0.0f;
    m_fScores[m_startIndex] = heuristic(m_startIndex);
    m_parents[m_startIndex] = -1;

    m_openSet.push(NodeRecord{m_fScores[m_startIndex], 0.0f, m_startIndex, m_sequenceCounter++});
    m_inOpen[m_startIndex] = true;
    m_status = Status::Running;
    return true;
}

Pathfinder::Status Pathfinder::step(int iterations) {
    if (m_status != Status::Running) {
        return m_status;
    }

    if (iterations < 1) {
        iterations = 1;
    }

    const int cellCount = m_grid.cellCount();

    for (int performed = 0; performed < iterations && m_status == Status::Running; ++performed) {
        if (m_openSet.empty()) {
            m_status = Status::NoPath;
            return m_status;
        }

        NodeRecord current = m_openSet.top();
        m_openSet.pop();

        if (current.index < 0 || current.index >= cellCount) {
            continue;
        }

        if (m_inClosed[current.index]) {
            continue;
        }

        if (current.gScore > m_gScores[current.index]) {
            continue;
        }

        m_inOpen[current.index] = false;

        if (current.index == m_goalIndex) {
            reconstructPath();
            m_status = Status::PathFound;
            return m_status;
        }

        const auto [cx, cy] = toGrid(current.index);
        if (current.index != m_startIndex) {
            m_grid.setOverlay(cx, cy, CellOverlay::ClosedSet);
        }

        constexpr int offsets[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        for (const auto& offset : offsets) {
            const int nx = cx + offset[0];
            const int ny = cy + offset[1];

            if (!m_grid.inBounds(nx, ny)) {
                continue;
            }

            const int neighborIndex = ny * m_width + nx;

            if (m_inClosed[neighborIndex]) {
                continue;
            }

            const auto cellType = m_grid.cell(nx, ny);
            if (cellType == CellType::Obstacle) {
                continue;
            }

            const float tentativeG = m_gScores[current.index] + 1.0f;
            if (tentativeG < m_gScores[neighborIndex]) {
                m_parents[neighborIndex] = current.index;
                m_gScores[neighborIndex] = tentativeG;
                m_fScores[neighborIndex] = tentativeG + heuristic(neighborIndex);
                m_openSet.push(NodeRecord{m_fScores[neighborIndex], tentativeG, neighborIndex, m_sequenceCounter++});
                m_inOpen[neighborIndex] = true;

                if (neighborIndex != m_goalIndex && neighborIndex != m_startIndex) {
                    m_grid.setOverlay(nx, ny, CellOverlay::OpenSet);
                }
            }
        }

        m_inClosed[current.index] = true;
    }

    return m_status;
}

void Pathfinder::reconstructPath() {
    int current = m_goalIndex;
    if (current < 0) {
        return;
    }

    while (current >= 0) {
        const auto [px, py] = toGrid(current);
        if (current != m_startIndex) {
            m_grid.setOverlay(px, py, CellOverlay::Path);
        }
        current = m_parents[current];
    }
}

float Pathfinder::heuristic(int index) const {
    const auto goalCell = m_grid.goal();
    if (!goalCell.has_value()) {
        return 0.0f;
    }

    const auto [x, y] = toGrid(index);
    return static_cast<float>(std::abs(goalCell->x - x) + std::abs(goalCell->y - y));
}

std::pair<int, int> Pathfinder::toGrid(int index) const noexcept {
    const int x = index % m_width;
    const int y = index / m_width;
    return {x, y};
}
