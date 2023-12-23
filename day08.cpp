#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <numeric>

// class to be held as the `value`s in the unordered_map of `Network`; the "self" value is to be held as the `key` in the unordered_map.
class Node {
private:
    std::string left;
    std::string right;

public:
    Node(std::string l, std::string r) : left(l), right(r) {};
    friend class Network;
};

// class to hold the network of `Node`s as an unordered_map
class Network {
private:
    std::unordered_map<std::string, Node> map;
    std::string pattern;
    std::vector<std::string> startlabels;

public:
    // Given an input `oneline1` string such as "VRN = (CSM, GPD)", splits it into `ownlabel`, `leftlabel`, and `rightlabel` strings, which are updated by reference. For this example,
    //  `self` should get set to "VRN"
    //  `left` should get set to "CSM"
    //  `right` should get set to "GPD"
    // Then, this combination is added to the `map` properly.
    void addNode(const std::string &oneline) {
        // slice the string into its proper pieces
        std::string ownlabel, leftlabel, rightlabel;
        ownlabel   = oneline.substr(0,                              3);
        leftlabel  = oneline.substr(oneline.find_first_of('(') + 1, 3);
        rightlabel = oneline.substr(oneline.find_first_of(',') + 2, 3);

        // all labels ending in 'A' are considered "start labels" for the purpose of Part 2
        if (ownlabel.back() == 'A')
            startlabels.push_back(ownlabel);
        // builds the unordered_map piece-by-piece
        map.insert({ownlabel, Node(leftlabel, rightlabel)});
        return;
    }

    // Solve Part 1: Find the number of iterations needed to go from node "AAA" to a node ending in 'Z'.
    // Repurposed for Part 2, where a non-default argument gives a different starting node.
    long long findZZZ_Part1(const std::string &startstring="AAA") const {
        long long count  = 0;
        std::string curr = startstring;

        // until we reach a label ending in Z, keep iterating through the nodes
        while (curr.back() != 'Z') {
            // necessary due to const-ness of function
            const Node &currNode = map.at(curr);
            // follow the correct branch, taking into account that the pattern repeats infinitely
            curr = (pattern.at(count % pattern.length()) == 'L' ? currNode.left : currNode.right);
            count++;
        }

        /*
        // IF THE PROBLEM INVOLVED NON-FULLY-LOOPING NODES, WE'D HAVE TO DO STUFF WITH THIS ARITHMETIC HERE
        
        long long count2 = count;
        
        do {
            // necessary due to const-ness of function
            const Node &currNode = map.at(curr);
            // follow the correct branch, taking into account that the pattern repeats infinitely
            curr = (pattern.at(count2 % pattern.length()) == 'L' ? currNode.left : currNode.right);
            count2++;
        } while (curr.back() != 'Z');

        std::cout << "Node " << startstring << " first reaches Z-node at " << count << " steps, then again after another " << count2 - count << " steps\n";
        */
        
        std::cout << "Start node " << startstring << " reaches a Z-final node after " << count << " steps\n";
        return count;
    }

    // Solve Part 2: Given all starting nodes, find the number of iterations needed to get all of them to simultaneously land on a node ending in 'Z'.
    long long findZZZ_Part2() const {
        std::vector<long long> zzzcounts;
        long long countLCM = 1;

        // get loop count for each individual start label, LCM-ing it with the current LCM
        for (const auto startlabel : startlabels)
            countLCM = std::lcm(countLCM, findZZZ_Part1(startlabel));

        return countLCM;
    }

    // setter for `pattern`
    void setPattern(const std::string &instring) {
        pattern = instring;
    }
};

int main(int argc, char* argv[]) {
    // check arguments
    if (argc != 2) {
        std::cout << "Usage: ./a.out <filename_to_process>" << std::endl;
        exit(1);
    }

    // load file
    std::ifstream inputfile(argv[1]);
    if (!inputfile) {
        std::cout << "Error opening file " << argv[1] << "." << std::endl;
        exit(1);   
    }

    // declarations
    std::string thisline, self, left, right;    // temporary holders
    Network network;                            // associative map w/ some bells & whistles

    // grab pattern string
    getline(inputfile, thisline);
    network.setPattern(thisline.substr(0, thisline.find_first_not_of("LR")));

    // skip empty line
    getline(inputfile, thisline);

    // read all nodes
    while (getline(inputfile, thisline))
        network.addNode(thisline);

    // solve the problems
    std::cout << "-----------------------------------------------------\n";
    auto answer1 = network.findZZZ_Part1();
    std::cout << " * Part 1: Number of steps is " << answer1 << std::endl;
    
    std::cout << "-----------------------------------------------------\n";
    auto answer2 = network.findZZZ_Part2();
    std::cout << " * Part 2: Number of steps is " << answer2 << ", the LCM of the above." << std::endl;

    std::cout << "-----------------------------------------------------\n";
    return 0;
}
