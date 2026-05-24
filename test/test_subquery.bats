
#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple subquery \w WHERE" {
    run ./build/squel "SELECT col3 FROM (SELECT col3,col1 FROM './test/data/small.csv') WHERE col3>100"
    [[ $"${lines[1]}" == "999" ]]
    [[ $"${lines[2]}" == "300" ]]
    [[ $"${lines[3]}" == "400" ]]
    [[ $"${lines[4]}" == "" ]]

}

@test "Simple subquery \w SELECT *" {
    run ./build/squel "SELECT col1 FROM (SELECT * FROM './test/data/small.csv')"
    [[ $"${lines[1]}" == "MAMA" ]]
    [[ $"${lines[2]}" == "UU" ]]
    [[ $"${lines[7]}" == "" ]]
}

@test "Simple subquery without WHERE" {
    run ./build/squel "SELECT col1 FROM (SELECT col1,col3 FROM './test/data/small.csv')"
    [[ $"${lines[1]}" == "MAMA" ]]
    [[ $"${lines[2]}" == "UU" ]]
    [[ $"${lines[7]}" == "" ]]
}

