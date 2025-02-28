#!/bin/sh
set -e

TARGET=x86_64-unknown-darwin-macho
HERE=`dirname $0`

RUST_BACKTRACE=1 cargo run --manifest-path $HERE/Cargo.toml --quiet -- -t $TARGET -o tmp_run.o
cc -target $TARGET -o tmp_run tmp_run.o $HERE/rt.o
./tmp_run $@
rm tmp_run.o tmp_run
