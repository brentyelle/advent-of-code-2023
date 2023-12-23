#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#define UNFILLED -1

enum TileLabel {VERTICAL, HORIZONTAL, BEND_L, BEND_J, BEND_7, BEND_F, GROUND, START};
enum Direction {UP, DOWN, LEFT, RIGHT};

// ================================================================================
class Tile {
private:
    TileLabel label;
    int step;

public:
    Tile(const TileLabel l=GROUND, const int s=UNFILLED) : label(l), step(s) { return; }
    Tile(const char c, const int s=UNFILLED) {
        step = s;
        switch (c) {
            case '|':
                label = VERTICAL;
                return;
            case '-':
                label = HORIZONTAL;
                return;
            case 'L':
                label = BEND_L;
                return;
            case 'J':
                label = BEND_J;
                return;
            case '7':
                label = BEND_7;
                return;
            case 'F':
                label = BEND_F;
                return;
            case '.':
                label = GROUND;
                return;
            case 'S':
                label = START;
                return;
            default:
                label = GROUND;
                std::cerr << "Bad character! Setting as ground.\n";
                return;
        }
    }

    bool isStart() const {
        return label == START;
    }

    bool isFilled() const {
        return step != UNFILLED;
    }

    // Determines if this tile can connect to or from the given direction.
    bool connects(const Direction d) const {
        // START connects to any direction
        if (label == START) {
            return true;
        }
        // ground is never connected
        if (label == GROUND) {
            return false;
        }
        // check the directions
        switch (d) {
            case UP:
                return (label == VERTICAL || label == BEND_L || label == BEND_J);
                break;
            case DOWN:
                return (label == VERTICAL || label == BEND_7 || label == BEND_F);
                break;
            case LEFT:
                return (label == HORIZONTAL || label == BEND_7 || label == BEND_J);
                break;
            case RIGHT:
                return (label == HORIZONTAL || label == BEND_F || label == BEND_L);
                break;
            default:
                return false;
        }
    }

    // Converts tile back to a printable character, but with 'S' replaced by '@' for easier spotting.
    char symbol() const {
        switch(label) {
            case VERTICAL:
                return '|';
            case HORIZONTAL:
                return '-';
            case BEND_L:
                return 'L';
            case BEND_J:
                return 'J';
            case BEND_7:
                return '7';
            case BEND_F:
                return 'F';
            case GROUND:
                return '.';
            case START:
                return '@';
            default:
                std::cerr << "BAD SYMBOL!\n";
                return '=';
        }
    }

    int getStep() const {
        return step;
    }

    void setStep(const int s) {
        step = s;
        return;
    }

};


// ================================================================================
class Grid {
private:
    std::vector<std::vector<Tile>> tiles;
    int start_i;
    int start_j;

    void setStart() {
        for (int i{0}; i < rowCount(); i++) {
            for (int j{0}; j < colCount(); j++) {
                if (tiles.at(i).at(j).isStart()) {
                    start_i = i;
                    start_j = j;
                    //tiles.at(i).at(j).setStep(0);
                    return;
                }
            }
        }
        std::cerr << "No starting position found!\n";
    }

    int rowCount() const {
        return tiles.size();
    }

    int colCount() const {
        if (rowCount() == 0)
            return 0;
        else
            return tiles.at(0).size();
    }
   
    // Returns whether the coordinates (i,j) are valid for the vector.
    bool isValidCoord(const int i, const int j) const {
        return !(i < 0 || j < 0 || i >= rowCount() || j >= colCount());
    }

    // Returns TRUE if the stepcount at (i,j) is not UNFILLED.
    // Returns FALSE if the stepcount is UNFILLED, or if the coordinates are invalid.
    bool has_stepcount(const int i, const int j) const {
        return isValidCoord(i,j) && tiles.at(i).at(j).isFilled();
    }

    // Returns whether the stepcount at (i,j) should be calculated.
    // The input `val` 
    bool shouldCalcStep(const int i, const int j, const int val) const {
        // if coordinate is invalid, don't allow calculation of stepcount
        if (!isValidCoord(i,j)) {
            return false;
        }
        // if coordinate is valid, and step has been calculated, but stepcount is bigger than it should be, allow rewriting
        else if (has_stepcount(i,j)) {
            return tiles.at(i).at(j).getStep() > val;
        }
        // if coordinate is valid, but step has not been calculated, then should calculate
        else {
            return true;
        }
    }

    bool has_connection(const int i1, const int j1, const int i2, const int j2) const {
        bool retval = false;

        if (!isValidCoord(i1,j1) || !isValidCoord(i2, j2)) {
            retval = false;
        } else { // if valid coordinates
            const int vertical_disp   = i2 - i1;
            const int horizont_disp   = j2 - j1;
            const int manhattan_dist  = std::abs(horizont_disp) + std::abs(vertical_disp);

            // if too far, there's no way they can be connected
            if (manhattan_dist > 1) {
                retval = false;
            }
            // if it's the same tile, then there is a connection
            else if (manhattan_dist == 0) {
                std::cerr << "Manhattan distance of 0?!\n";
                retval = true;
            }
            // then it must be that exactly one of horizont_disp and vertical_disp is +1 / -1
            else {
                /* 
                std::cerr << "Checking coordinate (" << i1 << "," << j1 << ")=" << tiles.at(i1).at(j1).symbol()
                          <<            " against (" << i2 << "," << j2 << ")=" << tiles.at(i2).at(j2).symbol() << "\n";
                */

                if (vertical_disp != 0) {
                    //std::cerr << "Looking for vertical connection...\n";
                    // higher row index means further down in the table
                    const int downer    = (i2 > i1 ? i2 : i1);
                    const int upper     = (i2 > i1 ? i1 : i2);
                    //std::cerr << "Does " << tiles.at(upper).at(j1).symbol() << " connect down? -- " << (tiles.at(upper).at(j1).connects(DOWN) ? "yes" : "no") << "\n";
                    //std::cerr << "Does " << tiles.at(downer).at(j1).symbol() << " connect up? ---- " << (tiles.at(downer).at(j1).connects(UP) ? "yes" : "no") << "\n"; 
                    retval = tiles.at(upper).at(j1).connects(DOWN)   && tiles.at(downer).at(j1).connects(UP);
                } else { // if (horizont_disp != 0)
                    //std::cerr << "Looking for horizontal connection...\n";
                    // higher column index means further right in the table
                    const int righter   = (j2 > j1 ? j2 : j1);
                    const int lefter    = (j2 > j1 ? j1 : j2);
                    //std::cerr << "Does " << tiles.at(i1).at(lefter).symbol() << " connect left? --- " << (tiles.at(i1).at(lefter).connects(RIGHT) ? "yes" : "no") << "\n";
                    //std::cerr << "Does " << tiles.at(i1).at(righter).symbol() << " connect right? -- " << (tiles.at(i1).at(righter).connects(LEFT) ? "yes" : "no") << "\n";
                    retval = tiles.at(i1).at(lefter).connects(RIGHT) && tiles.at(i1).at(righter).connects(LEFT);
                }
            }
        }

        //std::cerr << "Connection " << (retval ? "found" : "not found") << "\n";

        return retval;
    }

    // For tile (i,j), calculate the stepcount value for it and all adjacent tiles, if valid and have connections.
    void markNeighbors(const int i, const int j, int value) {
        // if this tile needs calculation or re-calculation, then do it
        if (shouldCalcStep(i, j, value)) {
            tiles.at(i).at(j).setStep(value);
            int val_plus1 = value + 1;
            // for the four adjacent tiles, check if they're connected, and mark them as well (recursively)
            if (shouldCalcStep(i+1, j  , val_plus1) && has_connection(i, j, i+1, j  ))
                markNeighbors( i+1, j  , val_plus1);
            if (shouldCalcStep(i-1, j  , val_plus1) && has_connection(i, j, i-1, j  ))
                markNeighbors( i-1, j  , val_plus1);
            if (shouldCalcStep(i  , j+1, val_plus1) && has_connection(i, j, i  , j+1))
                markNeighbors( i  , j+1, val_plus1);
            if (shouldCalcStep(i  , j-1, val_plus1) && has_connection(i, j, i  , j-1))
                markNeighbors( i  , j-1, val_plus1);
        }
        // after all recursion is finished, all tiles connected to the starting tile will have been marked
        return;
    }

public:
    // allow only zero-argument constructor
    Grid() : start_i(-1), start_j(-1) { return; }
    Grid(const Grid& g) = delete;

    // Used in `main` to add a line to the object
    void addLine(const std::string &line) {
        if (line.length() == 0)
            return;

        std::vector<Tile> newTileRow;

        for (const char c : line) {
            if (c == '\n' || c == '\r')
                break;
            newTileRow.emplace_back(c, UNFILLED);
        }

        tiles.push_back(newTileRow);
        return;
    }

    // Used by the `main` function to calculate all distances from the `S` (start) tile's position.
    int calculateSteps() {
        setStart();
        markNeighbors(start_i, start_j, 0);

        int maximum = 0;
        for (const auto &row : tiles) {
            for (const Tile &tile : row) {
                if (tile.getStep() > maximum)
                    maximum = tile.getStep();
            }
        }

        return maximum;
    }

    void printTiles(const bool print_symbols=true) const {
        int tilewidth = std::ceil(std::log10(colCount())) + 1;
        for (const auto &row : tiles) {
            for (const Tile &tile : row) {
                if (print_symbols) {
                    std::cout << std::setw(tilewidth) << tile.symbol();
                } else {
                    if (tile.getStep() == UNFILLED)
                        std::cout << std::setw(tilewidth) << '.';
                    else
                        std::cout << std::setw(tilewidth) << tile.getStep();
                }
            }
            std::cout << '\n';
        }
        return;
    }
};

// ================================================================================
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./a.out <filename_to_process>" << std::endl;
        exit(1);
    }
    std::ifstream inputfile(argv[1]);
    if (!inputfile) {
        std::cout << "Error opening file " << argv[1] << "." << std::endl;
        exit(1);   
    }

    std::string thisline;
    Grid grid;

    while (getline(inputfile, thisline)) {
        grid.addLine(thisline);
    }
    inputfile.close();

    /*
    // double-checking that it was read properly
    std::cout << "-----------------------------\n";
    std::cout << " GRID JUST READ (symbols):\n";
    std::cout << "-----------------------------\n";
    grid.printTiles(true);
    */

    // calculate for part 1
    int part1answer = grid.calculateSteps();

    /*
    std::cout << "-----------------------------\n";
    std::cout << " GRID JUST READ (stepcounts):\n";
    std::cout << "-----------------------------\n";
    grid.printTiles(false);
    */
    std::cout << "-----------------------------\n";
    std::cout << "Part 1: The maximum step count is " << part1answer << std::endl;
    std::cout << "-----------------------------\n";

    return 0;
}