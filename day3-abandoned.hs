{-# OPTIONS_GHC -Wno-unrecognised-pragmas #-}
{-# HLINT ignore "Use camelCase" #-}
import Data.Char (isDigit)
import Data.List (find, findIndex, elemIndex, subsequences)
import Debug.Trace (trace)
--import BrentHelpers (splitOn)

data CharId = DIGIT | SYMBOL | PERIOD deriving (Eq, Show)
data ArrayNum = ArrayNum { row :: Int, start_col :: Int, end_col :: Int, string_form :: String, is_adj_to_symbol :: Bool} deriving (Eq)

instance Show ArrayNum where
    show :: ArrayNum -> String
    show x = show (string_form x, is_adj_to_symbol x)

-- Custom implementation of splitOn function, working with regular strings.
-- COPIED FROM BrentHelpers from my repository https://github.com/brentyelle/reinventing-the-haskell-wheel/
splitOn :: Char -> String -> [String]
splitOn c str
    | str == ""         = []
    | c `notElem` str   = [str]
    | otherwise         = str_up_to_c : splitOn c str_after_c
    where
        str_up_to_c     = fst broken_string
        str_after_c     = tail $ snd broken_string
        broken_string   = break (== c) str

-- same as splitOn above, but drops any empty strings from result
splitOnDrop :: Char -> String -> [String]
splitOnDrop c str = filter (not . null) (splitOn c str)

-- does Python-esque list slicing, except:
--  if `from` < 0, then just grabs from start of list
--  if `to` > len, then just grabs from end of list
sliceList :: Show a => Int -> Int -> [a] -> [a]
sliceList from to lst = take (to' - from') (drop from' lst)
    where
        from'   = max from 0
        to'     = min to (length lst)

maybeOp :: (a -> b -> c) -> Maybe a -> Maybe b -> Maybe c
maybeOp op (Just x) (Just y) = Just (x `op` y)
maybeOp _ _ _                = Nothing

x :: String
x = "422"
y :: String
y = "......*.....968*.....$............*........=..348...*..........986....*...................459....*........422................#......%482...."


subseqOfLength :: Int -> [a] -> [[a]]
subseqOfLength n lst = filter (\xs -> length xs == n) (firstPass n lst) where
    firstPass :: Int -> [a] -> [[a]]
    firstPass _ []  = []
    firstPass n lst = take n lst : firstPass n (tail lst)


-- Finds index of first character of search string (x:xs) within the larger string (y:ys).
-- If search string appears more than once, grabs the leftmost instance.
findSubstring :: String -> String -> Maybe Int
findSubstring [] _          = Just 0
findSubstring _ []          = Nothing
--findSubstring (x:xs) (y:ys) = if x==y then findSubstring xs (take (length xs) ys) else maybeOp (+) (Just 1) (findSubstring (x:xs) ys)
findSubstring x y           = x `elemIndex` subseqOfLength (length x) y

-- Finds index of last character of search string (x:xs) within the larger string (y:ys).
-- If search string appears more than once, grabs the rightmost instance.
findSubstringR :: String -> String -> Maybe Int
findSubstringR x y = maybeOp (-) (Just $ length y - 1) (findSubstring (reverse x) (reverse y))

findAllSubstring :: String -> String -> [Int]
findAllSubstring [] _       = []
findAllSubstring _ []       = []
findAllSubstring x y        = case findSubstring x y of
    Just n      -> n : findAllSubstring x (replicate (dropamount n) ' ' ++ drop (dropamount n) y)
    Nothing     -> []
    where
        dropamount n = n + length x

{- ==========================
    BEGIN SPECIALIZED CODE
========================== -}

-- Given a 
grabNumericStrings :: String -> [String]
grabNumericStrings = filter isDigitString . splitOnDrop '.' . suppressNonDigits where
    isDigitString :: String -> Bool
    isDigitString = foldr ((&&) . isDigit) True
    suppressNonDigits :: String -> String
    suppressNonDigits = map (\x -> if isDigit x then x else '.')

{-
getArrayNum_step1 :: String -> String -> ArrayNum
getArrayNum_step1 number_str full_line = ArrayNum (-1) startc endc number_str False where
    Just startc = number_str `elemIndex` subsequences full_line
    endc   = startc + length number_str - 1
-}

-- given a line of input, changes it into a proper Identities array
identifyLine :: String -> [CharId]
identifyLine = map typex where
    typex x
        | isDigit x     = DIGIT
        | x == '.'      = PERIOD
        | otherwise     = SYMBOL


getArrayNum :: (String,Int) -> [[CharId]] -> String -> ArrayNum
getArrayNum (this_line, row_i) identities number_str = ArrayNum row_i start_j end_j number_str (isAdjacentToSymbol identities row_i start_j end_j)
    where
    start_j = case findSubstring  number_str this_line of
        Just x -> x
        _      -> (trace $ "could not findL [" ++ show number_str ++ "] in [" ++ show this_line ++ "]") 0
    end_j   = start_j + length number_str
    isAdjacentToSymbol identities row_i start_j end_j
        | SYMBOL `elem` prev_position       =  True
        | SYMBOL `elem` next_position       =  True
        | SYMBOL `elem` row_below_sliced    =  True
        | SYMBOL `elem` row_above_sliced    =  True
        | otherwise                         = False
        where
            same_row        = identities !! row_i
            row_above       = if row_i > 0                     then identities !! (row_i - 1) else []
            row_below       = if row_i + 1 < length identities then identities !! (row_i + 1) else []
            prev_position   = sliceList (start_j-1) start_j same_row
            next_position   = sliceList (end_j+1) (end_j+2) same_row
            row_below_sliced= sliceList (start_j-1) (end_j+2) row_below
            row_above_sliced= sliceList (start_j-1) (end_j+2) row_above

getArrayNums :: (String,Int) -> [[CharId]] -> [String] -> [ArrayNum]
getArrayNums (line,line_no) ids numstrs = map (getArrayNum (line,line_no) ids) numstrs

isPartNumber :: ArrayNum -> Bool
isPartNumber = is_adj_to_symbol

calcArrayNumVal :: ArrayNum -> Int
calcArrayNumVal = read . string_form

main :: IO ()
main = do
    wholefile <- readFile "day3_input.txt"
    let fileLines       = splitOn '\n' wholefile            :: [String]
    --print $ head fileLines
    let identities      = map identifyLine fileLines        :: [[CharId]]
    --print $ head identities
    let numbers         = map grabNumericStrings fileLines  :: [[String]]
    --print $ head numbers
    let linesLabeled    = zip fileLines [0,1..]             :: [(String, Int)]
    --print $ linesLabeled
    let allnumbers      = zipWith (`getArrayNums` identities) linesLabeled numbers
    --print $ take 1 allnumbers
    let partnumbers     = map calcArrayNumVal . filter isPartNumber . concat $ allnumbers
    print $ "The result is " ++ show (sum partnumbers)
    return ()