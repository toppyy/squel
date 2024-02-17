#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple COUNT" {
    run ./build/squel "SELECT COUNT(unemployed) FROM './test/data/unemployment.csv'"
    [[ $"${lines[0]}" == "213" ]]
    [[ $"${lines[1]}" == "" ]]
}

@test "Simple SUM" {
    run ./build/squel "SELECT SUM(unemployed) FROM './test/data/unemployment.csv'"
    [[ $"${lines[0]}" == "6867380" ]]
    [[ $"${lines[1]}" == "" ]]
}