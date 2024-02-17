#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple COUNT" {
    run ./build/squel "SELECT COUNT(1) FROM './test/data/unemployment.csv'"
    [[ $"${lines[0]}" == "213" ]]
}

