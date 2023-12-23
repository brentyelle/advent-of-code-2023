from itertools import chain

FILENAME = "day5_input.txt"

class SmarterMap:
    def __init__(self, rangelist=[]):
        self.ranges = []
        for r in rangelist:
            self.ranges.append(stringToTriple(r))
        return
    
    def update(self, rng : (int,int,int)):
        self.ranges.append(rng)
        return
    
    def lookup(self, key : int) -> int:
        for (dstart, sstart, rlen) in self.ranges:
            if key >= sstart and key < sstart+rlen:
                offset = key - sstart
                return dstart + offset
        # if not found, return the key
        return key

    def reverseLookup(self, val : int) -> int:
        for (dstart, sstart, rlen) in self.ranges:
            if val >= dstart and val < dstart+rlen:
                offset = val - dstart
                return sstart + offset
        return val

    def has(self, key : int) -> bool:
        for (dstart, sstart, rlen) in self.ranges:
            if key >= sstart and key < sstart+rlen:
                return True
        # if not found, return False
        return False

    def hasValue(self, val : int) -> bool:
        for (dstart, sstart, rlen) in self.ranges:
            if val >= dstart and val < dstart+rlen:
                return True
        return False

    def findOverlap(self, keyR : (int,int)) -> (int,int,int):
        keystart = keyR[0]
        keylen   = keyR[1]

        for (dstart, sstart, rlen) in self.ranges:
            if keystart >= sstart and keystart+keylen <= sstart+rlen:
                return (dstart, sstart, rlen)
        # if not found, return False
        return (-1,-1,-1)

    # def narrowSeedRange(self, seed_pair : (int, int)) -> (int, int):
    #     """
    #     if seed is  (70, 7)    : [70,71,72,73,74,75,76]
    #     if range is (_, 72, 8) :       [72,73,74,75,76,77,78,79] -> [_, _+1]
    #     then seed narrows to   :       [72,73,74,75,76]
    #       which is (72,5)

    #     beginning gets changed to max(70,72)=72
    #     len gets changed to oldseedlen  - (newbeginning - beginseed)  = 7-(72-70) = 7-2 = 5
    #     UNUSED --           oldrangelen - (newbeginning - beginrange) = 8-(72-72) = 7-0 = 8
        
    #     if seed is (31, 5)     :          [31,32,33,34,35]
    #     if range is (_, 28, 5) : [28,29,30,31,32]
    #     then seed narrows to   :          [31,32]
    #       which is (31,2)

    #     beginning gets changed to max(31,28)=31
    #     len gets changed to oldrangelen - (newbeginning - beginrange) = 5-(31-28) = 5-3 = 2
    #     UNUSED --           oldseedlen  - (newbeginning - beginseed)  = 5-(31-31) = 5-0 = 5

    #     then seed should be narrowed to (72, 3) """

    #     validKeyRange = self.findOverlap(seed_pair)
    #     if validKeyRange != (-1,-1,-1):
    #         # get old range
    #         old_seed_start  = seed_pair[0]
    #         old_seed_len    = seed_pair[1]
    #         range_start     = validKeyRange[1]
    #         range_len       = validKeyRange[2]
    #         # calculate narrowed range
    #         new_seed_start  = max(old_seed_start, range_start)
    #         new_seed_len    = min(old_seed_len-(new_seed_start-old_seed_start), range_len-(new_seed_start-range_start))
    #         return (new_seed_start, new_seed_len)
    #     else:
    #         return (0,0)

    # def lookupRange(self, keyR : (int,int)) -> int:

    #     narrowkey = self.narrowSeedRange(keyR)
    #     keystart = narrowkey[0]
    #     keylen   = narrowkey[1]

    #     validKeyRanges = self.findOverlap(keyR)

    #     if validKeyRanges != (-1,-1,-1):
    #         for (dstart, sstart, rlen) in self.ranges:
    #             if keystart >= sstart or keystart+keylen <= sstart+rlen:
    #                 offset = keystart - sstart
    #                 return dstart + offset
    #     else:

    #     return key
#end class


def getSeedsPart1(line : str) -> [int]:
    return [int(n) for n in line[6:].split()]

def getSeedsPart2_ranges(line : str) -> [int]:
    numbers = [int(n) for n in line[6:].split()]
    ranges      = range(0)
    for (start,length) in zip(numbers[::2], numbers[1::2]):
        print(f"adding range({start},{start+length})")
        ranges = chain(ranges, range(start,start+length))
    return ranges

def getSeedsPart2_pairs(line : str) -> [(int,int,int)]:
    numbers = [int(n) for n in line[6:].split()]
    return list(zip(numbers[::2], numbers[1::2]))

def stringToTriple(string : str) -> (int,int,int):
    [dest_range_start, src_range_start, range_len] = [int(n) for n in string.split()]
    return (dest_range_start, src_range_start, range_len)

def getDictsFromFile(seed2soil : SmarterMap, soil2fert : SmarterMap, fert2water : SmarterMap, water2light : SmarterMap, light2temp : SmarterMap, temp2humid : SmarterMap, humid2loc : SmarterMap, isPart1 : bool) -> list:
    with open(FILENAME, "r") as infile:
        if isPart1:
            seedlist = getSeedsPart1(infile.readline())
        else:
            seedlist = getSeedsPart2_pairs(infile.readline())
        while (True):
            line = infile.readline()
            line = lookForHeader(seed2soil, infile, "seed-to-soil map:", line)
            line = lookForHeader(soil2fert, infile, "soil-to-fertilizer map:", line)
            line = lookForHeader(fert2water, infile, "fertilizer-to-water map:", line)
            line = lookForHeader(water2light, infile, "water-to-light map:", line)
            line = lookForHeader(light2temp, infile, "light-to-temperature map:", line)
            line = lookForHeader(temp2humid, infile, "temperature-to-humidity map:", line)
            line = lookForHeader(humid2loc, infile, "humidity-to-location map:", line)
            if line == "":
                break
    return seedlist

def lookForHeader(dictionary, infile, triggerString, line):
    if line.strip() == triggerString:
        line = infile.readline()
        while (line.strip() != ""):
            dictionary.update(stringToTriple(line))
            line = infile.readline()
    return line

def hasInRange(seed, ranges_of_seeds):
    for (seed_start, seed_len) in ranges_of_seeds:
        if seed >= seed_start and seed < seed_start+seed_len:
            return True
    return False

def main():
    seed2soil   = SmarterMap()
    soil2fert   = SmarterMap()
    fert2water  = SmarterMap()
    water2light = SmarterMap()
    light2temp  = SmarterMap()
    temp2humid  = SmarterMap()
    humid2loc   = SmarterMap()

    seedlist1   = getDictsFromFile(seed2soil, soil2fert, fert2water, water2light, light2temp, temp2humid, humid2loc, isPart1=True)
    #print(f"Part 1: number of seeds to locate is {len(list(seedlist1))}")
    loclist1    = []

    #part 1
    for seed in seedlist1:
        soil    = seed2soil.lookup(seed)
        fert    = soil2fert.lookup(soil)
        water   = fert2water.lookup(fert)
        light   = water2light.lookup(water)
        temp    = light2temp.lookup(light)
        humid   = temp2humid.lookup(temp)
        loc     = humid2loc.lookup(humid)
        loclist1.append(loc)
    print(f"Part 1: The lowest location number corresponding to the starting seeds is {min(loclist1)}")

    #part 2
    seedlist2   = getDictsFromFile(seed2soil, soil2fert, fert2water, water2light, light2temp, temp2humid, humid2loc, isPart1=False)
    print(f"Part 2: number of seeds to locate is {len(list(seedlist2))}")

    print(f"{seedlist2=}")

    loc2 = 0
    #lowestLoc2 = 99999_9999_99999_99999
    
    while (loc2 < 2*max(seedlist1)):
        if loc2 % 10000 == 0:
            print(f"testing {loc2=}")
        humid   = humid2loc.reverseLookup(loc2)
        temp    = temp2humid.reverseLookup(humid)
        light   = light2temp.reverseLookup(temp)
        water   = water2light.reverseLookup(light)
        fert    = fert2water.reverseLookup(water)
        soil    = soil2fert.reverseLookup(fert)
        seed    = seed2soil.reverseLookup(soil)
        if hasInRange(seed, seedlist2):
            print(f"Found a match! {seed=}, {loc2=}")
            break
        loc2 += 1
    
    print(f"Part 2: The lowest location number corresponding to the starting seeds is {loc2}")
    # ANSWER WAS: seed=3205462429, loc2=77435348
    return

if __name__ == "__main__":
    main()
