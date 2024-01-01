from sys import argv
from enum import Enum
from math import comb as nCr
from itertools import combinations as iter_combos

SMALLER_THAN_ANY = -1
BIGGER_THAN_ANY  = 9999999
PAIR             = 2

class Space(Enum):
    EMPTY   = 0
    GALAXY  = 1
    INVALID = 2
#end clas Space

def str2space(char : str) -> Space:
    match char:
        case '.':
            return Space.EMPTY
        case '#':
            return Space.GALAXY
        case _:
            return Space.INVALID

def space2str(sp : Space) -> str:
    match sp:
        case Space.EMPTY:
            return '.'
        case Space.GALAXY:
            return '#'
        case Space.INVALID:
            return '?'

class Universe:
    def __init__(self, filestring : str) -> None:
        """Should take an entire file's contents, obtained via `fileObject.read()`, as the argument."""
        # interpret the file
        self.spaces     = [list(map(str2space, line.strip())) for line in filestring.splitlines()]
        self.emptyHorz  = []
        self.emptyVert  = []
        self.galaxyList = []
        # build list of empty horizontals
        for i, horz in enumerate(self.spaces):
            if all(space == Space.EMPTY for space in horz):
                self.emptyHorz.append(i)
        # build list of empty verticals
        if self.height() != 0:
            for j in range(len(self.spaces[0])):
                if all(space == Space.EMPTY for space in [row[j] for row in self.spaces]):
                    self.emptyVert.append(j)
        # build list of galaxies
        for i in range(self.height()):
            for j in range(self.width()):
                if self.spaces[i][j] == Space.GALAXY:
                    self.galaxyList.append((i,j))
        return

    def height(self) -> int:
        return len(self.spaces)
    
    def width(self) -> int:
        if len(self.spaces) == 0:
            return 0
        else:
            return len(self.spaces[0])

    def countEmptyHorizontals(self, between : (int,int) = (SMALLER_THAN_ANY, BIGGER_THAN_ANY)) -> int:
        """Given a range `between`, calculates how many of original empty horizontal rows existed in the `i`-range `between` (exclusive at the ends)."""
        count = 0
        for i in range(max(0, between[0]+1), min(len(self.spaces), between[1])):
            if i in self.emptyHorz:
                count += 1
        return count

    def countEmptyVerticals(self, between : (int,int) = (SMALLER_THAN_ANY, BIGGER_THAN_ANY)) -> int:
        """Given a range `between`, calculates how many of original empty vertical columns existed in the `j`-range `between` (exclusive at the ends)."""
        count = 0
        for j in range(max(0, between[0]+1), min(len(self.spaces), between[1])):
            if j in self.emptyVert:
                count += 1
        return count

    def calcGalaxyDistances(self, expansionFactor : int) -> int:
        """For all galaxies in `self.galaxyList`, calculates the sum of all distances between all pairs of galaxies. "Distance" is specifically the Manhattan distance between the coordinates of the two galaxies."""
        # progress markers
        count                   = 0
        totalCount              = nCr(len(self.galaxyList), PAIR)
        # return value
        totalDistances          = 0
        # type check, save value that's re-used
        expansionMultiplier     = int(expansionFactor) - 1

        # for all pairs of galaxies
        for (i1,j1), (i2,j2) in iter_combos(self.galaxyList, PAIR):
            count += 1
            if count % 8192 == 0:
                print(f"Step {count:5} out of {totalCount:5}, {100.0 * count/totalCount:5.2f}% complete")
            # calculate distance after expanding each empty row/column by a factor of `expansionFactor`
            i_dist = abs(i2-i1) + expansionMultiplier * self.countEmptyHorizontals(between=(i1, i2)                 )
            j_dist = abs(j2-j1) + expansionMultiplier * self.countEmptyVerticals  (between=(min(j1,j2), max(j1,j2)) )
            totalDistances += (i_dist + j_dist)
        return totalDistances

    def __str__(self):
        """Used to `print` this object."""
        return "\n".join(["".join(map(space2str, row)) for row in self.spaces]) + "\n"
#end class Universe

def main():
    with open(argv[1]) as infile:
        universe = Universe(infile.read())
    # Checking input worked fine
    print("Universe before expansion:")
    print(universe)

    print("Calculating Part 1...")
    part1_sumDistances = universe.calcGalaxyDistances(expansionFactor=2)
    print(f"Part 1: The sum of all shortest paths between galaxies is {part1_sumDistances}.\n")

    print("Calculating Part 2...")
    part2_sumDistances = universe.calcGalaxyDistances(expansionFactor=1000000)
    print(f"Part 2: The sum of all shortest paths between galaxies is {part2_sumDistances}.\n")
    return

if __name__ == "__main__":
    main()