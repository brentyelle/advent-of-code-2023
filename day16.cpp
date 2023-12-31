#include <cstdio>
#include <vector>
#include <string>
#include <functional>
#define MAX_ITERATIONS 100000

enum SqEnum {
    EMPTY,
    MIRROR_FWD,
    MIRROR_BCK,
    SPLIT_HORZ,
    SPLIT_VERT,
    INVALID
};

enum DirecEnum {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Converts a `DirecEnum` type into a nicely-printable `std::string` type.
std::string direc2string(const DirecEnum dir) {
    switch (dir) {
        case UP:
            return std::string("UP");
        case DOWN:
            return std::string("DOWN");
        case LEFT:
            return std::string("LEFT");
        case RIGHT:
            return std::string("RIGHT");
    }
    return std::string("ERROR");
}

struct BeamCoord;
struct Energization;
struct Square;
class Contraption;

// ============================================

// Models the current position (`x` and `y`) and movement direction (`dir`) of a beam of light.
struct BeamCoord {
    int x;
    int y;
    DirecEnum dir;

    BeamCoord() :
    x(0), y(0), dir(RIGHT)
    {
        return;
    }

    BeamCoord(const int x_init, const int y_init, const DirecEnum d_init) : 
    x(x_init), y(y_init), dir(d_init)
    {
        return;
    }

    // Adjusts the beam of light for when a mirror is struck. The type of mirror is to be passed in as the sole parameter.
    void reflect(const SqEnum mirror_type) {
        switch (dir) {
            case UP:
                dir = (mirror_type == MIRROR_FWD ? RIGHT : LEFT);
                break;
            case DOWN:
                dir = (mirror_type == MIRROR_FWD ? LEFT : RIGHT);
                break;
            case LEFT:
                dir = (mirror_type == MIRROR_FWD ? DOWN : UP);
                break;
            case RIGHT:
                dir = (mirror_type == MIRROR_FWD ? UP : DOWN);
                break;
        }
        return;
    }
};

// Models the energization state of a tile on the `Contraption`. Although being "energized" is unrelated to any direction, the `Energization` type keeps track of which way(s) it has been energized in order to properly cull beams bouncing around the `Contraption`.
struct Energization {
    bool by_up    {false};
    bool by_down  {false};
    bool by_left  {false};
    bool by_right {false};

    bool isOnByAny() const {
        return by_up || by_down || by_left || by_right;
    }

    bool isOnBy(const DirecEnum dir) const {
        switch (dir) {
            case UP:
                return by_up;
            case DOWN:
                return by_down;
            case LEFT:
                return by_left;
            case RIGHT:
                return by_right;
        }
        return false;
    }

    void resetOff() {
        by_up    = false;
        by_down  = false;
        by_left  = false;
        by_right = false;
        return;
    }

    void turnOn(const BeamCoord& beam) {
        switch (beam.dir) {
            case UP:
                by_up = true;
                break;
            case DOWN:
                by_down = true;
                break;
            case LEFT:
                by_left = true;
                break;
            case RIGHT:
                by_right = true;
                break;
        }
        return;
    }

    // to be used in the `statusHash` function of `Contraption`. Creates a unique 4-character string for the current energization state.
    std::string energyHash() const {
        std::string out_str("");
        out_str += (by_up    ? "U" : "u");
        out_str += (by_down  ? "D" : "d");
        out_str += (by_left  ? "L" : "l");
        out_str += (by_right ? "R" : "r");
        return out_str;
    }
};

// ============================================

// Models a single tile in the array of the `Contraption`. Each `Square` keeps track of its `SqEnum type` (what kind of terrain it is) and its `Energization energy` (see above).
struct Square {
public:
    SqEnum type;
    Energization energy;

    Square() :
    type(INVALID)
    {
        return;
    }

    Square(const SqEnum sqenum) :
    type(sqenum)
    {
        return;
    }

    Square(const char ch) {
        switch (ch) {
            case '.':
                type = EMPTY;
                break;
            case '/':
                type = MIRROR_FWD;
                break;
            case '\\':
                type = MIRROR_BCK;
                break;
            case '-':
                type = SPLIT_HORZ;
                break;
            case '|':
                type = SPLIT_VERT;
                break;
            default:
                printf("Invalid character in `Square` constructor: [%c]\n", ch);
                type = INVALID;
                break;
        }
        return;
    }

    char toChar() const {
        char to_return = ' ';
        if (energy.isOnByAny()) {
            to_return = '#';
        } else {
            switch (type) {
                case EMPTY:
                    to_return = '.';
                    break;
                case MIRROR_FWD:
                    to_return = '/';
                    break;
                case MIRROR_BCK:
                    to_return = '\\';
                    break;
                case SPLIT_HORZ:
                    to_return = '-';
                    break;
                case SPLIT_VERT:
                    to_return = '|';
                    break;
                case INVALID:
                    printf("Invalid `type` value %d\n", static_cast<int>(type));
                    to_return = '?';
                    break;
            }
        }
        return to_return;
    }
};

// ============================================

// Models the entire grid of `Square`s in `grid`, as well as all of the `beams` bouncing around it.
class Contraption {
private:
    std::vector<std::vector<Square>> grid;
    std::vector<BeamCoord> beams;

public:
    Contraption()
    {
        return;
    }

    int rowCt() const {
        return grid.size();
    }

    int colCt() const {
        if (grid.size() == 0) {
            return 0;
        } else {
            return grid[0].size();
        }
    }

    // Reads in all data from the given file into the `grid`. Assumes Part 1 is going to be done, so also sets up the initial `(0,0,RIGHT)` beam in `beams`.
    void loadfile(FILE* inputfile) {
        grid.clear();
        beams.clear();
        //add an initial row
        grid.emplace_back();

        char currChar = fgetc(inputfile);
        int rowCount  = 0;
        while (currChar != EOF) {
            if (currChar == '\n') {
                rowCount++;
                grid.emplace_back();
            } else if (currChar == '\r') {
                //do nothing
            } else {
                grid.at(rowCount).emplace_back(currChar);
            }
            currChar = fgetc(inputfile);
        }

        beams.emplace_back(0, 0, RIGHT);
    }

    void print() const {
        for (const auto& row : grid) {
            for (const Square& square : row) {
                printf("%c", square.toChar());
            }
            printf("\n");
        }
        printf("\n");
        return;
    }

    // Clears out the entire `beams` vector, sets all squares in the `grid` to be non-energized, and sets up a new initial beam (determined by the arguments).
    void resetBeam(const int reset_x=0, const int reset_y=0, const DirecEnum reset_direc=RIGHT) {
        // delete all existing beams, and reset the original single beam
        beams.clear();
        beams.emplace_back(reset_x, reset_y, reset_direc);

        // reset all squares to non-energy
        for (auto& row : grid) {
            for (Square& square : row) {
                square.energy.resetOff();
            }
        }
        return;
    }

    // Clears out the entire `beams` vector, sets all squares in the `grid` to be non-energized, and sets up a new initial beam (determined by the argument).
    void resetBeam(const BeamCoord& beam) {
        resetBeam(beam.x, beam.y, beam.dir);
        return;
    }

    bool isValidCoord(const int i, const int j) const {
        return i >= 0 && j >= 0 && i < rowCt() && j < colCt();
    }

    // Allows for access to the elements of `grid` in a more concise way; `const` overloaded.
    const Square& gridAt(const int i, const int j) const {
        return grid.at(i).at(j);
    }

    // Allows for access to the elements of `grid` in a more concise way; non-`const` overloaded.
    Square& gridAt(const int i, const int j) {
        return grid.at(i).at(j);
    }

    // Culls all unnecessary beams from the `beams` attribute vector. A beam can be deemed "unnecessary" in two ways: it can go out of bounds (easily checked by looking at the coordinates), or it can be following a path that has already been tread by another beam (which can be known via the directions in the `energy` of the `grid`-square that the beam is currently traversing.)
    int cullBeams() {
        int number_culled = 0;
        int beamCt = beams.size();
        // iterate backwards, since the tail's indices shift down after each deletion
        for (int i = beamCt-1; i >= 0; i--) {
            bool isBadBeam = (!isValidCoord(beams[i].x, beams[i].y)) || gridAt(beams[i].x, beams[i].y).energy.isOnBy(beams[i].dir);
            if (isBadBeam) {
                beams.erase(beams.begin() + i);
                number_culled++;
            }
        }
        //printf("Culled %d beams.\n", number_culled);
        return number_culled;
    }

    // Advance each beam ahead by a single unit in its currently-moving direction. No bounds-checking is done in this step; that is taken care of by calling `cullBeams` afterward.
    void advance() {
        for (BeamCoord& beam : beams) {
            switch (beam.dir) {
                case UP:
                    beam.x--;
                    break;
                case DOWN:
                    beam.x++;
                    break;
                case LEFT:
                    beam.y--;
                    break;
                case RIGHT:
                    beam.y++;
                    break;
            }
        }
        return;
    }

    // Simulates one "tick" of time in the `Contraption`:
    //  (1) Each beam energizes its current square.
    //  (2) Beams on mirrors change their direction, and beams on splitters split. Directions are changed, but not positions.
    //  (3) `advance` is called: Each beam moves along its direction by one unit.
    //  (4) `cullBeams` is called: Unnecessary beams are culled, preventing wasteful memory consumption and ensuring that the algorithm will halt.
    void tick() {
        // because we calculate `beamCt` beforehand, all elements appended to `beams` during this for-loop will NOT be included in the looping, which is my intention
        const int beamCt = beams.size();
        for (int n=0; n < beamCt; n++) {
            /*
            // if somehow an invalid beam slipped through the cracks, skip it, and it should get deleted next time
            if (!isValidCoord(beams[n].x, beams[n].y)) {
                fprintf(stderr, "Somehow, a beam avoided culling! Index %d.\n", n);
                continue;
            }
            */
            // mark the beam's location as energized.
            gridAt(beams[n].x, beams[n].y).energy.turnOn(beams[n]);
            // depending on what's at the current location, do something
            switch (gridAt(beams[n].x, beams[n].y).type) {
                case EMPTY:
                    break;
                case MIRROR_FWD:
                    beams[n].reflect(MIRROR_FWD);
                    break;
                case MIRROR_BCK:
                    beams[n].reflect(MIRROR_BCK);
                    break;
                case SPLIT_HORZ:
                    if (beams[n].dir == UP || beams[n].dir == DOWN) {
                        beams[n].dir = LEFT; //original beam becomes one of the splits
                        beams.emplace_back(beams[n].x, beams[n].y, RIGHT);
                    } 
                    break;
                case SPLIT_VERT:
                    if (beams[n].dir == LEFT || beams[n].dir == RIGHT) {
                        beams[n].dir = UP; //original beam becomes one of the splits
                        beams.emplace_back(beams[n].x, beams[n].y, DOWN);
                    }
                    break;
                case INVALID:
                    printf("INVALID found in `tick` function! Index %d\n", n);
                    break;
            }
        }
        advance();
        cullBeams();
        return;
    }

    // Creates a hash of the current state of the `Contraption`, taking into account all of the energy states of the squares in the `grid`.
    size_t statusHash() const {
        std::string str_to_hash("");

        for (const auto& row : grid) {
            for (const Square& square : row) {
                //str_to_hash += std::to_string(static_cast<int>(square.type));
                str_to_hash += square.energy.energyHash();
            }
        }

        return std::hash<std::string>{}(str_to_hash);
    }

    // Runs `tick()` until a stable end state is achieved.
    void shoot() {
        size_t prev_hash = statusHash();
        size_t curr_hash = 0;
        for (int i=0; prev_hash != curr_hash && i < MAX_ITERATIONS; i++) {
            tick();
            prev_hash = curr_hash;
            curr_hash = statusHash();
            if (prev_hash == curr_hash) {
                break;
            }
        }
        return;
    }

    // To be run after `shoot()` concludes. Counts up the number of squares in the `grid` that are energized.
    int energyCount() const {
        int energ_ct=0;
        for (const auto& row : grid) {
            for (const Square& square : row) {
                if (square.energy.isOnByAny()) {
                    energ_ct++;
                }
            }
        }
        return energ_ct;
    }

    // Part 2 only: Creates a `std::vector` of all possible beams starting from an edge position.
    std::vector<BeamCoord> generateEdgeBeams() const {
        std::vector<BeamCoord> out_vec;
        int max_row = rowCt() - 1;
        int max_col = colCt() - 1;

        // I originally had these grouped, but it made the output considerably harder to follow. At least it only ever runs once.
        for (int i=0; i <= max_row; i++) {
            out_vec.emplace_back(i, 0,       RIGHT);  // left edge
        }
        for (int i=0; i <= max_row; i++) {
            out_vec.emplace_back(i, max_col, LEFT);   // right edge
        }
        for (int j=0; j <= max_col; j++) {
            out_vec.emplace_back(0,       j, DOWN);   // top edge
        }
        for (int j=0; j <= max_col; j++) {
            out_vec.emplace_back(max_row, j, UP);     // bottom edge
        }

        return out_vec;
    }
};

// Part 2 only: Given a `Contraption`, finds the best edge-position beam, where "best" is defined to be the one that gives the highest value for `energyCount()`.
BeamCoord findBestBeam(Contraption &contrap) {
    int maxEnergy = 0;
    BeamCoord bestbeam;

    for (const BeamCoord &startBeam : contrap.generateEdgeBeams()) {
        contrap.resetBeam(startBeam);
        contrap.shoot();
        int this_energy = contrap.energyCount();

        printf("Shot beam from (%d,%d), final energy value of %d... ", startBeam.x, startBeam.y, this_energy);
        // contrap.print();

        if (this_energy > maxEnergy)
        {
            bestbeam = startBeam;
            maxEnergy = this_energy;
            printf("New maximum!");
        }
        printf("\n");
    }

    return bestbeam;
}

// ============================================

int main(const int argc, const char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: $ ./a.out <filename>\n");
        return 1;
    }

    FILE* infile = fopen(argv[1], "r");
    if (!infile) {
        fprintf(stderr, "Error opening file %.64s\n", argv[1]);
        return 1;
    }

    Contraption contrap;
    contrap.loadfile(infile);
    fclose(infile);

    printf("Contraption setup, before any beams are fired:\n");
    contrap.print();
    printf("====================================\n");

    printf("Energized contraption after shooting RIGHT from (0,0):\n");
    contrap.shoot();
    contrap.print();
    printf("Part 1: Energization count is %d.\n", contrap.energyCount());

    printf("====================================\n");
    BeamCoord bestbeam = findBestBeam(contrap);

    printf(" - - - - - - - - - - - - -\n");
    printf("Optimally-energized contraption for Part 2:\n");
    contrap.resetBeam(bestbeam);
    contrap.shoot();
    contrap.print();
    printf("Part 2: Optimized energization count is %d, shooting %s from (%d,%d).\n",
        contrap.energyCount(), direc2string(bestbeam.dir).c_str(), bestbeam.x, bestbeam.y);
    return 0;
}
