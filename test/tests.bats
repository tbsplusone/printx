setup() {
    load 'test_helper/bats-support/load'
    load 'test_helper/bats-assert/load'

    # get the containing directory of this file
    # use $BATS_TEST_FILENAME instead of ${BASH_SOURCE[0]} or $0,
    # as those will point to the bats executable's location or the preprocessed file respectively
    DIR="$( cd "$( dirname "$BATS_TEST_FILENAME" )" >/dev/null 2>&1 && pwd )"
    
    # make executables in src/ visible to PATH
    PATH="$DIR/../bin:$PATH"

    GIT_VERSION=$(git symbolic-ref -q --short HEAD || git describe --tags --exact-match)
}


@test "prints version as git branch or tag" {
    run printx -v
    assert_success
    assert_output $GIT_VERSION

    run printx --version
    assert_success
    assert_output $GIT_VERSION
}

@test "prints help" {
    expected_help_output=\
'Usage:
   printx [OPTIONS] FORMAT [ARGUMENT]...

OPTIONS:
   -h, --help         print this help
   -v, --version      print version
   -d, --debug        enable debug logging
   -a, --assing VAR   assign result to variable VAR

FORMAT sequences:
   \n   prints a newline
   %s   ARGUMENT is printed as string
   %%   a single %'

    run printx -h
    assert_success
    assert_output "$expected_help_output"

    run printx --help
    assert_success
    assert_output "$expected_help_output"
}

@test "prints format string as-is when no args" {
    run printx 'just a format string'
    assert_success
    assert_output 'just a format string'
}

@test "treats -- as options stopper" {
    run printx -- --version
    assert_success
    assert_output '--version'
}

@test "prints error on empty run" {
    run printx
    assert_failure
    assert_output \
'error: format not specified
usage: printx [-h, --help | -v, --version | -a VAR] FORMAT [ARGUMENT]...'
}

@test "prints error on unknown option" {
    run printx -u
    assert_failure
    assert_output \
'error: unknown option -u
usage: printx [-h, --help | -v, --version | -a VAR] FORMAT [ARGUMENT]...'

    run printx --unknown
    assert_failure
    assert_output \
'error: unknown option --unknown
usage: printx [-h, --help | -v, --version | -a VAR] FORMAT [ARGUMENT]...'
}

@test "prints error on missing assign VAR" {
    run printx -a
    assert_failure
    assert_output \
'error: value is not provided for -a / --assign
usage: printx [-h, --help | -v, --version | -a VAR] FORMAT [ARGUMENT]...'
}

@test "prints error on missing format" {
    run printx -a STUB
    assert_failure
    assert_output \
'error: format not specified
usage: printx [-h, --help | -v, --version | -a VAR] FORMAT [ARGUMENT]...'
}