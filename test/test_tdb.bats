
#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Create empty table and read column names" {
    run ./build/squel "CREATE TABLE test_bats_tbl AS (numcol INT, strcol CHAR 100)" 
    run ./build/squel "SELECT * FROM test_bats_tbl" 
    [[ $"${lines[0]}" == "numcol;strcol" ]]
}

@test "Can read records from TDB-table after insert" {
    run ./build/squel "CREATE TABLE test_bats_tbl AS (numcol INT, strcol CHAR 10)" 
    run ./build/squel "INSERT INTO test_bats_tbl SELECT numcol,strcol FROM './test/data/tdb_testdata.csv'"
    run ./build/squel "SELECT * FROM test_bats_tbl"
    [[ $"${lines[0]}" == "numcol;strcol" ]]
    [[ $"${lines[1]}" == "1;TDB" ]]
    [[ $"${lines[2]}" == "2;IS" ]]
    [[ $"${lines[3]}" == "3;A" ]]
    [[ $"${lines[4]}" == "4;FORMAT" ]]
}
