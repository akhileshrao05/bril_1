# Set TARGET to cross-compile. Brilift's AOT mode doesn't yet support macOS on
# ARM, so on Apple Silicon devices, you'll need to do this to run compiled
# programs through Rosetta 2:
# TARGET := x86_64-unknown-darwin-macho

# Brilift only supports core Bril for now, so we select those tests &
# benchmarks.
TESTS := ../test/interp/core/*.bril ../test/interp/float/*.bril
BENCHMARKS := $(shell grep -L 'ptr' ../benchmarks/*.bril)

CFLAGS := $(if $(TARGET),-target $(TARGET))
BRILFLAGS := $(if $(TARGET),-t $(TARGET))

foo:
	echo $(BENCHMARKS)

.PHONY: build
build:
	cargo build

.PHONY: install
install:
	cargo install --path .

.PHONY: test
test: rt.o
	turnt -e brilift-aot -e brilift-jit $(TESTS)

.PHONY: benchmark
benchmark: rt.o
	turnt -e brilift-aot -e brilift-jit $(BENCHMARKS)

rt.o: rt.c
	cc $(CFLAGS) -c -o $@ $^

%.o: %.bril
	bril2json < $^ | ./target/release/brilift $(BRILFLAGS) -o $@

%: %.o rt.o
	cc $(CFLAGS) -o $@ $^
