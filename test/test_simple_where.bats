#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple WHERE \w Equality." {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv' WHERE col1=col2"
    [[ $"${lines[0]}" == "UU;UU;100" ]]
    [[ $"${lines[1]}" == "DEFG;DEFG;400" ]]
}

@test "Simple WHERE \w Inequality." {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv' WHERE col1!=col2"
    [[ $"${lines[0]}" == "MAMA;ABC;32" ]]
    [[ $"${lines[1]}" == "UU;DEFG;999" ]]
    [[ $"${lines[2]}" == "UU;CCCCDDD;300" ]]
}

@test "Simple WHERE \w Less than." {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv' WHERE col1<col2"
    [[ $"${lines[0]}" == "ABC;ZYX;2" ]]
}

@test "Simple WHERE \w Greater than." {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv' WHERE col1>col2"
    [[ $"${lines[0]}" == "MAMA;ABC;32" ]]
    [[ $"${lines[1]}" == "UU;DEFG;999" ]]
    [[ $"${lines[2]}" == "UU;CCCCDDD;300" ]]
}