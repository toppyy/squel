#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple SELECT 1" {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "MAMA;ABC;32" ]]
    [[ $"${lines[1]}" == "UU;DEFG;999" ]]
}

@test "Simple SELECT 2" {
    run ./build/squel "SELECT col2,col1,col3 FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "ABC;MAMA;32" ]]
    [[ $"${lines[1]}" == "DEFG;UU;999" ]]
}

@test "Simple SELECT 3" {
    run ./build/squel "SELECT col3 FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "32" ]]
    [[ $"${lines[1]}" == "999" ]]
    [[ $"${lines[2]}" == "100" ]]
    [[ $"${lines[3]}" == "300" ]]
    [[ $"${lines[4]}" == "400" ]]
    [[ $"${lines[5]}" == "2" ]]
    [[ $"${lines[6]}" == "" ]]
}