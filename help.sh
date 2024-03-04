# ./build/squel "CREATE TABLE test_bats_tbl AS (numcol INT, strcol CHAR 50)" 
# ./build/squel "INSERT INTO test_bats_tbl SELECT numcol,strcol FROM './test/data/tdb_testdata.csv'"
## valgrind ./build/squel "SELECT numcol,strcol FROM test_bats_tbl"
valgrind  ./build/squel "SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3=int"
#valgrind ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv'"

#cat ./test/data/tdb_testdata.csv

#valgrind ./build/squel "SELECT lt.long_term_unemployed,lt.time,u.unemployed FROM './test/data/lt_unemployed.csv' AS lt JOIN './test/data/unemployed.csv' AS u ON u.time=lt.time"

#valgrind ./build/squel "SELECT SUM(unemployed) FROM (SELECT lt.long_term_unemployed,lt.time,u.unemployed FROM './test/data/lt_unemployed.csv' AS lt JOIN './test/data/unemployed.csv' AS u ON u.time=lt.time)"


# ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv'"