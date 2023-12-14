#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

typedef enum {
    DIGIT, PERIOD, SYMBOL
} CharID_t;

class LineInfo_t {
public:
    std::string str;            // the line in string form
    std::vector<CharID_t> ids;  // each character in the line, identified

    LineInfo_t() : str("") { return; }
    LineInfo_t(std::string s, std::vector<CharID_t> is) : str(s), ids(is) { return; }
};

class ArrayNum_t {
public:
    int row;                // row that the number is found at
    int start_col;          // column index of number's first digit
    int end_col;            // column index of number's last digit
    std::string num_str;    // ASCII form of number (use std::stoi'd to make numeric)
    bool is_adj_symbol;     // is adjacent to symbol

    ArrayNum_t() : row(-1), start_col(-1), end_col(-1), num_str(""), is_adj_symbol(false) { return; }
};

bool isDigit(const char c);
std::vector<CharID_t> parseForCharIDs(const std::string &inputline);
std::vector<ArrayNum_t> parseForArrayNums(const int this_row_i, const std::vector<LineInfo_t> &all_lines_info);
std::vector<ArrayNum_t> confirmAdjecentToSymbol(std::vector<ArrayNum_t> &found_numbers, const LineInfo_t &this_line, const int this_row_i, const std::vector<LineInfo_t> &all_lines_info);
std::vector<ArrayNum_t> findAllArrayNums(const LineInfo_t &this_line, const int this_row_i);
std::vector<CharID_t> charVecSlice(const std::vector<CharID_t> &vec, const int from, const int to);
bool charVecHas(const std::vector<CharID_t> &vec, const CharID_t &val);
std::string printRowOfIDs(const std::vector<CharID_t> &lineinfo);

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
    std::vector<LineInfo_t> all_lines_info;
    std::vector<ArrayNum_t> all_ANs;

    while (getline(inputfile, thisline)) {
        // trim newline and carriage-return characters
        if (thisline[thisline.length()-1] == '\n' || thisline[thisline.length()-1] == '\r') {
            thisline = thisline.substr(0, thisline.length() - 1);
        }
        // classify each character in the line as a CharID: DIGIT, PERIOD, or SYMBOL
        all_lines_info.emplace_back(thisline, parseForCharIDs(thisline));
    }

    // Part 1: For each line, grab its ArrayNumbers (identified for adjacency/non-adjacency to SYMBOLs) and store them in `all_ANs`
    for (int row_ct=0; row_ct < all_lines_info.size(); row_ct++) {
        std::vector<ArrayNum_t> thisline_ANs = parseForArrayNums(row_ct, all_lines_info);
        // concatenate the just-grabbed ANs onto the accumulator
        all_ANs.insert(std::end(all_ANs), std::begin(thisline_ANs), std::end(thisline_ANs));
    }

    // Part 1: Sum up all ArrayNumbers that were identified as being adjacent to a SYMBOL, ignore the rest
    int sum = 0;
    for (const ArrayNum_t an : all_ANs) {
        if (an.is_adj_symbol)
            sum += std::stoi(an.num_str);
    }

    std::cout << "Part 1: Sum of all part-numbers is: " << sum << std::endl;
    return 0;
}

/* ---------------------------------
Hopefully self-explanatory.
--------------------------------- */
bool isDigit(const char c) {
    return (c >= '0' && c <= '9');
}

/* ---------------------------------
Given a single line of the input, identifies each character as either DIGIT, PERIOD, or SYMBOL (an enum).
--------------------------------- */
std::vector<CharID_t> parseForCharIDs(const std::string &inputline) {
    std::vector<CharID_t> symbol_ids;

    for (const auto c : inputline) {
        if (isDigit(c)) {
            symbol_ids.push_back(DIGIT);
        } else if (c == '.') {
            symbol_ids.push_back(PERIOD);
        } else {
            symbol_ids.push_back(SYMBOL);
        }
    }

    return symbol_ids;
}


/* ---------------------------------
The meat-and-potatoes of Part 1
--------------------------------- */
std::vector<ArrayNum_t> parseForArrayNums(const int this_row_i, const std::vector<LineInfo_t> &all_lines_info) {
    const LineInfo_t &this_line = all_lines_info[this_row_i];   // for readability
    std::vector<ArrayNum_t> all_array_nums = findAllArrayNums(this_line, this_row_i);
    return confirmAdjecentToSymbol(all_array_nums, this_line, this_row_i, all_lines_info);
}

/* ---------------------------------
Extracts all numbers found in `this_line` and returns a vector of ArrayNum_t of all of them.
However, this then needs to be passed to `confirmAdjacentToSymbol` to check if they are indeed adjacent to symbols.
--------------------------------- */
std::vector<ArrayNum_t> findAllArrayNums(const LineInfo_t &this_line, const int this_row_i) {
    std::vector<ArrayNum_t> found_numbers;                      // will hold all ArrayNumbers that are found
    ArrayNum_t new_AN;                                          // will temporarily hold new ArrayNumber as we find it, to be pushed into `found_numbers`
    bool finding_number = false;                                // whether we're currently in the process of building a number from found digits

    for (int j = 0; j < this_line.str.length(); j++)
    {
        // if we're not mid-number, but we find a digit, start seeking through for more digits
        if (!finding_number && this_line.ids[j] == DIGIT)
        {
            finding_number = true;
            new_AN.row = this_row_i;
            new_AN.start_col = j;
            new_AN.end_col = j;
            new_AN.num_str = "";
            new_AN.is_adj_symbol = false;
        }

        // if we're mid-number and find another digit, extend the ArrayNumber to include that digit, too
        if (finding_number && this_line.ids[j] == DIGIT)
        {
            new_AN.num_str += this_line.str[j];
            new_AN.end_col = j;
        }

        // if we're mid-number and find a non-digit, we've reached the end of the number we were finding, so add it to `found_numbers`
        if (finding_number && this_line.ids[j] != DIGIT)
        {
            finding_number = false;
            found_numbers.push_back(new_AN);
        }
    }

    // if we were mid-number and reached the end of the line, add that number to `found_numbers`
    if (finding_number)
    {
        // finding_number = false;
        found_numbers.push_back(new_AN);
    }

    return found_numbers;
}

/* ---------------------------------
Given the list `found_numbers`, adjusts each element's `.is_adj_symbol` ("is adjacent to SYMBOL") boolean value by looking around for adjacentSYMBOLs in the `all_lines_info` table on the same row.
--------------------------------- */
std::vector<ArrayNum_t> confirmAdjecentToSymbol(std::vector<ArrayNum_t> &found_numbers, const LineInfo_t &this_line, const int this_row_i, const std::vector<LineInfo_t> &all_lines_info)
{
    // identify which of the found numbers are adjacent to symbols
    for (int k = 0; k < found_numbers.size(); k++)
    {
        const ArrayNum_t an = found_numbers.at(k);
        const int an_l = an.start_col;
        const int an_r = an.end_col;

        // check preceding char, same line
        if (an_l > 0 && this_line.ids[an_l - 1] == SYMBOL)
            found_numbers[k].is_adj_symbol = true;
        // check following char, same line
        else if (an_r + 1 < this_line.ids.size() && this_line.ids[an_r + 1] == SYMBOL)
            found_numbers[k].is_adj_symbol = true;
        // check line above (directly above or diagonally above any digit)
        else if (this_row_i > 0 && charVecHas(charVecSlice(all_lines_info[this_row_i - 1].ids, an_l - 1, an_r + 2), SYMBOL))
            found_numbers[k].is_adj_symbol = true;
        // check line below (directly below or diagonally below any digit)
        else if (this_row_i + 1 < all_lines_info.size() && charVecHas(charVecSlice(all_lines_info[this_row_i + 1].ids, an_l - 1, an_r + 2), SYMBOL))
            found_numbers[k].is_adj_symbol = true;
        // not really necessary, since false is default, but it helps me sleep better at night to have this here
        else
            found_numbers[k].is_adj_symbol = false;
    }

    return found_numbers;
}



/* ---------------------------------
Do some Python-esque slicing of the given vector of CharIDs, but with guards to prevent out-of-bounds access.
--------------------------------- */
std::vector<CharID_t> charVecSlice(const std::vector<CharID_t> &vec, const int from, const int to) {
    std::vector<CharID_t> sliced_vec;
    const int fixed_from = std::max(from, 0);
    const int fixed_to   = std::min(to, static_cast<int>(vec.size()));
    for (int i=fixed_from; i < fixed_to; i++) {
        sliced_vec.push_back(vec.at(i));
    }

    return sliced_vec;
}

/* ---------------------------------
Determine whether a vector of CharIDs has the given `search_val` CharID in it.
--------------------------------- */
bool charVecHas(const std::vector<CharID_t> &vec, const CharID_t &search_val) {
    for (const CharID_t elem : vec) {
        if (elem == search_val)
            return true;
    }
    return false;
}

/* ---------------------------------
For debugging: Prints all of the CharIDs in a vector.
--------------------------------- */
std::string printRowOfIDs(const std::vector<CharID_t> &lineinfo) {
    std::string string_to_print;
    for (const CharID_t char_id : lineinfo) {
        if (char_id == DIGIT)  string_to_print += '#';
        if (char_id == SYMBOL) string_to_print += '*';
        if (char_id == PERIOD) string_to_print += '.';
    }

    string_to_print += '\n';
    return string_to_print;
}
