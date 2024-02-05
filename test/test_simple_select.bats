#!/usr/bin/env bats

@test "Simple SELECT 1" {
    run ./build/squel "SELECT col1,col2,col3 FROM './test/data/small.csv'"

    [[ $"${lines[0]}" == "MAMA;ABC;32" ]]
    [[ $"${lines[1]}" == "UU;DEFG;99" ]]
    
}

