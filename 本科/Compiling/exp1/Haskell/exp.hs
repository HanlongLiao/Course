import Data.List
import Data.Time
import System.IO

merge :: [Int]->[Int]->[Int]
merge xs [] = xs
merge [] ys = ys
merge (x:xs) (y:ys)
        | x > y = y:merge (x:xs) ys
        | otherwise = x:merge xs (y:ys)

mergeSort :: [Int]->[Int]
mergeSort []=[]
mergeSort (x:[])=(x:[])
mergeSort xs = merge (mergeSort x1) (mergeSort x2)
        where
            (x1, x2) = split xs
            split xs = (take mid xs, drop mid xs)
            mid = (length xs) `div` 2 

main=do
    f <- readFile "F:\\vscodework\\Compiling\\test100000.txt"
    let m = words f
    starttime<-getCurrentTime
    let l = mergeSort(map read m)
    putStrLn("Data Size:"++show(length(l))) 
    endtime<-getCurrentTime
    putStrLn("Time:"++show(endtime `diffUTCTime` starttime))