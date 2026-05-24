#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Join animals to fruits duplicating rows" {
    run ./build/squel "SELECT a.asize,a.animal,f.fruit FROM './test/data/animals.csv' AS a JOIN './test/data/fruits.csv' AS f ON a.asize=f.fsize"
    [[ $"${lines[0]}" == "asize;animal;fruit" ]]
    [[ $"${lines[1]}" == "small;monkey;grape" ]]
    [[ $"${lines[2]}" == "small;monkey;strawberry" ]]
    [[ $"${lines[3]}" == "small;cat;grape" ]]
    [[ $"${lines[4]}" == "small;cat;strawberry" ]]
    [[ $"${lines[5]}" == "very big;whale;watermelon" ]]
    [[ $"${lines[6]}" == "medium;horse;orange" ]]

}

@test "Join fruits to animals duplicating rows" {
    run ./build/squel "SELECT a.asize,a.animal,f.fruit FROM './test/data/fruits.csv' AS f JOIN './test/data/animals.csv' AS a ON a.asize=f.fsize"
    [[ $"${lines[0]}" == "asize;animal;fruit" ]]
    [[ $"${lines[1]}" == "small;monkey;grape" ]]
    [[ $"${lines[2]}" == "small;cat;grape" ]]
    [[ $"${lines[3]}" == "small;monkey;strawberry" ]]
    [[ $"${lines[4]}" == "small;cat;strawberry" ]]
    [[ $"${lines[5]}" == "very big;whale;watermelon" ]]
    [[ $"${lines[6]}" == "medium;horse;orange" ]]

}

@test "Join animals to fruits with WHERE filter" {
    run ./build/squel "SELECT a.animal,f.fruit FROM './test/data/animals.csv' AS a JOIN './test/data/fruits.csv' AS f ON a.asize=f.fsize WHERE a.asize='small'"
    [[ $"${lines[0]}" == "animal;fruit" ]]
    [[ $"${lines[1]}" == "monkey;grape" ]]
    [[ $"${lines[2]}" == "monkey;strawberry" ]]
    [[ $"${lines[3]}" == "cat;grape" ]]
    [[ $"${lines[4]}" == "cat;strawberry" ]]
    [[ $"${lines[5]}" == "" ]]
}

@test "Join with selected column from one table only" {
    run ./build/squel "SELECT f.fruit FROM './test/data/animals.csv' AS a JOIN './test/data/fruits.csv' AS f ON a.asize=f.fsize"
    [[ $"${lines[0]}" == "fruit" ]]
    [[ $"${lines[1]}" == "grape" ]]
    [[ $"${lines[2]}" == "strawberry" ]]
    [[ $"${lines[3]}" == "grape" ]]
    [[ $"${lines[4]}" == "strawberry" ]]
    [[ $"${lines[5]}" == "watermelon" ]]
    [[ $"${lines[6]}" == "orange" ]]
    [[ $"${lines[7]}" == "" ]]
}
