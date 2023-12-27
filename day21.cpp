#include <iostream>
#include <vector>
#include <unordered_map>
#define LIMIT_STEPS 26501365
#define DUMMYCHAR 'y'

typedef long long int myint;

enum class TILE {START, GARDEN, ROCK, INVALID};

class Indices {
public:
    myint x;
    myint y;

    Indices(const myint xx=0, const myint yy=0) : x(xx), y(yy) { return; }

    bool operator==(const Indices& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
};

template <> struct std::hash<Indices> {
    std::size_t operator()(const Indices &ij) const {
        using std::size;
        using std::hash;

        return hash<myint>()(ij.x) ^ hash<myint>()(ij.y << 1);
    }
};

class TileType {
private:
    TILE tile;

public:
    TileType(const char c) {
        switch (c) {
            case 'S':
                tile = TILE::START;
                break;
            case '.':
                tile = TILE::GARDEN;
                break;
            case '#':
                tile = TILE::ROCK;
                break;
            default:
                tile = TILE::INVALID;
                break;
        }
        return;
    }

    char to_char() const {
        switch (tile) {
            case TILE::START:
                return 'S';
            case TILE::GARDEN:
                return '.';
            case TILE::ROCK:
                return '#';
            default:
                return '=';
        }
    }

    bool is_traversible() const {
        return (tile == TILE::GARDEN || tile == TILE::START);
    }

    bool is_start() const {
        return (tile == TILE::START);
    }

    TILE terrain_type() const {
        return tile;
    }

    static bool is_valid(const char c) {
        switch (c) {
            case 'S':
                return true;
            case '.':
                return true;
            case '#':
                return true;
            default:
                return false;
        }
    }
};

class TileArray {
private:
    std::vector<std::vector<TileType>> tiles;
    std::unordered_map<Indices, char> reached_inds;

public:
    TileArray() {
        return;
    }

    TileArray(const TileArray& rhs) {
        tiles = rhs.tiles;
        reached_inds = rhs.reached_inds;
        return;
    }

    Indices dimensions() const {
        Indices indices;
        indices.x = tiles.size();
        if (indices.x != 0) {
            indices.y = tiles.at(0).size();
        } else {
            indices.y = 0;
        }
        return indices;
    }

    TileType& tiles_loopat(const myint ti, const myint tj) {
        const Indices tiles_max = dimensions();
        myint ti_fixed = ti % tiles_max.x;
        myint tj_fixed = tj % tiles_max.y;
        if (ti_fixed < 0)
            ti_fixed += tiles_max.x;
        if (tj_fixed < 0)
            tj_fixed += tiles_max.y;
        return tiles.at(ti_fixed).at(tj_fixed);
    }

    const TileType& tiles_loopat_const(const myint ti, const myint tj) const {
        const Indices tiles_max = dimensions();
        myint ti_fixed = ti % tiles_max.x;
        myint tj_fixed = tj % tiles_max.y;
        if (ti_fixed < 0)
            ti_fixed += tiles_max.x;
        if (tj_fixed < 0)
            tj_fixed += tiles_max.y;
        return tiles.at(ti_fixed).at(tj_fixed);
    }

    myint count_reached() const {
        return reached_inds.size();
    }

    bool can_traverse(const Indices &ij) const {
        return is_valid_coord(ij) && tiles.at(ij.x).at(ij.y).is_traversible();
    }

    bool can_traverse_loop(const Indices &ij) const {
        return tiles_loopat_const(ij.x, ij.y).is_traversible();
    }

    bool is_valid_coord(const Indices& inds) const {
        const Indices tiles_max = dimensions();
        return (inds.x >= 0 && inds.y >= 0 && inds.x < tiles_max.x && inds.y < tiles_max.y);
    }

    void reset_to_start() {
        const Indices tiles_max = dimensions();
        reached_inds.clear();
        printf("Dimensions are %lld-by-%lld\n", tiles_max.x, tiles_max.y);

        bool continuefor = true;
        for (myint ti{0}; continuefor && (ti < tiles_max.x); ti++) {
            for (myint tj{0}; continuefor && (tj < tiles_max.y); tj++) {
                if (tiles.at(ti).at(tj).is_start()) {
                    reached_inds.emplace(Indices(ti, tj), DUMMYCHAR);
                    continuefor = false;
                    printf("Found start position: (%lld,%lld)\n", ti, tj);
                }
            }
        }

        return;
    }

    void read_file(FILE* const inputfile) {
        tiles.clear();

        char curr_char;
        myint curr_row = 0;

        rewind(inputfile);
        curr_char = fgetc(inputfile);

        // begin row of index 0
        tiles.emplace_back();

        while (curr_char != EOF) {
            //printf("Just found character [%c]\n", curr_char);
            //printf("Its validity is %s\n", TileType::is_valid(curr_char) ? "true" : "false");

            // assuming we find a good character, then add it to the `tiles` array
            if (TileType::is_valid(curr_char)) {
                tiles.at(curr_row).emplace_back(curr_char);
            }
            // if we find a newline character, go to the next line
            else if (curr_char == '\n') {
                curr_row++;
                tiles.emplace_back();
            }
            // skip any carriage returns
            else if (curr_char == '\r') {
            }
            // otherwise, something went horribly wrong
            else {
                std::cerr << "read_file() : PANIC over character " << static_cast<int>(curr_char) << '\n';
            }
            // then get the next character from the file
            curr_char = fgetc(inputfile);
        }

        printf("Scanned %lu valid chars in total.\n", tiles.size() * tiles.at(0).size());
        return;
    }

    void print_tiles() const;
    void step_once();
    void step_once_loop();

    void step(const myint n=1, bool please_print=false) {
        for (myint i=1; i <= n; i++) {
            step_once();
            if (please_print) {
                printf("==================================\n");
                printf("Result after %lld iterations:\n\n", i);
                print_tiles();
                printf("\nCurrently %lld possibilities for places to reach.\n", count_reached());
            }
        }
        return;
    }

    void step_loop(const myint n=1, bool please_print=false) {
        for (myint i=1; i <= n; i++) {
            step_once_loop();
            if (please_print) {
                printf("==================================\n");
                printf("Result after %lld iterations:\n\n", i);
                print_tiles();
                printf("\nCurrently %lld possibilities for places to reach.\n", count_reached());
            }
        }
        return;
    }

    friend bool has_entry(const std::unordered_map<Indices, char> &inds, const myint i, const myint j);
    friend bool has_entry(const std::unordered_map<Indices, char> &inds, const Indices& ij);
};

// ====================================================

bool has_entry(const std::unordered_map<Indices, char> &inds, const myint i, const myint j) {
    return (inds.find(Indices(i,j)) != inds.end());
}
bool has_entry(const std::unordered_map<Indices, char> &inds, const Indices& ij) {
    return (inds.find(ij) != inds.end());
} 

void TileArray::print_tiles() const {
    const Indices tiles_max = dimensions();

    for (myint i=0; i < tiles_max.x; i++) {
        for (myint j=0; j < tiles_max.y; j++) {
            if (has_entry(reached_inds, i, j)) {
                std::cout << 'O';
            } else {
                std::cout << tiles.at(i).at(j).to_char();
            }
        }
        std::cout << '\n';
    }

    return;
}

void TileArray::step_once() {
    std::unordered_map<Indices, char> new_reached_inds;
    new_reached_inds.reserve(reached_inds.size());

    // for each index we reached last step, try to step to that index's adjacent places
    for (const auto& indexpair : reached_inds) {
        const Indices    up(indexpair.first.x - 1, indexpair.first.y    );
        const Indices  down(indexpair.first.x + 1, indexpair.first.y    );
        const Indices  left(indexpair.first.x    , indexpair.first.y + 1);
        const Indices right(indexpair.first.x    , indexpair.first.y - 1);

        
        if (is_valid_coord(up   ) && !has_entry(new_reached_inds, up   ) && can_traverse(up  ) ) {
            new_reached_inds.emplace(up,    DUMMYCHAR);
        }
        if (is_valid_coord(down ) && !has_entry(new_reached_inds, down ) && can_traverse(down) ) {
            new_reached_inds.emplace(down,  DUMMYCHAR);
        }
        if (is_valid_coord(left ) && !has_entry(new_reached_inds, left ) && can_traverse(left) ) {
            new_reached_inds.emplace(left,  DUMMYCHAR);
        }
        if (is_valid_coord(right) && !has_entry(new_reached_inds, right) && can_traverse(right)) {
            new_reached_inds.emplace(right, DUMMYCHAR);
        }
    }

    // swap the new indices into the object's attribute `reached_inds`
    reached_inds.swap(new_reached_inds);
    // the old reached_inds will be deleted when leaving scope
    return;
}

void TileArray::step_once_loop() {
    std::unordered_map<Indices, char> new_reached_inds;

    // for each index we reached last step, try to step to that index's adjacent places
    for (const auto& indexpair : reached_inds) {
        const Indices    up(indexpair.first.x - 1, indexpair.first.y    );
        const Indices  down(indexpair.first.x + 1, indexpair.first.y    );
        const Indices  left(indexpair.first.x    , indexpair.first.y + 1);
        const Indices right(indexpair.first.x    , indexpair.first.y - 1);
        
        if (!has_entry(new_reached_inds, up) && can_traverse_loop(up)) {
            new_reached_inds.emplace(up, DUMMYCHAR);
        }
        if (!has_entry(new_reached_inds, down) && can_traverse_loop(down)) {
            new_reached_inds.emplace(down, DUMMYCHAR);
        }
        if (!has_entry(new_reached_inds, left) && can_traverse_loop(left)) {
            new_reached_inds.emplace(left, DUMMYCHAR);
        }
        if (!has_entry(new_reached_inds, right) && can_traverse_loop(right)) {
            new_reached_inds.emplace(right, DUMMYCHAR);
        }
    }

    // swap the new indices into the object's attribute `reached_inds`
    reached_inds.swap(new_reached_inds);
    // the old reached_inds will be deleted when leaving scope
    return;
}

// ====================================================

int main(const int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: $ ./a.out <filename> <num_iterations>\n";
        exit(1);
    }

    const char* FILENAME = argv[1];
    FILE* const infile = fopen(FILENAME, "r");

    if (!infile) {
        std::cerr << "Error opening file \"" << argv[1] << "\"\n";
        exit(1);
    }

    TileArray all_tiles;
    all_tiles.read_file(infile);
    // Part 1:
    all_tiles.reset_to_start();
    printf("----------------------------------\n");
    all_tiles.step(atoi(argv[2]), false);
    
    std::cout << "Part 1: The answer is " << all_tiles.count_reached() << std::endl;
    printf("==================================\n");

    // Part 2:
    all_tiles.reset_to_start();
    printf("----------------------------------\n");
    myint counter=0;
    while (all_tiles.count_reached() < LIMIT_STEPS) {
        counter++;
        all_tiles.step_once_loop();
        if (counter % 10 == 0) {
            printf("Reached %lld-th iteration! Current count_reached is %lld\n", counter, all_tiles.count_reached());
        }
    }
    std::cout << "Part 2: The count_reached first crosses " << LIMIT_STEPS << " in the " << counter << "th generation." << std::endl;
    printf("==================================\n");

    exit(0);
}
