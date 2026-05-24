
#!/usr/bin/env bats

setup_file() {
    rm -f data/test_bats_tbl.tdb
    run make 
}

@test "Create empty table and read column names" {
    run ./build/squel "CREATE TABLE test_bats_tbl AS (numcol INT, strcol CHAR(50))" 
    run ./build/squel "SELECT * FROM test_bats_tbl" 
    [[ $"${lines[0]}" == "numcol;strcol" ]]
}

@test "Can read records from TDB-table after insert" {
    run ./build/squel "CREATE TABLE test_bats_tbl AS (numcol INT, strcol CHAR (50))" 
    run ./build/squel "INSERT INTO test_bats_tbl SELECT numcol,strcol FROM './test/data/tdb_testdata.csv'"
    run ./build/squel "SELECT * FROM test_bats_tbl"
    [[ $"${lines[0]}" == "numcol;strcol" ]]
    [[ $"${lines[4]}" == "1;TDB" ]]
    [[ $"${lines[3]}" == "2;IS" ]]
    [[ $"${lines[2]}" == "3;A" ]]
    [[ $"${lines[1]}" == "4;FORMAT" ]]
}

@test "Can SELECT with WHERE on TDB table" {
    run ./build/squel "CREATE TABLE test_where_tbl AS (numcol INT, strcol CHAR(50))"
    run ./build/squel "INSERT INTO test_where_tbl SELECT numcol,strcol FROM './test/data/tdb_testdata.csv'"
    run ./build/squel "SELECT * FROM test_where_tbl WHERE numcol>2"
    [[ $"${lines[0]}" == "numcol;strcol" ]]
    [[ $"${lines[1]}" == "3;A" ]]
    [[ $"${lines[2]}" == "4;FORMAT" ]]
    [[ $"${lines[3]}" == "" ]]
}

@test "Can SELECT reordered columns from TDB" {
    run ./build/squel "CREATE TABLE test_reorder_tbl AS (numcol INT, strcol CHAR(50))"
    run ./build/squel "INSERT INTO test_reorder_tbl SELECT numcol,strcol FROM './test/data/tdb_testdata.csv'"
    run ./build/squel "SELECT strcol,numcol FROM test_reorder_tbl"
    [[ $"${lines[0]}" == "strcol;numcol" ]]
    [[ $"${lines[1]}" == "TDB;1" ]]
    [[ $"${lines[2]}" == "IS;2" ]]
    [[ $"${lines[3]}" == "A;3" ]]
    [[ $"${lines[4]}" == "FORMAT;4" ]]
    [[ $"${lines[5]}" == "" ]]
}
