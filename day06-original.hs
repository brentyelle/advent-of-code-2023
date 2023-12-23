{-# OPTIONS_GHC -Wno-unrecognised-pragmas #-}
{-# HLINT ignore "Use zipWith" #-}
import Data.List (intercalate)

testTimes :: [Integer]
testTimes = [7, 15, 30]
testRecords :: [Integer]
testRecords = [9, 40, 200]

realTimes :: [Integer]
realTimes = [38, 67, 76, 73]

realRecords :: [Integer]
realRecords = [234, 1027, 1157, 1236]

race :: Integer -> Integer -> Integer
race maxTime holdTime = holdTime * (maxTime - holdTime)

raceWin :: Integer -> Integer -> [Integer]
raceWin maxTime record = filter (> record) (map (race maxTime) [1..maxTime])

{-
h*(m-h) = r
m*h - h^2 = r
h^2 - m*h + r = 0
h = (m +/- sqrt(m^2 - 4r))/2
-}
raceWinEdges :: Integer -> Integer -> (Double, Double)
raceWinEdges maxTime record = (minus_sol, plus_sol) where
    plus_sol  = (m + sqrt (m*m - 4*r)) / 2
    minus_sol = (m - sqrt (m*m - 4*r)) / 2
    m = fromInteger maxTime
    r = fromInteger record

raceWinCount :: Integer -> Integer -> Integer
raceWinCount maxTime record = p' - m' + 1 where
    (m,p) = raceWinEdges maxTime record
    p' = ceiling (p-1)
    m' = floor (m+1)

getTimes :: [Integer] -> [Integer] -> Integer
getTimes t r = product $ map (uncurry raceWinCount) (zip t r)

partOne :: Integer
partOne = getTimes realTimes realRecords

partTwo :: Integer
partTwo = getTimes [tCat] [rCat] where
    tCat = read $ intercalate "" (map show realTimes)
    rCat = read $ intercalate "" (map show realRecords)
