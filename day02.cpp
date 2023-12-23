#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#define GAME_SPACE_LEN 5
#define SKIP_SPACING 2

const int MAX_REDS = 12;
const int MAX_GREENS = 13;
const int MAX_BLUES = 14;

const char* const FILENAME = "day2_input.txt";

typedef struct {
    int blue_ct{0};
    int red_ct{0};
    int green_ct{0};
} RoundInfo_t;

RoundInfo_t interpretRoundStr(const std::string &round_str);
std::vector<RoundInfo_t> interpretGameStr(const std::string &game_str);

void partOneSolution(std::ifstream &inputfile);
int getGameNumber(const std::string &game_str);
bool isValidRound(const RoundInfo_t &round_info);

void partTwoSolution(std::ifstream &inputfile);
RoundInfo_t calculateNeededColors(const std::vector<RoundInfo_t> &round_info_vec);
int calculateColorPower(const RoundInfo_t &game_info);

int main() {
    std::ifstream inputfile(FILENAME);
    if (!inputfile) {
        std::cerr << "Error opening file " << FILENAME << "." << std::endl;
        exit(1);
    }

    // file is already at beginning for Part 1
    partOneSolution(inputfile);

    // reset file to look at beginning for Part 2
    inputfile.clear();
    inputfile.seekg(0);
    partTwoSolution(inputfile);

    inputfile.close();

    return 0;
}

void partOneSolution(std::ifstream &inputfile)
{
    std::string thisgame_str; // holds lines grabbed from file
    int sum_valid_game_ids = 0;   // sum of all valid game IDs

    while (getline(inputfile, thisgame_str))
    { // loop through all lines in the file
        // to hold whether or not the game is valid
        bool thisgame_isvalid = true;
        // split the string for the game into a vector of rounds
        for (auto round_info : interpretGameStr(thisgame_str)) {
            thisgame_isvalid = thisgame_isvalid && isValidRound(round_info);
        }
        // if all rounds of the game are valid, add the game id to the total
        if (thisgame_isvalid) {
            // std::cerr << "valid game -- #" << getGameNumber(thisgame_str) << "\n\n";
            sum_valid_game_ids += getGameNumber(thisgame_str);
            //} else {
            //    std::cerr << "invalid game -- #" << getGameNumber(thisgame_str) << "\n\n";
        }
    }

    std::cout << "Part 1 solution is: " << sum_valid_game_ids << std::endl;
    return;
}

void partTwoSolution(std::ifstream &inputfile)
{

    std::string thisgame_str{}; // holds lines grabbed from file
    int sum_game_powers = 0;

    while (getline(inputfile, thisgame_str)) {
        sum_game_powers += calculateColorPower(calculateNeededColors(interpretGameStr(thisgame_str)));
    }
    
    std::cout << "Part 2 solution is: " << sum_game_powers << std::endl;
    return;
}

// given a game string, grabs the ID of the game
//  e.g.,   "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"
//          --> 1
int getGameNumber(const std::string &game_str) {
    //std::cerr << "calculated game #" << stoi(game_str.substr(GAME_SPACE_LEN, game_str.find_first_of(':') - GAME_SPACE_LEN )) << "\n";

    return stoi(
        game_str.substr(
            GAME_SPACE_LEN,
            game_str.find_first_of(':') - GAME_SPACE_LEN ) );
}

/*  given a game string, splits the string into a vector of substrings, then interprets those substrings
    e.g.,   "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"
            -->     ["3 blue, 4 red", "1 red, 2 green, 6 blue", "2 green"]
            -->-->  [{red_ct : 3, green_ct : 0, blue_ct : 3},
                     {red_ct : 1, green_ct : 2, blue_ct : 6},
                     {red_ct : 0, green_ct : 2, blue_ct : 0}]
    This last step is achieved by applying the `interpretRoundStr` function to each round's string.    */
std::vector<RoundInfo_t> interpretGameStr(const std::string &game_str) {
    std::vector<std::string> round_str_vec;
    std::vector<RoundInfo_t> round_info_vec;

    std::string remaining_game_str = game_str.substr(game_str.find_first_of(':') + SKIP_SPACING);  // trim the "Game ###:" info

    while (!remaining_game_str.empty()) {
        size_t next_semicolon_index = remaining_game_str.find_first_of(';');
        if (next_semicolon_index != std::string::npos) {
            // store remaining string up to but not including next ';'
            round_str_vec.emplace_back(remaining_game_str.substr(0, next_semicolon_index));
            // then, skip past the "; "
            remaining_game_str = remaining_game_str.substr(next_semicolon_index + SKIP_SPACING);
        } else {
            round_str_vec.emplace_back(remaining_game_str);
            remaining_game_str = ""; // trigger loop end
        }
    }

    for (const std::string round_str : round_str_vec) {
        round_info_vec.emplace_back(interpretRoundStr(round_str));
    }
    
    return round_info_vec;
}

// given a round_str string, interprets the string into a RoundInfo_t-type struct containing that round_str's info
//  e.g.,   "3 blue, 4 red"
//          --> {blue_ct : 3, red_ct : 4, green_ct : 0}
RoundInfo_t interpretRoundStr(const std::string &round_str) {
    std::vector<std::string> round_str_vec;         // to temporarily separate round into components
    std::string remaining_round_str = round_str;    // to temporarily hold pieces of the string, to separate it properly

    while (!remaining_round_str.empty()) {
        size_t next_comma_index = remaining_round_str.find_first_of(',');
        if (next_comma_index != std::string::npos) {
            // store remaining string up to but not including next ','
            round_str_vec.emplace_back(remaining_round_str.substr(0, next_comma_index));
            // then, skip past the ", "
            remaining_round_str = remaining_round_str.substr(next_comma_index + SKIP_SPACING);
        } else {
            round_str_vec.emplace_back(remaining_round_str);
            remaining_round_str = ""; // trigger loop end
        }
    }

    RoundInfo_t round_info; // return value

    for (const std::string color_ct_name : round_str_vec) {
        // identify what color is being mentioned, and update the corresponding count
        if (color_ct_name.find("blue") != std::string::npos) {
            round_info.blue_ct = std::stoi(color_ct_name);
        } else if (color_ct_name.find("red") != std::string::npos) {
            round_info.red_ct = std::stoi(color_ct_name);
        } else if (color_ct_name.find("green") != std::string::npos) {
            round_info.green_ct = std::stoi(color_ct_name);
        } else { // just to double check
            std::cerr << "interpretRoundStr : BAD COLOR [" << color_ct_name << "]" << std::endl;
        }
    }

    return round_info;
}

RoundInfo_t calculateNeededColors(const std::vector<RoundInfo_t> &round_info_vec) {
    RoundInfo_t maxes;

    for (const RoundInfo_t round : round_info_vec) {
        maxes.red_ct    = round.red_ct   > maxes.red_ct   ? round.red_ct   : maxes.red_ct;
        maxes.green_ct  = round.green_ct > maxes.green_ct ? round.green_ct : maxes.green_ct;
        maxes.blue_ct   = round.blue_ct  > maxes.blue_ct  ? round.blue_ct  : maxes.blue_ct;
    }

    return maxes;
}

// if a round is valid (all of its marble counts are within the maxima)
bool isValidRound(const RoundInfo_t &round_info) {
    return (round_info.red_ct   <= MAX_REDS  &&
            round_info.green_ct <= MAX_GREENS &&
            round_info.blue_ct  <= MAX_BLUES);
}

int calculateColorPower(const RoundInfo_t &game_info) {
    return game_info.red_ct * game_info.green_ct * game_info.blue_ct;
}
