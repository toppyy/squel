#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple COUNT" {
    run ./build/squel "SELECT COUNT(unemployed) FROM './test/data/unemployment.csv'"
    [[ $"${lines[1]}" == "213" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple SUM" {
    run ./build/squel "SELECT SUM(unemployed) FROM './test/data/unemployment.csv'"
    [[ $"${lines[1]}" == "6867380" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple AVG" {
    run ./build/squel "SELECT AVG(unemployed) FROM './test/data/unemployment.csv'"
    [[ $"${lines[1]}" == "32241.22" ]]
    [[ $"${lines[2]}" == "" ]]
}

