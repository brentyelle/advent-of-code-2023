# advent-of-code-2023
The contents of this repository are my attempts at the [Advent of Code 2023](https://adventofcode.com/2023) challenge. I'm going to try to spice things up by varying my choice of language regularly—though it's going to be mostly C, C++, Python, and Haskell. Online discussions on the [Advent of Code Subreddit](https://www.reddit.com/r/adventofcode/) were only examined after solving or abandoning the day in question.

## Languages used for each day, number of stars earned, date completed, and personal comments
1. **C** (⭐⭐) — Dec 9, 2023
    * Other participants noted difficulty with overlapping words such as `sevenine`, but thanks to writing in C and brute-force searching for every numeric word individually, I actually never ran into this problem.
2. **C++** (⭐⭐) — Dec 11, 2023
    * Quite straightfoward, little to no issue.
3. **C++** (⭐⭐) — Dec 14, 2023
    * First attempted with Haskell, but abandoned due to the headache of not having loop indices or mutable data for this particular problem. Thankfully, C++ was much better suited to the task at hand.
    * My biggest issue in the C++ version was with mistakenly *not* grabbing numbers if they occurred at the end of a line.
4. **Python** (⭐⭐) — Dec 12, 2023
    * Another straightforward puzzle. Rather than do what some other participants did, replicating the cards in memory, I instead had each `ScratchCard` object keep track of how many copies of itself were owned.
5. **Python** (⭐⭐) — _(Part I)_ Dec 13, 2023; _(Part II)_ Dec 14, 2023
    * My first major disappointment in my solution, which took roughly 20~30 minutes to run on my laptop. I had started the implementation of a proper solution—mapping ranges to ranges instead of numbers to numbers—but I was getting fed up with the problem and decided to simply let it run in the background brute-forcing the solution.
6. **Haskell** (⭐⭐) — Dec 14, 2023
    * By far the easiest problem so far, and the first that could feasibly be solved even with pencil and paper.
    * Due to its simplicity, I later re-implemented my solution in **NASM x86-64** (on Dec 20, 2023) for extra practice.
7. **Python** (⭐) — _(Part I)_ Dec 14, 2023
    * The first puzzle where I found Part II too annoying to implement.
8. **C++** (⭐⭐) — Dec 18, 2023
    * Originally wrote a brute-force solution for Part II, but after it ran for about 30 seconds without halting, I gave it some thought and realized I could use the least common multiple to solve it way faster.
    * Other participants noted that, had the data been different (with the cycles not returning to their starting point), this question could have been far harder than simply using the LCM. Thankfully, the puzzle-maker was merciful.
9. **C** (⭐⭐) — Dec 21, 2023
    * Although my solution works quickly and correctly, I find myself unhappy with how inflexible it is; in particular, my code does not rely on counting the number of integers in a given line of the file, but rather relies on the global definition `#define INTS_PER_LINE`, which must be changed depending on the input file. To make it more flexible, I should have written a subroutine to either "slice" or "tokenize" the input line based on spaces.
10. **C++** (⭐) — _(Part I)_ Dec 22, 2023
    * This puzzle marks the first day where, going into the puzzle, I am not yet sure *how* to go about solving Part II...
    * ...But one possible idea is to (1) mark every non-pipe as "contained", then (2) re-mark every non-pipe that isn't adjacent to a path-pipe as "not contained", then (3) recursively re-re-mark every "not contained" non-pipe adjacent to a "contained" non-pipe as "contained" again.
11. _(not yet attempted)_
12. _(not yet attempted)_
13. _(not yet attempted)_
14. _(not yet attempted)_
15. **C** (⭐) — _(Part I)_ Dec 24, 2023
16. _(not yet attempted)_
17. _(not yet attempted)_
18. _(not yet attempted)_
19. _(not yet attempted)_
20. _(not yet attempted)_
21. **C++** (⭐) — _(Part I)_ Dec 26, 2023
    * Development of this day's solution went through several phases:
    * In the first phase, I attempted to use an [adjacency matrix](https://en.wikipedia.org/wiki/Adjacency_matrix) to solve the pathing problem "cleverly," but it turns out that doing so is actually extremely slow, due to needing to multiply 121<sup>2</sup>-by-121<sup>2</sup> boolean matrices against each other, even with using caching to my advantage. However, since the matrices were very sparse, I had an idea of what to do next...
    * In the second phase, I scrapped the idea of an adjacency matrix and instead used a `std::vector` of all of the coordinates that could be reached by taking *n* steps. This allowed the time needed for my program to solve Part I from an estimated half-year down to approximately 0.19 seconds—probably the most drastic performance improvement I've ever seen in one of my own programs.
    * In the third and current phase, I expanded the program's functionality enough to solve Part 2, but since the `std::vector` of coordinates grows as *Θ(n<sup>2</sup>)* where *n* is the number of steps, the lookup time increased considerably for each iteration. Therefore, I replaced the Part 2 `std::vector` with a `std::unordered_map`, hoping to take advantage of the *Θ(1)* lookup time. Unfortunately, I have yet to see much improvement.
