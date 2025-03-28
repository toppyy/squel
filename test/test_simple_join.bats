#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple JOIN on column equality" {
    run ./build/squel "SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3=int"
    [[ $"${lines[1]}" == "MAMA;32;32" ]]
    [[ $"${lines[2]}" == "UU;999;999" ]]
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

@test "Hashjoin with small hashtable" {
    run ./build/squel --htsize 10 "SELECT COUNT(u.unemployed) FROM './test/data/lt_unemployed.csv' AS lt JOIN './test/data/unemployed.csv' AS u ON u.time=lt.time"
    [[ $"${lines[0]}" == "unemployed" ]]
    [[ $"${lines[1]}" == "213" ]]
    [[ $"${lines[2]}" == "" ]]
}


