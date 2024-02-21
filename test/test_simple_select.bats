#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple SELECT 1" {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "col1;col2;col3" ]]
    [[ $"${lines[1]}" == "MAMA;ABC;32" ]]
    [[ $"${lines[2]}" == "UU;DEFG;999" ]]
}

@test "Simple SELECT 2" {
    run ./build/squel "SELECT col2,col1,col3 FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "col2;col1;col3" ]]
    [[ $"${lines[1]}" == "ABC;MAMA;32" ]]
    [[ $"${lines[2]}" == "DEFG;UU;999" ]]
}

@test "Simple SELECT 3" {
    run ./build/squel "SELECT col3 FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "col3" ]]
    [[ $"${lines[1]}" == "32" ]]
    [[ $"${lines[2]}" == "999" ]]
    [[ $"${lines[3]}" == "100" ]]
    [[ $"${lines[4]}" == "300" ]]
    [[ $"${lines[5]}" == "400" ]]
    [[ $"${lines[6]}" == "2" ]]
    [[ $"${lines[7]}" == "" ]]
}

@test "Simple SELECT *" {
    run ./build/squel "SELECT * FROM './test/data/small.csv'"
    [[ $"${lines[0]}" == "col1;col2;col3" ]]
    [[ $"${lines[1]}" == "MAMA;ABC;32" ]]
    [[ $"${lines[2]}" == "UU;DEFG;999" ]]
    [[ $"${lines[3]}" == "UU;UU;100" ]]
    [[ $"${lines[4]}" == "UU;CCCCDDD;300" ]]
    [[ $"${lines[5]}" == "DEFG;DEFG;400" ]]
    [[ $"${lines[6]}" == "ABC;ZYX;2" ]]
    [[ $"${lines[7]}" == "" ]]


}