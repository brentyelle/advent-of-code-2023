#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

//const char* const FILENAME = "day3_input.txt";

typedef enum {
    DIGIT, PERIOD, SYMBOL
} Character_t;

class ArrayNum_t {
public:
    int row;                // row that the number is found at
    int start_col;          // column index of number's first digit
    int end_col;            // column index of number's last digit
    std::string num_str;    // ASCII form of number (use std::stoi'd to make numeric)
    bool is_adj_symbol;     // is adjacent to symbol

    ArrayNum_t() : row(-1), start_col(-1), end_col(-1), num_str(""), is_adj_symbol(true) { return; }
};

class LineInfo_t {
public:
    std::string str;
    std::vector<Character_t> ids;

    LineInfo_t() : str("") { return; }
    LineInfo_t(std::string s, std::vector<Character_t> is) : str(s), ids(is) { return; }
};

bool isDigit(const char c);
std::vector<Character_t> identifyInputLine(const std::string &inputline);
std::vector<ArrayNum_t> identifyNumbers(const int this_row_i, const std::vector<LineInfo_t> &all_lines_info);
std::vector<Character_t> vecSlice(const std::vector<Character_t> &vec, const int from, const int to);
bool vecHas(const std::vector<Character_t> &vec, const Character_t &val);
std::string printRowOfIDs(const std::vector<Character_t> &linfo);

int main(int argc, char* argv[]) {
    std::ifstream inputfile(argv[1]);
    if (!inputfile) {
        std::cout << "Error opening file " << argv[1] << "." << std::endl;
        exit(1);   
    }
    std::cout << "File opened successfully!" << std::endl;

    std::string thisline;
    std::vector<LineInfo_t> all_lines_info;
    std::vector<ArrayNum_t> all_ANs;

    while (getline(inputfile, thisline)) {
        // trim newline char
        if (thisline[thisline.length()-1] == '\n' || thisline[thisline.length()-1] == '\r') {
            //std::cout << "found a newline char!" << std::endl;
            thisline = thisline.substr(0, thisline.length() - 1);
        } else {
            //std::cout << "not newline: |" << (int)thisline[thisline.length()-1] << "|" << std::endl;
        }
        //std::cout << "newline detected" << std::endl;
        all_lines_info.emplace_back(thisline, identifyInputLine(thisline));
        //std::cout << "read line [" << thisline << "]" << std::endl;
    }

    for (int row_ct=0; row_ct < all_lines_info.size(); row_ct++) {
        std::vector<ArrayNum_t> thisline_ANs = identifyNumbers(row_ct, all_lines_info);
        // concatenate the just-grabbed ANs onto the accumulator
        all_ANs.insert(std::end(all_ANs), std::begin(thisline_ANs), std::end(thisline_ANs));
    }
    
    // debug
    std::cout << "total number of ANs found: " << all_ANs.size() << std::endl;

    int sum = 0;
    for (const ArrayNum_t an : all_ANs) {
        std::cout << "Number Found - " << std::setw(5) << an.num_str << " -- good = " << (an.is_adj_symbol ? "true" : "false") << "\n";
        if (an.is_adj_symbol)
            sum += std::stoi(an.num_str);
    }

    std::cout << "Sum of all good numbers is: " << sum << std::endl;
    return 0;
}

// self explanatory
bool isDigit(const char c) {
    return (c >= '0' && c <= '9');
}


// given a single line of the input, identifies each character as either DIGIT, PERIOD, or SYMBOL (an enum)
std::vector<Character_t> identifyInputLine(const std::string &inputline) {
    std::vector<Character_t> symbol_ids;

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



std::vector<ArrayNum_t> identifyNumbers(const int this_row_i, const std::vector<LineInfo_t> &all_lines_info) {
    std::vector<ArrayNum_t> found_numbers;
    bool finding_number = false;

    const LineInfo_t this_line = all_lines_info[this_row_i];

    ArrayNum_t new_AN;

    for (int j=0; j < this_line.str.length(); j++) {
        //std::cout << "identifyNumbers working on (r,c)=" << this_row_i << "," << j << std::endl;
        if (!finding_number && this_line.ids[j] == DIGIT) {
            finding_number = true;
            new_AN.row          = this_row_i;
            new_AN.start_col    = j;
            new_AN.end_col      = j;
            new_AN.num_str      = "";
            new_AN.is_adj_symbol= false;
            //std::cout << "found start of number at c=" << j << std::endl;
        }

        if (finding_number && this_line.ids[j] == DIGIT) {
            new_AN.num_str     += this_line.str[j];
            new_AN.end_col      = j;
        }

        if (finding_number && this_line.ids[j] != DIGIT) {
            finding_number = false;
            found_numbers.push_back(new_AN);
            //std::cout << "found end of number at c=" << j-1 << std::endl;
            //std::cout << "the number was " << new_AN.num_str << std::endl;
        }
    }

    // debug
    //std::cout << "made it to point A" << std::endl;
    //std::cout << "number of numbers found was -- " << found_numbers.size() << std::endl;

    for (int k=0; k < found_numbers.size(); k++) {
        const ArrayNum_t an = found_numbers.at(k);
        const int an_l = an.start_col;
        const int an_r = an.end_col;
        
        //std::cout << "made it to point B for" << an.num_str << std::endl;

        // check if preceding character on same line is a SYMBOL
        if (an_l > 0 && all_lines_info[this_row_i].ids[an_l-1] == SYMBOL) {
            std::cout << "the number " << an.num_str << " at indices (" << an_l << "," << an_r << ") in line" << an.row << " [\n" << all_lines_info[this_row_i].str << "] has a preceding SYMBOL" << std::endl; 
            std::cout << printRowOfIDs(all_lines_info[this_row_i].ids);
            std::cout << std::endl;
            found_numbers[k].is_adj_symbol = true;
        // check if subsequent character on same line is SYMBOL
        } else if (an_r + 1 < all_lines_info[this_row_i].ids.size() && all_lines_info[this_row_i].ids[an_r+1] == SYMBOL) {
            std::cout << "the number " << an.num_str << " at indices (" << an_l << "," << an_r << ") in line" << an.row << " [\n" << all_lines_info[this_row_i].str << "] has a following SYMBOL" << std::endl; 
            std::cout << printRowOfIDs(all_lines_info[this_row_i].ids);
            std::cout << std::endl;
            found_numbers[k].is_adj_symbol = true;
        // check if any character directly/diagonally above is SYMBOL
        } else if (this_row_i > 0 && vecHas(vecSlice(all_lines_info[this_row_i-1].ids, an_l-1, an_r+2), SYMBOL) ) {
            std::cout << "the number " << an.num_str << " at indices (" << an_l << "," << an_r << ") in line" << an.row << " [\n" << all_lines_info[this_row_i].str << "] has a SYMBOL on the line above" << std::endl; 
            std::cout << printRowOfIDs(all_lines_info[this_row_i-1].ids);
            std::cout << std::endl;
            found_numbers[k].is_adj_symbol = true;
        // check if any character directly/diagonally below is SYMBOL
        } else if (this_row_i + 1 < all_lines_info.size() && vecHas(vecSlice(all_lines_info[this_row_i+1].ids, an_l-1, an_r+2), SYMBOL)) {
            std::cout << "the number " << an.num_str << " at indices (" << an_l << "," << an_r << ") in line" << an.row << " [\n" << all_lines_info[this_row_i].str << " has a SYMBOL on the line below" << std::endl; 
            std::cout << printRowOfIDs(all_lines_info[this_row_i+1].ids);
            std::cout << std::endl;
            found_numbers[k].is_adj_symbol = true;
        } else {
            std::cout << "the number " << an.num_str << " at indices (" << an_l << "," << an_r << ") in line" << an.row << " [\n" << all_lines_info[this_row_i].str << " has NO ADJACENT SYMBOL" << std::endl; 
            if (this_row_i > 0)
                std::cout << printRowOfIDs(all_lines_info[this_row_i-1].ids);
            if (true)
                std::cout << all_lines_info[this_row_i].str << std::endl;
            if (this_row_i + 1 < all_lines_info.size())
                std::cout << printRowOfIDs(all_lines_info[this_row_i+1].ids);
            std::cout << std::endl;
            found_numbers[k].is_adj_symbol = false;
        }
    }
    
    //std::cout << "made it to point A" << std::endl;

    return found_numbers;
}

std::vector<Character_t> vecSlice(const std::vector<Character_t> &vec, const int from, const int to) {
    std::vector<Character_t> sliced_vec;
    const int fixed_from = std::max(from, 0);
    const int fixed_to   = std::min(to, static_cast<int>(vec.size()));

    //std::cout << "slicing vector from " << from << " to " << to << std::endl;
    //std::cout << "fixed to be slicing [" << fixed_from << ":" << fixed_to << "]" << std::endl;

    for (int i=fixed_from; i < fixed_to; i++) {
        sliced_vec.push_back(vec.at(i));
    }

    return sliced_vec;
}

bool vecHas(const std::vector<Character_t> &vec, const Character_t &val) {
    for (const Character_t elem : vec) {
        if (elem == val)
            return true;
    }
    return false;
}

std::string printRowOfIDs(const std::vector<Character_t> &linfo) {
    std::string printer;
    for (const Character_t x : linfo) {
        if (x == DIGIT)  printer += '#';
        if (x == SYMBOL) printer += '*';
        if (x == PERIOD) printer += '.';
    }

    printer += '\n';
    return printer;
}