#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Join animals to fruits duplicating rows" {
    run ./build/squel "SELECT a.size,a.animal,f.fruit FROM './test/data/animals.csv' AS a JOIN './test/data/fruits.csv' AS f ON a.size=f.size"
    [[ $"${lines[0]}" == "size;animal;fruit" ]]
    [[ $"${lines[1]}" == "small;monkey;grape" ]]
    [[ $"${lines[2]}" == "small;monkey;strawberry" ]]
    [[ $"${lines[3]}" == "small;cat;grape" ]]
    [[ $"${lines[4]}" == "small;cat;strawberry" ]]
    [[ $"${lines[5]}" == "very big;whale;watermelon" ]]
    [[ $"${lines[6]}" == "medium;horse;orange" ]]

}

@test "Join fruits to animals duplicating rows" {
    run ./build/squel "SELECT a.size,a.animal,f.fruit FROM './test/data/fruits.csv' AS f JOIN './test/data/animals.csv' AS a ON a.size=f.size"
    [[ $"${lines[0]}" == "size;animal;fruit" ]]
    [[ $"${lines[1]}" == "small;monkey;grape" ]]
    [[ $"${lines[2]}" == "small;cat;grape" ]]
    [[ $"${lines[3]}" == "small;monkey;strawberry" ]]
    [[ $"${lines[4]}" == "small;cat;strawberry" ]]
    [[ $"${lines[5]}" == "very big;whale;watermelon" ]]
    [[ $"${lines[6]}" == "medium;horse;orange" ]]

}
