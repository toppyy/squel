#!/usr/bin/env bash

setup() {
    load './test_helper/bats-support/load'
    load './test_helper/bats-assert/load'
    run make 
}

@test "EXPLAIN - subquery \w WHERE" {
    run ./build/squel "EXPLAIN SELECT col3 FROM (SELECT col3,col1 FROM './test/data/small.csv') WHERE col3>100"
    expected_output=$(printf "******* EXPLAIN **********\nOP_PROJECT\nOP_FILTER\nOP_PROJECT\nOP_SCAN\n**************************")
    assert_output "$expected_output"
}

@test "EXPLAIN - hash join" {
    run ./build/squel "EXPLAIN SELECT col1,col3,int FROM test_small JOIN test_small2 ON col3=int"
    [[ $"${lines[0]}" == "******* EXPLAIN **********" ]]
    [[ $"${lines[1]}" == "OP_PROJECT" ]]
    [[ $"${lines[2]}" == "OP_HASHJOIN" ]]
    [[ $"${lines[3]}" == "OP_FILTER" ]]
    [[ $"${lines[4]}" == "OP_SCANTDB" ]]
    [[ $"${lines[5]}" == "OP_SCANTDB" ]]
    [[ $"${lines[6]}" == "**************************" ]]
}



@test "EXPLAIN - join with nested loop join" {
    run ./build/squel "EXPLAIN SELECT col1,col3,int FROM test_small JOIN test_small2 ON col3>int"
    [[ $"${lines[0]}" == "******* EXPLAIN **********" ]]
    [[ $"${lines[1]}" == "OP_PROJECT" ]]
    [[ $"${lines[2]}" == "OP_JOIN" ]]
    [[ $"${lines[3]}" == "OP_FILTER" ]]
    [[ $"${lines[4]}" == "OP_SCANTDB" ]]
    [[ $"${lines[5]}" == "OP_SCANTDB" ]]
    [[ $"${lines[6]}" == "**************************" ]]
}


