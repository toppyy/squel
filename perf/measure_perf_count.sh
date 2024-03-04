#!bin/bash

echo Generate test data

echo "filetype;records;time" 1>&2

for i in 100000 1000000 10000000
do
    echo "Start CSV-measures for table sized $i"
    /usr/bin/time -f "CSV;$i;%E" ./build/squel "SELECT COUNT(col1) FROM './data/numbers_$i.csv' WHERE col1 < 500000"
    echo "Start TDB-measures for table sized $i"
    /usr/bin/time -f "TDB;$i;%E" ./build/squel "SELECT COUNT(col1) FROM perftest_$i WHERE col1 < 500000" 
done



