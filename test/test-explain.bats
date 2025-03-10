
#!/usr/bin/env bats

setup_file() {
    run make 
}

@test "Simple subquery \w WHERE" {
    run ./build/squel "EXPLAIN SELECT col3 FROM (SELECT col3,col1 FROM './test/data/small.csv') WHERE col3>100"
    [[ $"${lines[0]}" == "******* EXPLAIN **********" ]]
    [[ $"${lines[1]}" == "OP_PROJECT" ]]
    [[ $"${lines[2]}" == "OP_FILTER" ]]
    [[ $"${lines[3]}" == "OP_PROJECT" ]]
    [[ $"${lines[4]}" == "OP_SCAN" ]]
    [[ $"${lines[5]}" == "**************************" ]]
}

@test "EXPLAIN a query" {
    run ./build/squel "EXPLAIN SELECT col1,col3,int FROM test_small JOIN test_small2 ON col3=int"
    [[ $"${lines[0]}" == "******* EXPLAIN **********" ]]
    [[ $"${lines[1]}" == "OP_PROJECT" ]]
    [[ $"${lines[2]}" == "OP_JOIN" ]]
    [[ $"${lines[3]}" == "OP_FILTER" ]]
    [[ $"${lines[4]}" == "OP_SCANTDB" ]]
    [[ $"${lines[5]}" == "OP_SCANTDB" ]]
    [[ $"${lines[6]}" == "**************************" ]]
}
