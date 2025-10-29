#pragma once

#include <optional>
#include <vector>

#include <SFML/System/Vector2.hpp>

enum class CellType {
    Empty,
    Obstacle,
    Start,
    Goal
};

enum class CellOverlay {
    None,
    OpenSet,
    ClosedSet,
    Path
};

class Grid {
public:
    Grid(int width, int height);

    int width() const noexcept { return m_width; }
    int height() const noexcept { return m_height; }
    int cellCount() const noexcept { return static_cast<int>(m_cells.size()); }

    bool inBounds(int x, int y) const noexcept;

    CellType cell(int x, int y) const;
    CellOverlay overlay(int x, int y) const;

    bool setObstacle(int x, int y);
    bool clearCell(int x, int y);

    bool setStart(const sf::Vector2i& cell);
    bool setGoal(const sf::Vector2i& cell);

    std::optional<sf::Vector2i> start() const noexcept { return m_start; }
    std::optional<sf::Vector2i> goal() const noexcept { return m_goal; }

    void clearOverlays();
    void setOverlay(int x, int y, CellOverlay value);

    void clearObstacles();
    void reset();

    const std::vector<CellType>& rawCells() const noexcept { return m_cells; }
    std::vector<CellOverlay>& rawOverlays() noexcept { return m_overlays; }
    const std::vector<CellOverlay>& rawOverlays() const noexcept { return m_overlays; }

private:
    int index(int x, int y) const noexcept;

    int m_width;
    int m_height;
    std::vector<CellType> m_cells;
    std::vector<CellOverlay> m_overlays;
    std::optional<sf::Vector2i> m_start;
    std::optional<sf::Vector2i> m_goal;
};
