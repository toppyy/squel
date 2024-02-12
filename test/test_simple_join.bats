#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple JOIN on column equality" {
    run ./build/squel "SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3=int"
    [[ $"${lines[0]}" == "MAMA;32;32" ]]
    [[ $"${lines[1]}" == "UU;999;999" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple JOIN on column unequality" {
    run ./build/squel "SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3>int"
    [[ $"${lines[0]}" == "UU;999;32" ]]
    [[ $"${lines[1]}" == "UU;100;32" ]]
    [[ $"${lines[2]}" == "UU;300;32" ]]
    [[ $"${lines[3]}" == "DEFG;400;32" ]]
    [[ $"${lines[4]}" == "" ]]
}





