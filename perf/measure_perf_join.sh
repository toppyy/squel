#!bin/bash


echo "filetype;records_left;records_right;time" 1>&2

for left in 10000 100000
do 
    for right in 100 1000
    do
        echo "Start CSV-measures for join $left vs $right"
        cmd="SELECT COUNT(col1) FROM './data/numbers_$left.csv' AS a JOIN './data/numbers_$right.csv' AS b ON a.col1<b.col1 AND a.col2<b.col2"
        /usr/bin/time -f "CSV;$left;$right;%E" ./build/squel "$cmd"
        echo "Start TDB-measures for join $left vs $right"
        cmd="SELECT COUNT(a.col1) FROM perftest_$left AS a JOIN perftest_$right AS b ON a.col1<b.col1 AND a.col2<b.col2"
        /usr/bin/time -f "TDB;$left;$right;%E" ./build/squel "$cmd"
    done
done





