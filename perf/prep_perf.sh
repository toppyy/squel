#!bin/bash
gcc ./perf/gen_test_data.c -o build/perf.out

echo Generate test data

for i in 10 100 1000 10000 100000 1000000 10000000
do
   echo "Generating $i records"
    (./build/perf.out ./data/numbers_$i.csv $i)
    (chmod +r ./data/numbers_$i.csv)
    echo " - Storing the in a tdb-table"
    ./build/squel "CREATE TABLE perftest_$i AS (col1 INT, col2 INT)" 
    ./build/squel "INSERT INTO perftest_$i SELECT col1,col2 FROM './data/numbers_$i.csv'"    
done
