
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

