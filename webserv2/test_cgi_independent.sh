#!/bin/bash

echo "🧪 CGI Standalone Testing Suite"
echo "================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to run a test and check result
run_test() {
    local test_name="$1"
    local command="$2"
    
    echo -e "\n${BLUE}🔍 Testing: $test_name${NC}"
    echo "Command: $command"
    echo "----------------------------------------"
    
    if eval "$command"; then
        echo -e "${GREEN}✅ PASS: $test_name${NC}"
        return 0
    else
        echo -e "${RED}❌ FAIL: $test_name${NC}"
        return 1
    fi
}

# Check if required files exist
echo -e "\n${YELLOW}📋 Pre-flight Checks${NC}"

if [ ! -f "tmp/www/test.php" ]; then
    echo -e "${RED}❌ test.php not found${NC}"
    exit 1
fi

if [ ! -f "tmp/www/test.py" ]; then
    echo -e "${RED}❌ test.py not found${NC}"
    exit 1
fi

# Check for PHP availability
if command -v php >/dev/null 2>&1; then
    echo -e "${GREEN}✅ PHP interpreter found${NC}"
    PHP_AVAILABLE=true
else
    echo -e "${YELLOW}⚠️  PHP interpreter not found - PHP tests will be skipped${NC}"
    PHP_AVAILABLE=false
fi

echo -e "${GREEN}✅ All test files present${NC}"

# Compile the standalone test
echo -e "\n${YELLOW}🔨 Compiling standalone CGI test...${NC}"
if g++ -std=c++11 -Wall -Wextra -Werror -I. test_cgi_standalone.cpp src/CGIExecutor.cpp -o test_cgi_standalone; then
    echo -e "${GREEN}✅ Compilation successful${NC}"
else
    echo -e "${RED}❌ Compilation failed${NC}"
    exit 1
fi

# Run the compiled test
echo -e "\n${YELLOW}🚀 Running CGI Tests${NC}"
run_test "Standalone CGI Test Suite" "./test_cgi_standalone"

# Direct interpreter tests
echo -e "\n${YELLOW}🔧 Direct Interpreter Tests${NC}"

# Test PHP directly - only if available
if [ "$PHP_AVAILABLE" = true ]; then
    run_test "PHP Direct Execution" "echo 'GET /test.php?name=DirectTest HTTP/1.1' | php tmp/www/test.php"
else
    echo -e "\n${BLUE}🔍 Testing: PHP Direct Execution${NC}"
    echo -e "${YELLOW}⚠️  SKIP: PHP not available${NC}"
fi

# Test Python directly  
run_test "Python Direct Execution" "QUERY_STRING='name=DirectTest' REQUEST_METHOD='GET' python3 tmp/www/test.py"

# Test with environment variables
echo -e "\n${YELLOW}🌍 Environment Variable Tests${NC}"

if [ "$PHP_AVAILABLE" = true ]; then
    run_test "PHP with Environment" "REQUEST_METHOD='POST' QUERY_STRING='test=env' CONTENT_LENGTH='11' php tmp/www/test.php <<< 'hello world'"
else
    echo -e "\n${BLUE}🔍 Testing: PHP with Environment${NC}"
    echo -e "${YELLOW}⚠️  SKIP: PHP not available${NC}"
fi

run_test "Python with Environment" "REQUEST_METHOD='POST' QUERY_STRING='test=env' CONTENT_LENGTH='11' SCRIPT_FILENAME='tmp/www/test.py' python3 tmp/www/test.py <<< 'hello world'"

# Test error conditions
echo -e "\n${YELLOW}⚠️  Error Condition Tests${NC}"

if [ "$PHP_AVAILABLE" = true ]; then
    run_test "Non-existent PHP file" "! php nonexistent.php 2>/dev/null"
else
    echo -e "\n${BLUE}🔍 Testing: Non-existent PHP file${NC}"
    echo -e "${YELLOW}⚠️  SKIP: PHP not available${NC}"
fi

run_test "Non-existent Python file" "! python3 nonexistent.py 2>/dev/null"

# Performance test
echo -e "\n${YELLOW}⚡ Performance Tests${NC}"

echo -e "\n${BLUE}🔍 Testing: CGI Performance${NC}"

if [ "$PHP_AVAILABLE" = true ]; then
    echo "Running 10 PHP CGI calls..."
    start_time=$(date +%s.%N)
    for i in {1..10}; do
        QUERY_STRING="test=$i" php tmp/www/test.php > /dev/null 2>&1
    done
    end_time=$(date +%s.%N)
    duration=$(echo "$end_time - $start_time" | bc)
    echo "PHP total time: ${duration}s"
    echo "PHP average per call: $(echo "scale=4; $duration / 10" | bc)s"
else
    echo "PHP not available - skipping PHP performance test"
fi

echo "Running 10 Python CGI calls..."
start_time=$(date +%s.%N)
for i in {1..10}; do
    QUERY_STRING="test=$i" python3 tmp/www/test.py > /dev/null 2>&1
done
end_time=$(date +%s.%N)
duration=$(echo "$end_time - $start_time" | bc)
echo "Python total time: ${duration}s"
echo "Python average per call: $(echo "scale=4; $duration / 10" | bc)s"

# Cleanup
echo -e "\n${YELLOW}🧹 Cleanup${NC}"
rm -f test_cgi_standalone

echo -e "\n${GREEN}🎉 CGI Testing Complete!${NC}"
echo -e "${BLUE}📊 Summary: Your CGI system can run independently of the HTTP server${NC}"