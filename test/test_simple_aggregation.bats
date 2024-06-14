#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple COUNT" {
    run ./build/squel "SELECT COUNT(unemployed) FROM './test/data/unemployment_all.csv'"
    [[ $"${lines[1]}" == "213" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple SUM" {
    run ./build/squel "SELECT SUM(unemployed) FROM './test/data/unemployment_all.csv'"
    [[ $"${lines[1]}" == "6867380" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple AVG" {
    run ./build/squel "SELECT AVG(unemployed) FROM './test/data/unemployment_all.csv'"
    [[ $"${lines[1]}" == "32241" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple MAX" {
    run ./build/squel "SELECT MAX(unemployed) FROM './test/data/unemployment_all.csv'"
    [[ $"${lines[1]}" == "65210" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "Simple MIN" {
    run ./build/squel "SELECT MIN(unemployed) FROM './test/data/unemployment_all.csv'"
    [[ $"${lines[1]}" == "16946" ]]
    [[ $"${lines[2]}" == "" ]]
}

@test "SUM on subquery \w join" {
    run ./build/squel "SELECT SUM(unemployed) FROM (SELECT lt.long_term_unemployed,lt.time,u.unemployed FROM './test/data/lt_unemployed.csv' AS lt JOIN './test/data/unemployed.csv' AS u ON u.time=lt.time)"
    [[ $"${lines[1]}" == "6867380" ]]
    [[ $"${lines[2]}" == "" ]]
}