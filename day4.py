from math import floor

FILENAME = "./day4_input.txt"

class ScratchCard:
    """Implements a class that will hold one scratchcard's data."""
    def __init__(self, inputline : str):
        [i_string, w_string, n_string] = ScratchCard._splitColonPipe(inputline)
        #print(i_string, w_string, n_string, "<-- extracted data")
        self.id = self._extractCardID(i_string)
        self.winners = self._extractNumbers(w_string)
        self.numbers = self._extractNumbers(n_string)
        self.copies = 1
        return
    
    def _extractCardID(self, string : str) -> int:
        return int(string.strip().split()[1])

    def _extractNumbers(self, string : str) -> [int]:
        return [int(s) for s in string.strip().split()]
    
    def _splitColonPipe( s : str) -> [str]:
        return s.strip().replace(':', '|').split('|')
    
    def points(self) -> int:
        s = self.howManyWinners()
        return floor(2**(s-1))
    
    def howManyWinners(self) -> int:
        return len([x for x in self.numbers if x in self.winners])


# Used for Part 2's solution:
#   Having `n` number of winning numbers on card `i` will give you one extra copy of the next `n` cards after card `i`.
#   However, this compounds on itself. For example, if I have 6 copies of card i, then each of the 6 copies gives an extra copy of the next `n` cards.
def replicateScratchCards(scs : [ScratchCard]):
    totalCardCount = len(scs)
    # for all cards
    for i in range(totalCardCount):
        # for the (number-of-wins-on-this-card) subsequent cards
        winners_on_i = scs[i].howManyWinners()
        for j in range(1, winners_on_i+1):
            # give us one additional copy of that subsequent card
            if (i+j) < totalCardCount:
                # we use scs[i].copies because we might have multiple copies of this card
                scs[i+j].copies += scs[i].copies
    return scs

def main():
    # convert file into a list of ScratchCard objects
    scratchcards = None
    with open(FILENAME, "r") as ifile:
        scratchcards = [ScratchCard(line) for line in ifile.readlines()]
    scratchcards = replicateScratchCards(scratchcards)

    # part 1
    pointsPart1 = sum([card.points() for card in scratchcards])
    print(f"Part 1: Sum of points for all cards is {pointsPart1}")

    # part 2
    cardsPart2 = sum([card.copies for card in scratchcards])
    print(f"Part 2: Total number of cards is {cardsPart2}")

    return


if __name__ == "__main__":
    main()
