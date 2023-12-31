from enum import Enum
from sys import argv

CYCLECOUNT = 1_000
CYCLEPRINT = 1_00

class TileEnum(Enum):
    CUBIC   = 0
    ROUND   = 1
    EMPTY   = 2
    INVALID = 3

class DirectionEnum(Enum):
    NORTH   = 10
    SOUTH   = 11
    WEST    = 12
    EAST    = 13

def tile2char(tile : TileEnum) -> str:
    match tile:
        case TileEnum.CUBIC:
            return '#'
        case TileEnum.ROUND:
            return 'O'
        case TileEnum.EMPTY:
            return '.'
        case _:
            return '?'

class Platform:
    def __init__(self, allLines : list[str]) -> None:
        self.array = []
        self.rowCt = 0
        self.colCt = 0
        for line in allLines:
            self.addRow(line)
        return
    
    def addRow(self, line : str) -> None:
        newline = []
        for ch in line:
            match ch:
                case 'O':
                    newline.append(TileEnum.ROUND)
                case '.':
                    newline.append(TileEnum.EMPTY)
                case '#':
                    newline.append(TileEnum.CUBIC)
                case '\n' | '\r':
                    pass
                case _:
                    newline.append(TileEnum.INVALID)
        self.array.append(newline)
        self.recountRows()
        self.recountCols()
        return
    
    def __str__(self) -> str:
        outString = ""
        for row in self.array:
            for tile in row:
                outString += tile2char(tile)
            outString += '\n'
        return outString
    
    def __repr__(self) -> str:
        return "Platform(" + self.__str__().replace('\n', r'\n') + ")"

    def isEmpty(self) -> bool:
        return len(self.array) == 0
    
    def recountRows(self) -> int:
        self.rowCt = len(self.array)
        return
    
    def recountCols(self) -> int:
        if self.rowCt > 0:
            self.colCt = len(self.array[0])
        else:
            self.colCt = 0
        return
        
    def isValidCoord(self, i : int, j : int) -> bool:
        outval = not ((i < 0) or (i >= self.rowCt) or (j < 0) or (j >= self.rowCt))
        #print("is", i, j, "a valid coord? ==> ", outval)
        return outval

    def hasTileType(self, t : TileEnum, i : int, j : int) -> bool:
        return self.isValidCoord(i,j) and self.array[i][j] == t

    def isntBlocked(self, i : int, j : int) -> bool:
        outval = self.hasTileType(TileEnum.EMPTY, i, j)
        #print("  checking coordinates", i, j, ":", f"is {outval}ly open")
        return outval

    def slideOnce(self, dir : DirectionEnum) -> bool:
        rangeRows = list(range(self.rowCt))
        rangeCols = list(range(self.rowCt))

        if dir == DirectionEnum.SOUTH:
            rangeRows.reverse()
        elif dir == DirectionEnum.EAST:
            rangeCols.reverse()

        anyChange = False
        for i in rangeRows:
            for j in rangeCols:
                ni, nj = self.nextSlidingCoord(i, j, dir)
                #print(f"Can {i},{j} slide to {ni},{nj}?")
                if self.isntBlocked(ni, nj) and self.hasTileType(TileEnum.ROUND, i, j):
                    #print("Sliding!")
                    self.array[ni][nj] = TileEnum.ROUND
                    self.array[i][j]   = TileEnum.EMPTY
                    anyChange = True
        return anyChange

    def nextSlidingCoord(self, i: int, j: int, dir: DirectionEnum) -> (int, int):
        match dir:
            case DirectionEnum.NORTH:
                return (i-1, j)
            case DirectionEnum.SOUTH:
                return (i+1, j)
            case DirectionEnum.EAST:
                return (i, j+1)
            case DirectionEnum.WEST:
                return (i, j-1)
            case _:
                raise ValueError("bad value for DirectionEnum")
        

    def slide(self, direction : DirectionEnum) -> None:
        change = True
        while (change):
            change = self.slideOnce(direction)
        return
    
    def cycle(self, numCycles : int) -> None:
        for cyc in range(numCycles):
            if cyc % CYCLEPRINT == 0:
                print("Calculating cycle", cyc)
            self.slide(DirectionEnum.NORTH)
            self.slide(DirectionEnum.WEST)
            self.slide(DirectionEnum.SOUTH)
            self.slide(DirectionEnum.EAST)
        return

    def calculateLoad(self) -> int:
        load = 0

        for i in range(self.rowCt):
            for j in range(self.colCt):
                if self.hasTileType(TileEnum.ROUND, i, j):
                    load += (self.rowCt - i)
        
        return load
    
def main() -> None:
    with open(argv[1], 'r') as infile:
        lines = infile.readlines()

    plat1 = Platform(lines)
    plat1.slide(DirectionEnum.EAST)
    print(f"Part 1: The total load is {plat1.calculateLoad()}.")

    plat2 = Platform(lines)
    plat2.cycle(CYCLECOUNT)
    print(f"Part 2: The load after {CYCLECOUNT} cycles is {plat2.calculateLoad()}.")

    return
    
if __name__ == "__main__":
    main()