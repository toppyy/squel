
#!/usr/bin/env bats

setup_file() {
    run rm ./build/hashmap_test.o
    run gcc ./test/hashmap_test.c -o ./build/hashmap_test.o
    run gcc ./test/hashmap_test_nonexistent.c -o ./build/hashmap_test_nonexistent.o
    run gcc ./test/hashmap_test_multivalue.c -o ./build/hashmap_test_multivalue.o
}

@test "Hashmap functionality" {
    run ./build/hashmap_test.o
    [[ $"${lines[0]}" == "12345 in map with value 1442" ]]
    [[ $"${lines[1]}" == "12X46 in map" ]]
}

@test "Hashmap nonexistent key" {
    run ./build/hashmap_test_nonexistent.o
    [[ $"${lines[0]}" == "nonexistent key not in map" ]]
    [[ $"${lines[1]}" == "getValue for nonexistent key returns 0" ]]
}

@test "Hashmap multiple values and reset cursor" {
    run ./build/hashmap_test_multivalue.o
    [[ $"${lines[0]}" == "first value: 100" ]]
    [[ $"${lines[1]}" == "second value: 200" ]]
    [[ $"${lines[2]}" == "after reset: 100" ]]
}

