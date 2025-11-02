
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
