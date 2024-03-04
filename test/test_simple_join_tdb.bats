#!/usr/bin/env bats

setup_file() {
    run make 
    run ./build/squel "CREATE TABLE test_small  AS (col1 CHAR 50, col2 CHAR 50, col3 INT)"
    run ./build/squel "CREATE TABLE test_small2 AS (char CHAR 50, int INT)" 
    run ./build/squel "INSERT INTO test_small  SELECT col1,col2,col3 FROM './test/data/small.csv'"
    run ./build/squel "INSERT INTO test_small2 SELECT char,int FROM './test/data/small2.csv'"
}

@test "Simple JOIN on column equality" {
    run ./build/squel "SELECT col1,col3,int FROM test_small JOIN test_small2 ON col3=int"
    [[ $"${lines[1]}" == "UU;999;999" ]]
    [[ $"${lines[2]}" == "MAMA;32;32" ]]
    [[ $"${lines[3]}" == "" ]]
}

@test "Simple JOIN on column unequality" {
    run ./build/squel "SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3>int"
    [[ $"${lines[1]}" == "UU;999;32" ]]
    [[ $"${lines[2]}" == "UU;100;32" ]]
    [[ $"${lines[3]}" == "UU;300;32" ]]
    [[ $"${lines[4]}" == "DEFG;400;32" ]]
    [[ $"${lines[5]}" == "" ]]
}


@test "Simple JOIN on column unequality using aliases" {
    run ./build/squel "SELECT a.col3 FROM './test/data/small.csv' AS a JOIN './test/data/small2.csv' AS b ON a.col3>b.int"
    [[ $"${lines[1]}" == "999" ]]
    [[ $"${lines[2]}" == "100" ]]
    [[ $"${lines[3]}" == "300" ]]
    [[ $"${lines[4]}" == "400" ]]
    [[ $"${lines[5]}" == "" ]]
}



