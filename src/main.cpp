#include <iostream>

#include <SFML/Graphics.hpp>

#include "Grid.hpp"
#include "Pathfinder.hpp"

namespace {
constexpr int GRID_WIDTH = 30;
constexpr int GRID_HEIGHT = 20;
constexpr int CELL_SIZE = 32;
constexpr int STEPS_PER_FRAME = 8;

sf::Vector2i mouseToCell(const sf::RenderWindow& window) {
    const sf::Vector2i pixel = sf::Mouse::getPosition(window);
    if (pixel.x < 0 || pixel.y < 0) {
        return {-1, -1};
    }
    return {pixel.x / CELL_SIZE, pixel.y / CELL_SIZE};
}

bool isCellValid(const Grid& grid, const sf::Vector2i& cell) {
    return grid.inBounds(cell.x, cell.y);
}

sf::Color baseColor(CellType type) {
    if (type == CellType::Obstacle) {
        return sf::Color(60, 60, 60);
    }
    if (type == CellType::Start) {
        return sf::Color(80, 195, 130);
    }
    if (type == CellType::Goal) {
        return sf::Color(220, 95, 95);
    }

    return sf::Color(230, 230, 230);
}

sf::Color overlayColor(CellOverlay overlay) {
    if (overlay == CellOverlay::OpenSet) {
        return sf::Color(120, 190, 255);
    }

    if (overlay == CellOverlay::ClosedSet) {
        return sf::Color(173, 216, 230);
    }

    if (overlay == CellOverlay::Path) {
        return sf::Color(255, 255, 0);
    }

    return sf::Color::Transparent;
}

void printControls() {
    std::cout << "Controls:\n";
    std::cout << "  Drag left mouse to put walls\n";
    std::cout << "  Drag right mouse to remove walls\n";
    std::cout << "  S - move start\n";
    std::cout << "  G - move goal\n";
    std::cout << "  Enter - run the search\n";
    std::cout << "  R - reset pathfinder\n";
    std::cout << "  C - clear the board\n";
}
} // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE),
                            "A* Pathfinding Visualizer",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Grid grid(GRID_WIDTH, GRID_HEIGHT);
    grid.setStart({2, 2});
    grid.setGoal({GRID_WIDTH - 3, GRID_HEIGHT - 3});

    Pathfinder pathfinder(grid);

    printControls();

    bool leftHeld = false;
    bool rightHeld = false;
    Pathfinder::Status lastStatus = Pathfinder::Status::Idle;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    leftHeld = true;
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    rightHeld = true;
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    leftHeld = false;
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    rightHeld = false;
                }
            } else if (event.type == sf::Event::KeyPressed) {
                sf::Vector2i cell = mouseToCell(window);
                if (event.key.code == sf::Keyboard::Enter) {
                    if (!pathfinder.start()) {
                        std::cout << "You need start and goal before searching.\n";
                    }
                } else if (event.key.code == sf::Keyboard::R) {
                    pathfinder.reset();
                } else if (event.key.code == sf::Keyboard::C) {
                    grid.clearObstacles();
                    pathfinder.reset();
                } else if (event.key.code == sf::Keyboard::S) {
                    if (!grid.setStart(cell)) {
                        std::cout << "Failed to place start there.\n";
                    } else {
                        pathfinder.reset();
                    }
                } else if (event.key.code == sf::Keyboard::G) {
                    if (!grid.setGoal(cell)) {
                        std::cout << "Failed to place goal there.\n";
                    } else {
                        pathfinder.reset();
                    }
                }
            }
        }

        const sf::Vector2i hovered = mouseToCell(window);
        if (leftHeld) {
            grid.setObstacle(hovered.x, hovered.y);
        }
        if (rightHeld) {
            grid.clearCell(hovered.x, hovered.y);
        }

        if (pathfinder.status() == Pathfinder::Status::Running) {
            pathfinder.step(STEPS_PER_FRAME);
        }

        const auto currentStatus = pathfinder.status();
        if (currentStatus != lastStatus) {
            if (currentStatus == Pathfinder::Status::PathFound) {
                std::cout << "Path found.\n";
            } else if (currentStatus == Pathfinder::Status::NoPath) {
                std::cout << "No path exists with current obstacles.\n";
            }
            lastStatus = currentStatus;
        }

        window.clear(sf::Color(34, 34, 44));

        sf::RectangleShape cellShape(sf::Vector2f(static_cast<float>(CELL_SIZE),
                                                  static_cast<float>(CELL_SIZE)));

        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                const CellType type = grid.cell(x, y);
                const CellOverlay overlay = grid.overlay(x, y);

                sf::Color color = baseColor(type);
                sf::Color ocolor = overlayColor(overlay);
                if (overlay != CellOverlay::None) {
                    color = ocolor;
                }

                cellShape.setPosition(static_cast<float>(x * CELL_SIZE),
                                      static_cast<float>(y * CELL_SIZE));
                cellShape.setFillColor(color);
                window.draw(cellShape);
            }
        }

        window.display();
    }

    return 0;
}
