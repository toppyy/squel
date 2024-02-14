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

@test "Simple WHERE \w Greater/less than \w constants." {
    run ./build/squel "SELECT col3 FROM './test/data/small.csv' WHERE col3<100"
    [[ $"${lines[0]}" == "32" ]]
    [[ $"${lines[1]}" == "2" ]]
    [[ $"${lines[2]}" == "" ]]

    run ./build/squel "SELECT col3 FROM './test/data/small.csv' WHERE col3>100"
    [[ $"${lines[0]}" == "999" ]]
    [[ $"${lines[1]}" == "300" ]]
    [[ $"${lines[2]}" == "400" ]]
    [[ $"${lines[3]}" == "" ]]

    run ./build/squel "SELECT col3 FROM './test/data/small.csv' WHERE 998<col3"
    [[ $"${lines[0]}" == "999" ]]
    [[ $"${lines[1]}" == "" ]]

}

@test "Simple WHERE \w Greater/less than \w only constants." {

    run ./build/squel "SELECT col3 FROM './test/data/small.csv' WHERE 1=1"
    [[ $"${lines[0]}" == "32" ]]
    [[ $"${lines[1]}" == "999" ]]
    [[ $"${lines[2]}" == "100" ]]
    [[ $"${lines[3]}" == "300" ]]
    [[ $"${lines[4]}" == "400" ]]
    [[ $"${lines[5]}" == "2" ]]
    [[ $"${lines[6]}" == "" ]]
}

@test "Simple WHERE \w two conditions (AND)." {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv' WHERE col1='UU' AND col3<900"
    [[ $"${lines[0]}" == "UU;UU;100" ]]
    [[ $"${lines[1]}" == "UU;CCCCDDD;300" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple WHERE \w three conditions (AND)." {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv' WHERE col1='UU' AND col1=col2 AND col3<900"
    [[ $"${lines[0]}" == "UU;UU;100" ]]
    [[ $"${lines[1]}" == "" ]]
}

@test "Simple WHERE \w two conditions (OR)." {
    run ./build/squel "SELECT char FROM './test/data/small2.csv' WHERE char='OTHER' OR char='DATA'"
    [[ $"${lines[0]}" == "OTHER" ]]
    [[ $"${lines[1]}" == "DATA" ]]
    [[ $"${lines[2]}" == "" ]]
}
