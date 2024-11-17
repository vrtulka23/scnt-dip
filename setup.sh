#!/usr/bin/env bash

set -e

. settings.env
DIR_ROOT=$(pwd)

function clean_code {
    if [[ -d $DIR_BUILD ]]; then
	    rm -r $DIR_BUILD
    fi
}

function build_code {    
    if [[ ! -d $DIR_BUILD ]]; then
	    mkdir $DIR_BUILD
    fi
    cmake -B $DIR_BUILD -DCODE_VERSION="${CODE_VERSION}" 
    cd $DIR_BUILD
    make -j 10
    cd $DIR_ROOT
}

function install_code {
    cd $DIR_BUILD
    make install
    cd $DIR_ROOT
}

function test_code {
    if [[ "${1}" == "" ]]; then
	    ./$DIR_BUILD/TestSolver
    else
        ./$DIR_BUILD/TestSolver --gtest_filter="${1}"
    fi
}

function run_code {
    ./$DIR_BUILD/$1
}

function grep_code {
    if [[ "${1}" != "" ]]; then
	    grep -Inr $1 ./src ./gtest ./exec
    fi
}

function show_help {
    echo "Physical Units and Quantities"
    echo ""
    echo "Options:"
    echo " -c|--clean          clean the build directory"
    echo " -b|--build          build code"
    echo " -i|--install        install dip"
    echo " -r|--run <example>  run an example code"
    echo " -t|--test [<test>]  run all/specific tests"
    echo " -g|--grep <expr>    find expression in a code"
    echo " -h|--help           show this help"
    echo ""
    echo "Examples:"
    echo "./setup.sh -c -b               clean and build the code"
    echo "./setup.sh -t Output.Data      run 'Output.Data' test"
    echo "./setup.sh -g class            find 'class' in the code or test"
}

if [[ "${1}" == "" ]]; then
    show_help
fi
while [[ $# -gt 0 ]]; do
    case $1 in
	-c|--clean)
	    clean_code; shift;;
	-b|--build)
	    build_code; shift;;
	-i|--install)
	    install_code; shift;;
	-r|--run)
	    run_code $2; shift; shift;;
	-t|--test)
	    test_code $2; shift; shift;;
	-g|--grep)
	    grep_code $2; shift; shift;;
	-h|--help)
	    show_help; shift;;
	-*|--*)
	    show_help; exit 1;;
	*)
	    show_help; shift;;
    esac
done
