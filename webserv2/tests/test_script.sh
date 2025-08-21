#!/bin/bash

echo "🧪 Request Class Testing Script"
echo "=============================="
echo

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✅ $2${NC}"
    else
        echo -e "${RED}❌ $2${NC}"
    fi
}

print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

echo "1. Compiling Request class..."
g++ -std=c++98 -Wall -Wextra -Werror -I../include -c ../src/Request.cpp -o ../obj/Request.o 2>/dev/null
print_status $? "Request.cpp compilation"

echo
echo "2. Running basic unit tests..."
if [ -f "test_simple" ]; then
    rm test_simple
fi

g++ -std=c++98 -Wall -Wextra -Werror -I../include test_simple.cpp ../src/Request.cpp -o test_simple 2>/dev/null
if [ $? -eq 0 ]; then
    ./test_simple > /dev/null 2>&1
    print_status $? "Basic unit tests"
else
    print_status 1 "Test compilation failed"
fi

echo
echo "3. Checking file structure..."
[ -f "../include/Request.hpp" ] && print_status 0 "Request.hpp exists" || print_status 1 "Request.hpp missing"
[ -f "../src/Request.cpp" ] && print_status 0 "Request.cpp exists" || print_status 1 "Request.cpp missing"

echo
echo "4. Code quality checks..."

# Check for basic required functions
grep -q "get_method" ../include/Request.hpp && print_status 0 "get_method() declared" || print_status 1 "get_method() missing"
grep -q "get_path" ../include/Request.hpp && print_status 0 "get_path() declared" || print_status 1 "get_path() missing"
grep -q "get_body" ../include/Request.hpp && print_status 0 "get_body() declared" || print_status 1 "get_body() missing"
grep -q "ready" ../include/Request.hpp && print_status 0 "ready() declared" || print_status 1 "ready() missing"
grep -q "is_valid" ../include/Request.hpp && print_status 0 "is_valid() declared" || print_status 1 "is_valid() missing"

echo
echo "5. Implementation checks..."
grep -q "get_request_first_line" ../src/Request.cpp && print_status 0 "Request line parsing implemented" || print_status 1 "Request line parsing missing"
grep -q "get_request_line" ../src/Request.cpp && print_status 0 "Header parsing implemented" || print_status 1 "Header parsing missing"
grep -q "checkHeader" ../src/Request.cpp && print_status 0 "Header validation implemented" || print_status 1 "Header validation missing"

echo
print_info "Testing complete!"
print_info "For full HTTP testing, integrate with your webserver and use:"
print_info "  - curl commands (see TESTING_GUIDE.md)"
print_info "  - telnet for raw HTTP"
print_info "  - Custom test clients"

echo
echo "📁 Generated files:"
echo "  - test_simple (basic test executable)"
echo "  - TESTING_GUIDE.md (comprehensive testing guide)"
echo "  - test_request.cpp (advanced test template)"
echo "  - obj/Request.o (compiled object file)"
