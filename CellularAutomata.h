#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include <vector>
#include <functional>

enum NeighborhoodType {
    VON_NEUMANN,
    MOORE
};

class CellularAutomata {
public:
    CellularAutomata(int width, int height, int speciesCount, NeighborhoodType neighborhood);

    void initialize(const std::vector<int>& initialState);
    void setRule(std::function<int(const std::vector<int>&)> rule);
    void step();
    void display() const;
    void generateRandomInitialState();
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCell(int x, int y) const { return grid[getIndex(x, y)]; }
    void performAction();


private:
    int width, height, speciesCount;
    NeighborhoodType neighborhood;
    std::vector<int> grid, buffer;
    std::function<int(const std::vector<int>&)> rule;
    
    std::vector<int> getNeighborhood(int x, int y) const;
    int getIndex(int x, int y) const;
};

#endif // CELLULARAUTOMATA_H
