from sys import argv

# TWO     = 2
# THREE   = 3
# FOUR    = 4
# FIVE    = 5
# SIX     = 6
# SEVEN   = 7
# EIGHT   = 8
# NINE    = 9
# TEN     = 10
# JACK    = 11
# QUEEN   = 12
# KING    = 13
# ACE     = 14
class Card:
    """Defines a `Card` class--basically a glorified `enum`--for the `Hand` class to contain."""
    def __init__(self, r):
        if type(r) == int:
            self.label = r
        elif type(r) == str:
            match r:
                case "T":
                    self.label = 10
                case "J":
                    self.label = 11
                case "Q":
                    self.label = 12
                case "K":
                    self.label = 13
                case "A":
                    self.label = 14
                case _:
                    # raises ValueError if invalid
                    self.label = int(r)
        elif type(r) == Card:
            self.label = r.label
        else:
            raise ValueError
        return
    
    def __eq__(self, rhs) -> bool:
        if rhs == None:
            return False
        if type(rhs) != Card:
            raise ValueError
        return self.label == rhs.label
    
    def __lt__(self, rhs) -> bool:
        if type(rhs) != Card:
            raise ValueError
        return self.label < rhs.label
    
    def __hash__(self) -> int:
        return hash(self.label)
    
    def __str__(self) -> str:
        match self.label:
            case 10:
                return "T"
            case 11:
                return "J"
            case 12:
                return "Q"
            case 13:
                return "K"
            case 14:
                return "A"
            case _:
                return str(self.label)
#end class

class Hand:
    """Defines a `Hand` class for the hands of `Card`s."""
    def __init__(self, cardlist, bidval=None):
        # interpret cardlist
        if type(cardlist) == str:
            (self.cards, self.bid) = self.parseHandString(cardlist)
            return
        elif type(cardlist) == list:
            self.cards = [Card(c) for c in cardlist]
            self.bid   = int(bidval)
            return
        # shortcut in case I want to copy a hand
        elif type(cardlist) == Hand:
            self.cards = [] + cardlist.cards
            self.bid   = bidval
            return
        else:
            raise ValueError
    
    def parseHandString(self, string : str) -> ([Card], Card):
        """Given a line from the input file, parse it for the `__init__` constructor of this class."""
        [cards_string, bid_string] = string.strip().split()
        cards_nums = [Card(i) for i in list(cards_string)]
        return (cards_nums, int(bid_string))

    def cardCounts(self):
        """Return a list of tuples of [1] each card (no duplicates) in the hand and [2] the number of copies of that card that there are."""
        return [(card, self.cards.count(card)) for card in set(self.cards)]

    def has_n_ofakind(self, n : int, exclude=None) -> Card:
        """For calculating 5-, 4-, and 3-of-a-kind, as well as 1-pair and 2-pair scores.
           The `exclude` kwarg is an optional `Card`-type argument for matches to exclude from consideration."""
        for (card, ct) in self.cardCounts():
            if ct == n and card != exclude:
                return card
        return None

    def has_fullhouse(self) -> (Card,Card):
        """Defined as having both 3-of-a-kind and a pair, but of different card numbers."""
        the_triple = self.has_n_ofakind(3)
        the_pair   = self.has_n_ofakind(2, exclude=the_triple)
        if the_triple == None or the_pair == None:
            return None
        else:
            return (the_triple, the_pair)

    def has_1pair(self) -> int:
        """Defined as having *exactly* one pair of cards."""
        pair1   = self.has_n_ofakind(2)
        pair2   = self.has_n_ofakind(2, exclude=pair1)
        if pair2 == None:
            return pair1
        else:
            return None
    
    def has_2pair(self) -> (int, int):
        """Defined as having *exactly* two pairs of cards."""
        pair1   = self.has_n_ofakind(2)
        pair2   = self.has_n_ofakind(2, exclude=pair1)
        if pair1 != None and pair2 != None:
            return (pair1, pair2)
        else:
            return None
    
    def getStrength(self) -> int:
        """Used for sorting hands based on their strength."""
        if self.has_n_ofakind(5) != None:
            #print(f"hand has 5 of a kind: {str(self)}")
            return 7
        elif self.has_n_ofakind(4) != None:
            #print(f"hand has 4 of a kind: {str(self)}")
            return 6
        elif self.has_fullhouse() != None:
            #print(f"hand has full house : {str(self)}")
            return 5
        elif self.has_n_ofakind(3) != None:
            #print(f"hand has 3 of a kind: {str(self)}")
            return 4
        elif self.has_2pair() != None:
            #print(f"hand has 2 pairs    : {str(self)}")
            return 3
        elif self.has_1pair() != None:
            #print(f"hand has 1 pair     : {str(self)}")
            return 2
        else:
            #print(f"hand has high card  : {str(self)}")
            return 1

    def __eq__(self, rhs) -> bool:
        if type(rhs) != Hand:
            raise ValueError
        elif self.getStrength() != rhs.getStrength():
            return False
        else:
            for i in range(5):
                if self.cards[i] < rhs.cards[i]:
                    return False
            #print("equal cards found!", str(self), "and", str(rhs))
            return True

    def __lt__(self, rhs) -> bool:
        #print("-----------------------------")
        #print(f"comparing", str(self), " < ", str(rhs), "...")
        if type(rhs) != Hand:
            raise ValueError
        elif self.getStrength() < rhs.getStrength():
            #print("True : ", str(self), " < ", str(rhs), "chance1")
            return True
        elif self.getStrength() > rhs.getStrength():
            return False
        else:
            for i in range(5):
                #print(f"...comparing cards: {str(self.cards[i])=}, {str(rhs.cards[i])=}")
                if self.cards[i] < rhs.cards[i]:
                    #print("True : ", str(self), " < ", str(rhs))
                    return True
                elif self.cards[i] > rhs.cards[i]:
                    #print("True : ", str(self), " !< ", str(rhs))
                    return False
            #print("False: ", str(self), " !< ", str(rhs))
            return False

    def __str__(self):
        printer = ""
        for i in range(5):
            printer += str(self.cards[i])
            if i != 4:
                printer += ","
        return printer
# end class
        
def rankHands(handlist : [Hand]) -> [(int, Hand)]:
    """Given a list of `Hand`s, sorts the list and returns an `iterable` of tuples consisting of:
       [1] the rank of the hand, and [2] the hand itself."""
    handlist.sort()
    return zip(range(1, len(handlist)+1, 1), handlist)

def calculateWinnings(handlist : [Hand]) -> int:
    """A `Hand`'s winnings is defined as its rank multiplied by its bid value."""
    #for (rank, hand) in rankedhands:
        #print(f"{rank=}, {str(hand)=}, {hand.bid=}")
    return sum([rank * hand.bid for (rank, hand) in rankHands(handlist)])

def main():
    with open(argv[1], "r") as infile:
        linelist = infile.readlines()
    
    handlist = [Hand(line) for line in linelist]

    # Part 1
    print(f"Part 1: My total winnings are {calculateWinnings(handlist)}")
    return

if __name__ == "__main__":
    main()