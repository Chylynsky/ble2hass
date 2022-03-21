# Copyright 2022 Borys Chyliński

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#!/bin/bash

declare -r BUILD_DIR="build-test"

declare -i BUILD_JOBS=8

declare -a REQUIRED_COMMANDS=(
    "make"
    "cmake"
)

# Colors for logs
declare -r NO_COLOR='\033[0m'
declare -r YELLOW='\033[0;33m'
declare -r RED='\033[0;31m'

function print_info() {
    echo "[INFO] ${1}"
}

function print_warning() {
    echo -e "${YELLOW}[WARNING]${NO_COLOR} ${1}"
}

function print_error() {
    echo -e "${RED}[ERROR]${NO_COLOR} ${1}"
}

print_info "Build directory set to \"${PWD}/${BUILD_DIR}\"."
print_info "Build set to generate ${BUILD_JOBS} jobs."

print_info "Checking for required commands..."

for command in ${REQUIRED_COMMANDS[@]}; do
    print_info "Checking \"${command}\" command..."
    if ! [ -x "$(command -v ${command})" ]; then
        print_error "Command \"${command}\" could not be found."
        exit
    fi
done

mkdir ${BUILD_DIR} &> /dev/null

print_info "Entering ${BUILD_DIR} directory."
print_info "Generating CMake files."

cd ${BUILD_DIR}
cmake ../test -DB2H_BUILD_BENCHMARK=OFF -DCMAKE_BUILD_TYPE=Debug

if ! [ $? -eq 0 ]; then
    print_error "CMake generation failed."
    exit
fi

print_info "Building the target."
make -j 8

if ! [ $? -eq 0 ]; then
    print_error "Target build failed."
    exit
fi

print_info "Done."
