from sys import argv
from itertools import combinations as iter_combos
from math import comb, log10, ceil

MULTIPLICAND = 5
CUTOFF = 1_000_000
#CUTOFF = 20_000

def findall(string : str, substring : str) -> list[int]:
    foundList  = []
    currOffset = 0

    newIndex = string.find(substring, currOffset)
    while (newIndex != -1):
        foundList.append(newIndex)
        currOffset = newIndex+1
        newIndex = string.find(substring, currOffset)
    
    return foundList

def replaceQmarks(inputstring : str, locations : tuple) -> str:
    newstring = "" + inputstring
    for i in locations:
        newstring = newstring[:i] + '#' + newstring[i+1:]
    newstring = newstring.replace('?', '.')
    return newstring

def countContiguousHashes(inputstring : str) -> list[int]:
    newstring = "" + inputstring
    counts    = []
    thisCount = 0

    while ('#' in newstring):
        if newstring.startswith('#'):
            thisCount += 1
        elif thisCount > 0:
            counts.append(thisCount)
            thisCount = 0
        newstring = newstring[1:]

    if thisCount > 0:
        counts.append(thisCount)

    return tuple(counts)

class ConditionRecord:
    def __init__(self, inString : str):
        [self.springs, contigpart] = inString.strip().split()
        self.hashGroups = tuple(int(n) for n in contigpart.split(','))
        return

    def countArrangements(self, please_print:bool=False, from_mult=False) -> int:
        n = self.springs.count('?')
        r = sum(self.hashGroups) - self.springs.count('#')
        nCr = comb(n,r)

        if from_mult:
            if please_print:
                print(f"SING: Calculating for [{self.springs}], pattern {self.hashGroups}")
                print(f"You are about to calculate {nCr=:,} combinations ({n=}, {r=}).")        
            if nCr > CUTOFF:
                if please_print:
                    print(f"Returning 0, due to {nCr:,} being greater than {CUTOFF} by a factor of 10^{ceil(log10(nCr/CUTOFF))}.")
                return 0
            if please_print:
                print("But since that's not too big, let's just calculate it manually.")
        
        # create an iterable of tuples, where each entry in the tuple is an index of a '?' in self.springs that could be replaced with a '#'
        combinations = iter_combos(
            findall(self.springs, '?'),     # the indices of all '?' characters
            r)                              # the number of '?'s that need replacing with '#'s, given our known counts
        
        #if please_print:
        #    print(f"Found {len(list(combinations))} combinations!")
        
        # make them into strings to test
        #[print(combo) for combo in combinations]
        possibleStrings  = [replaceQmarks(self.springs, combo) for combo in combinations]
        contiguityCounts = [countContiguousHashes(string) for string in possibleStrings]
        correctCounts = [x for x in contiguityCounts if x == self.hashGroups]
        if please_print:
            print(f"Hash Grouping should be {self.hashGroups}")
            [print("", self.springs, '\n', x, y, "----", self.hashGroups) for x,y in zip(possibleStrings, contiguityCounts)]
            print(f"possible arrangements = {len(correctCounts)}")
            print(f"-------------------------------------")
        return len(correctCounts)


    def countArrangementsMult(self, please_print:bool=False) -> int:
        return_value = 0

        if please_print:
            print(f"MULT: Calculating for [{self.springs}]x{MULTIPLICAND}, pattern {self.hashGroups}x{MULTIPLICAND}")
    
        # if we end or begin with the proper grouping, then we're good to just multiply
        if self.springs.endswith('#' * self.hashGroups[0]) or self.springs.startswith('#' * self.hashGroups[0]):
            if please_print:
                print("DEBUG: countArrangementMult -- branch A")
            return_value = self.countArrangements() ** MULTIPLICAND
        # if we end or begin with '.', then calculate for "?pattern" or "pattern?", use it 4 times, then multiply by usual
        elif self.springs[0] == '.' or self.springs[-1] == '.':
            if please_print:
                print("DEBUG: countArrangementMult -- branch B")
            self_countArrangements = self.countArrangements()
            # [.???]?[.???]?[.???]?[.???]?[.???] = [.????][.????][.????][.????]+[.???]
            padded_final = ConditionRecord("" + self.springs + "? " + ",".join([str(x) for x in self.hashGroups]))
            total_if_final = (padded_final.countArrangements() ** (MULTIPLICAND-1)) * self_countArrangements
            padded_start = ConditionRecord("?" + self.springs + " " + ",".join([str(x) for x in self.hashGroups]))
            total_if_start = self_countArrangements * (padded_start.countArrangements() ** (MULTIPLICAND-1))
            return_value = max(total_if_final, total_if_start)
        elif False:
            #TODO: Add more cases! Ones to consider:
                #If you have e.g. ?###???????? and (3,_,_) then the first ? should always be a . -- trim the first 4 chars and multiply by rest
                #If you have e.g. ?###.??????? and (4,_,_) then the first ? should always be a # -- trim the first n chars and multiply by rest
                #If you have e.g. ...?###?.... and (_,3,_) then both adjacent ? should be .      -- set those ? to . and re-run self
            pass
        else:
            if please_print:
                print(" **** PANIC! UNCAUGHT CASE! ****")
                print("Doing the long calculation...")
            multiplied = self.multiply(MULTIPLICAND)
            return_value = multiplied.countArrangements(please_print=False, from_mult=True)

        if please_print:
            if return_value != 0:
                print(f"possible arrangements = {return_value}")
            print(f"--------------------------------------")
        return return_value
    
    def multiply(self, factor : int):
        return ConditionRecord("?".join([self.springs] * factor) + " " + ",".join([str(x) for x in self.hashGroups] * factor))

# end class ConditionRecord

def main():
    with open(argv[1], 'r') as inputfile:
        lines = inputfile.readlines()
    records = tuple(ConditionRecord(line) for line in lines)

    print(f"--------------------------------------")
    records1 = [cr.countArrangements() for cr in records]
    #records2 = [cr.countArrangementsMult(please_print=False) for cr in records]
    print("================================================")
    print(f"Part 1: The answer is {sum(records1)}")
    #print(f"Part 2: The answer is {sum(records2)}")
    return

if __name__ == "__main__":
    main()