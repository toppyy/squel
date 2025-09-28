#!bin/bash

echo "filetype;records_left;records_right;time;hashjoin" 1>&2

for left in 100000
do
    for right in 1000 # Hashtable is created from this
    do
        # Add 5 rows from left to right to get matches
        cp ./data/numbers_$right.csv ./data/numbers_tmp_$right.csv
        tail -n 5 ./data/numbers_$left.csv >> ./data/numbers_tmp_$right.csv

        cmd="SELECT COUNT(col1) FROM './data/numbers_$left.csv' AS a JOIN './data/numbers_tmp_$right.csv' AS b ON b.col1 = a.col2"
        /usr/bin/time -f "CSV;$left;$right;enabled;%E" ./build/squel "$cmd"

        cmd="SELECT COUNT(col1) FROM './data/numbers_$left.csv' AS a JOIN './data/numbers_tmp_$right.csv' AS b ON b.col1 = a.col2"
        /usr/bin/time -f "CSV;$left;$right;disenabled;%E" ./build/squel --hashjoin 0 "$cmd"
    done
done





