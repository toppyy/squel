
#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Create empty table and read column names" {
    run ./build/squel "CREATE TABLE test_bats_tbl AS (numcol INT, strcol CHAR 100)" 
    run ./build/squel "SELECT * FROM test_bats_tbl" 
    [[ $"${lines[0]}" == "numcol;strcol" ]]
}

