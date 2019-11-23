# Build like:
#   make FPGA=cvgt EN_SWAP=1
#

# FPGA device
FPGA         := svgx

# Enable/disable register word-swapping
EN_SWAP      := 0

# By default, just build a JTAG programming-file
all: defs


# C/C++ version of the project-specific definitions
defs: FORCE
	cat defs.vh | sed 's/localparam int/#define/g' | sed 's/=//g' | sed 's/;//g' | sed 's/1<</1U<</g;s/\((\d+)\)/\(\1U\)/g' > ../../driver/defs.h

# Clean up
clean: FORCE
	rm -rf build

FORCE:
