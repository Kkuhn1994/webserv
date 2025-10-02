#!/bin/bash

##nginx_cgi_test

echo "🧪 Comprehensive CGI Server Testing"
echo "=================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counter
PASSED=0
TOTAL=0

# Function to run test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_pattern="$3"
    
    echo -e "\n${YELLOW}🔍 Testing: $test_name${NC}"
    echo "Command: $command"
    
    TOTAL=$((TOTAL + 1))
    
    # Run command and capture output
    output=$(eval "$command" 2>&1)
    exit_code=$?
    
    if [ $exit_code -eq 0 ] && echo "$output" | grep -q "$expected_pattern"; then
        echo -e "${GREEN}✅ PASS${NC}"
        PASSED=$((PASSED + 1))
        echo "Output preview: $(echo "$output" | head -1)"
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "Exit code: $exit_code"
        echo "Output: $output"
    fi
}

echo -e "\n🚀 Starting CGI Tests..."

# Test 1: Basic PHP CGI
run_test "Basic PHP CGI" \
    'curl -s -H "Host: example.com" http://localhost:80/test.php' \
    "Hello from WebServ CGI"

# Test 2: PHP CGI with Query String
run_test "PHP CGI with Query Parameters" \
    'curl -s -H "Host: example.com" "http://localhost:80/parameter.php?name=TestUser&age=123"' \
    "Hello my name is TestUser and i am 123 years old!"

# Test 3: Python CGI
run_test "Python CGI" \
    'curl -s -H "Host: example.com" http://localhost:80/test.py' \
    "Python CGI Test"

# Test 4: Python CGI with Query String
run_test "Python CGI with Query Parameters" \
    'curl -s -H "Host: example.com" "http://localhost:80/test.py?param1=value1&param2=value2"' \
    "Query String"

# Test 5: POST Request to PHP
run_test "PHP CGI POST Request" \
    'curl -s -H "Host: example.com" -X POST -d "form_data=hello_world" http://localhost:80/test.php' \
    "Hello from WebServ CGI"

# Test 6: POST Request to Python
run_test "Python CGI POST Request" \
    'curl -s -H "Host: example.com" -X POST -d "test_data=post_value" http://localhost:80/test.py' \
    "POST"

# Test 7: Different HTTP Methods
run_test "HTTP GET Method" \
    'curl -s -H "Host: example.com" -X GET http://localhost:80/test.php' \
    "Request Method: GET"

# Test 8: Content-Type Header Test
run_test "Content-Type Header" \
    'curl -s -H "Host: example.com" -H "Content-Type: application/json" -X POST -d "{\"key\":\"value\"}" http://localhost:80/test.py' \
    "Python CGI Test"

# Test 9: Multiple Query Parameters
run_test "Complex Query String" \
    'curl -s -H "Host: example.com" "http://localhost:80/test.php?user=john&age=25&role=admin&active=true"' \
    "Query String"

# Test 10: Error Handling - Non-existent CGI file
run_test "Error Handling - Non-existent File" \
    'curl -s -H "Host: example.com" http://localhost:80/nonexistent.php' \
    "500\|Error\|Not Found"

# Test 11: PHP CGI with Query String
run_test "POST PHP CGI with Query Parameters" \
    'curl -s -H "Host: example.com" "http://localhost:80/parameter.php" -X POST -d "name=TestUser&age=123"' \
    "Hello my name is TestUserand i am  123 years old!"

echo -e "\n📊 Test Results Summary"
echo "======================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"

if [ $PASSED -eq $TOTAL ]; then
    echo -e "\n🎉 ${GREEN}All tests passed! Your CGI system is working perfectly!${NC}"
    exit 0
else
    echo -e "\n⚠️  ${YELLOW}Some tests failed. Check the output above for details.${NC}"
    exit 1
fi