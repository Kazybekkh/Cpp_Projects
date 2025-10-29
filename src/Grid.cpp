#include "Grid.hpp"

#include <algorithm>
#include <limits>

using namespace std;

static bool sameSpot(const optional<sf::Vector2i>& a, const sf::Vector2i& b) {
    if (!a.has_value()) {
        return false;
    }
    return a->x == b.x && a->y == b.y;
}

Grid::Grid(int width, int height)
    : m_width(width),
      m_height(height),
      m_cells(static_cast<size_t>(width * height), CellType::Empty),
      m_overlays(static_cast<size_t>(width * height), CellOverlay::None) {}

bool Grid::inBounds(int x, int y) const noexcept {
    if (x < 0 || y < 0) {
        return false;
    }
    if (x >= m_width) {
        return false;
    }
    if (y >= m_height) {
        return false;
    }
    return true;
}

int Grid::index(int x, int y) const noexcept {
    return y * m_width + x;
}

CellType Grid::cell(int x, int y) const {
    const int idx = index(x, y);
    return m_cells[idx];
}

CellOverlay Grid::overlay(int x, int y) const {
    const int idx = index(x, y);
    return m_overlays[idx];
}

bool Grid::setObstacle(int x, int y) {
    if (!inBounds(x, y)) {
        return false;
    }

    const int idx = index(x, y);
    if (m_cells[idx] == CellType::Start || m_cells[idx] == CellType::Goal) {
        return false;
    }
    m_cells[idx] = CellType::Obstacle;
    return true;
}

bool Grid::clearCell(int x, int y) {
    if (!inBounds(x, y)) {
        return false;
    }

    const int idx = index(x, y);
    const auto currentType = m_cells[idx];
    m_cells[idx] = CellType::Empty;

    if (currentType == CellType::Start) {
        m_start = nullopt;
    } else if (currentType == CellType::Goal) {
        m_goal = nullopt;
    }

    return true;
}

bool Grid::setStart(const sf::Vector2i& cellPos) {
    if (!inBounds(cellPos.x, cellPos.y)) {
        return false;
    }

    const int idx = index(cellPos.x, cellPos.y);
    if (m_cells[idx] == CellType::Obstacle) {
        return false;
    }

    if (sameSpot(m_goal, cellPos)) {
        return false;
    }

    if (m_start.has_value()) {
        const int oldIdx = index(m_start->x, m_start->y);
        m_cells[oldIdx] = CellType::Empty;
    }

    m_start = cellPos;
    m_cells[idx] = CellType::Start;
    return true;
}

bool Grid::setGoal(const sf::Vector2i& cellPos) {
    if (!inBounds(cellPos.x, cellPos.y)) {
        return false;
    }

    const int idx = index(cellPos.x, cellPos.y);
    if (m_cells[idx] == CellType::Obstacle) {
        return false;
    }

    if (sameSpot(m_start, cellPos)) {
        return false;
    }

    if (m_goal.has_value()) {
        const int oldIdx = index(m_goal->x, m_goal->y);
        m_cells[oldIdx] = CellType::Empty;
    }

    m_goal = cellPos;
    m_cells[idx] = CellType::Goal;
    return true;
}

void Grid::clearOverlays() {
    for (size_t i = 0; i < m_overlays.size(); ++i) {
        m_overlays[i] = CellOverlay::None;
    }
}

void Grid::setOverlay(int x, int y, CellOverlay value) {
    if (!inBounds(x, y)) {
        return;
    }

    const int idx = index(x, y);
    if (m_cells[idx] == CellType::Start || m_cells[idx] == CellType::Goal) {
        return;
    }
    m_overlays[idx] = value;
}

void Grid::clearObstacles() {
    for (size_t i = 0; i < m_cells.size(); ++i) {
        if (m_cells[i] == CellType::Obstacle) {
            m_cells[i] = CellType::Empty;
        }
    }
}

void Grid::reset() {
    for (size_t i = 0; i < m_cells.size(); ++i) {
        m_cells[i] = CellType::Empty;
    }
    for (size_t i = 0; i < m_overlays.size(); ++i) {
        m_overlays[i] = CellOverlay::None;
    }
    m_start = nullopt;
    m_goal = nullopt;
}
