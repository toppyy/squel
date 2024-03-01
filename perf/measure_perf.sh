#!bin/bash

echo Generate test data
./build/perf.out ./perf/data/numbers.csv $1

echo Start CSV-measures
/usr/bin/time -o ./perf/csv1.txt -f "Operation with $1 elements took: %E" ./build/squel "SELECT COUNT(col1) FROM './perf/data/numbers.csv' WHERE col1 < 50000"

echo Create test table TDB
./build/squel "CREATE TABLE perftest AS (col1 INT, col2 INT)" 
./build/squel "INSERT INTO perftest SELECT col1,col2 FROM './perf/data/numbers.csv'"

echo Start TDB-measures
/usr/bin/time -o ./perf/tdb1.txt -f "Operation with $1 elements took: %E" ./build/squel "SELECT COUNT(col1) FROM perftest WHERE col1 < 50000" 