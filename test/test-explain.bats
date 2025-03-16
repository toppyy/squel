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
    run ./build/squel "EXPLAIN SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3=int"
    expected_output=$(printf "******* EXPLAIN **********\nOP_PROJECT\nOP_HASHJOIN\nOP_FILTER\nOP_SCAN\nOP_SCAN\n**************************\n")
    assert_output "$expected_output"

}

@test "EXPLAIN - join with nested loop join" {
    run ./build/squel "EXPLAIN SELECT col1,col3,int FROM './test/data/small.csv' JOIN './test/data/small2.csv' ON col3>int"
    expected_output=$(printf "******* EXPLAIN **********\nOP_PROJECT\nOP_JOIN\nOP_FILTER\nOP_SCAN\nOP_SCAN\n**************************\n")
    assert_output "$expected_output"
}


