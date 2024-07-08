#include "CellularAutomata.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <omp.h>

CellularAutomata::CellularAutomata(int width, int height, int speciesCount, NeighborhoodType neighborhood)
    : width(width), height(height), speciesCount(speciesCount), neighborhood(neighborhood) {
    grid.resize(width * height, 0);
    buffer.resize(width * height, 0);
}

void CellularAutomata::initialize(const std::vector<int>& initialState) {
    if (initialState.size() == grid.size()) {
        grid = initialState;
    }
}

void CellularAutomata::setRule(std::function<int(const std::vector<int>&)> rule) {
    this->rule = rule;
}

void CellularAutomata::step() {
    #pragma omp parallel for 
    for (int i = 0; i < this->width*this->height; i++){
        performAction();
    }
}

void CellularAutomata::performAction() {
    // Ensure thread-safety for random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, width * height - 1);
    std::uniform_int_distribution<> actionDis(0, 2);

    int action = actionDis(gen);
    //printf("Action: %d\n", action);

    int cellFrom = dis(gen);
    //printf("cellFrom: %d\n", cellFrom);
    // Get the coordinates of the selected cell
    int xFrom = cellFrom % width;
    int yFrom = cellFrom / width;
    //printf("xFrom: %d\n", xFrom);
    //printf("yFrom: %d\n", yFrom);
    //printf("grid[cellFrom]: %d\n", grid[cellFrom]);
    

    // Get the list of valid neighbors for the cell
    std::vector<std::pair<int, int>> neighbors;

    if (neighborhood == VON_NEUMANN) {
        std::vector<std::pair<int, int>> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        for (const auto& dir : directions) {
            int nx = xFrom + dir.first;
            int ny = yFrom + dir.second;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                neighbors.emplace_back(nx, ny);
            }
        }
    } else if (neighborhood == MOORE) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx != 0 || dy != 0) {
                    int nx = xFrom + dx;
                    int ny = yFrom + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        neighbors.emplace_back(nx, ny);
                    }
                }
            }
        }
    }

    // Randomly select a cellTo from the neighbors
    if (!neighbors.empty()) {
        std::uniform_int_distribution<> neighborDis(0, neighbors.size() - 1);
        int neighborIndex = neighborDis(gen);
        auto [xTo, yTo] = neighbors[neighborIndex];
        int cellTo = getIndex(xTo, yTo);
        //printf("cellTo: %d\n", cellTo);
        //printf("grid[cellTo]: %d\n", grid[cellTo]);


        // Perform the selected action
        if (action == 0) { // Move
            if (grid[cellFrom] != 0 && grid[cellTo] == 0) {
                std::swap(grid[cellFrom], grid[cellTo]);
                //this->display();
            }
        } else if (action == 1) { // Predar
            if (grid[cellFrom] != 0 && grid[cellTo] != 0) {
                int predatorSpecies = grid[cellFrom];
                int preySpecies = grid[cellTo];

                // Check for cyclic predation
                if ((predatorSpecies % speciesCount) +1 == preySpecies) {
                    grid[cellTo] = 0;
                    //this->display();
                }
            }
        } else if (action == 2) { // Reproduzir
            if (grid[cellFrom] != 0 && grid[cellTo] == 0) {
                grid[cellTo] = grid[cellFrom];
                //this->display();
            }
        }
    }
}

void CellularAutomata::display() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << grid[getIndex(x, y)] << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<int> CellularAutomata::getNeighborhood(int x, int y) const {
    std::vector<int> neighborhood;
    if (this->neighborhood == VON_NEUMANN) {
        std::vector<std::pair<int, int>> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        for (const auto& dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                neighborhood.push_back(grid[getIndex(nx, ny)]);
            }
        }
    } else if (this->neighborhood == MOORE) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx != 0 || dy != 0) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        neighborhood.push_back(grid[getIndex(nx, ny)]);
                    }
                }
            }
        }
    }
    return neighborhood;
}

int CellularAutomata::getIndex(int x, int y) const {
    return y * width + x;
}

void CellularAutomata::generateRandomInitialState() {
    std::vector<int> initialState(width * height); // Create a vector to hold the initial state of the grid
    int totalCells = width * height; // Total number of cells in the grid 

    // Define the amount of cells per species and for zero
    int cellsPerSpecies = totalCells / (speciesCount + 1); // +1 to include zero

    // Fill the vector with the appropriate number of each species and zeros
    for (int i = 0; i < speciesCount; ++i) {
        for (int j = 0; j < cellsPerSpecies; ++j) {
            initialState[i * cellsPerSpecies + j] = i + 1; // Assign species numbers (1 to speciesCount) / Atribui números das espécies (1 a speciesCount)
        }
    }

    // Fill the remaining cells with zeros
    for (int i = speciesCount * cellsPerSpecies; i < totalCells; ++i) {
        initialState[i] = 0; // Set remaining cells to zero / Define as células restantes como zero
    }

    // Shuffle the vector to create a random initial state
    std::shuffle(initialState.begin(), initialState.end(), std::mt19937{std::random_device{}()});

    initialize(initialState); // Initialize the grid with the shuffled state
}
