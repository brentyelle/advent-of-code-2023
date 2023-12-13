from math import floor

FILENAME = "./day4_input.txt"

class ScratchCard:
    """Implements a class that will hold our data about one scratchcard:
        \n.id         -- card ID number (unused)
        \n.winners    -- list of winning numbers
        \n.numbers    -- list of scratch-off numbers
        \n.copies     -- how many copies of this scratchcard we currently own
    """
    def __init__(self, inputline : str):
        """Inputline should be of the form "Card #: # # # # # | # # # # # # # #",
        consisting of "Card", the game ID (integer), a colon ':', the list of winning numbers (any nonzero amount), a pipe '|', and the list of scratch-off numbers (any nonzero amount)."""
        [i_string, w_string, n_string] = ScratchCard._splitColonPipe(inputline)
        self.id = self._extractCardID(i_string)
        self.winners = self._extractNumbers(w_string)
        self.numbers = self._extractNumbers(n_string)
        self.copies = 1
        return
    
    # takes 
    def _extractCardID(self, string : str) -> int:
        """Given a string of the form "Card 17", for example, extracts the number 17."""
        return int(string.strip().split()[1])

    def _extractNumbers(self, string : str) -> [int]:
        """Given a string of ASCII numbers separated by spaces, splits them into a list of integers.
        \nThe int() function takes care of any whitespace not taken care of by the .strip() method."""
        return [int(n) for n in string.strip().split()]
    
    def _splitColonPipe(string : str) -> [str]:
        """Given a string of the form "textA: textB | textC", splits them into a list ["textA", "textB", "textC"].
        \nDepending on input string, this can leave some trailing or leading whitespace in the extracted strings."""
        return string.strip().replace(':', '|').split('|')
    
    def points(self) -> int:
        """If you have 0 winning numbers, you get 0 points.
        \nOtherwise, if you have n winning numbers, you get 2^(n-1) points."""
        n = self.howManyWinners()
        return floor(2**(n-1)) # floor is required for the n=0 case
    
    def howManyWinners(self) -> int:
        """Calculates how many of the scratch-off numbers on the tickets are winning numbers."""
        return len([x for x in self.numbers if x in self.winners])
#class end

def replicateScratchCards(scs : [ScratchCard]):
    """Having `n` number of winning numbers on card `i` will give you one extra copy of the next `n` cards after card `i`.
    However, this compounds on itself. For example, if I have 6 copies of card i, then each of the 6 copies gives an extra copy of the next `n` cards."""
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
    with open(FILENAME, "r") as ifile:
        scratchcards = [ScratchCard(line) for line in ifile.readlines()]
    # do replication for part 2 (does not affect part 1 calculation)
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
