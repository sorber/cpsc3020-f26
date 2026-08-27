#!/bin/sh
set -eu

check() {
    input=$1
    expected=$2
    output=$(./parser_demo "$input")
    echo "$output" | grep -F "$expected" >/dev/null
    printf 'PASS: %s\n' "$input"
}

check 'echo "hello world"' 'argv[1]=<hello world>'
check 'cat<input.txt|grep error>output.txt' 'commands=2'
check 'producer > tcp:localhost:9000' 'output: tcp localhost:9000'
check 'consumer < tcp:[::1]:8080' 'input: tcp ::1:8080'
check 'consumer < tcp:127.0.0.1:8080' 'input: tcp 127.0.0.1:8080'
check 'printf ""' 'argv[1]=<>'

if ./parser_demo 'cat |' >/dev/null 2>&1; then
    echo 'FAIL: malformed pipeline was accepted' >&2
    exit 1
fi
printf 'PASS: malformed pipeline rejected\n'
