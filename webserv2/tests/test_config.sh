#!/bin/bash

# 42 Webserv Evaluation Tests - Configuration
echo "🔧 Configuration Tests"
echo "======================"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PASSED=0
TOTAL=0

run_test() {
    local test_name="$1"
    local command="$2"
    local expected_pattern="$3"
    
    echo -e "\n${YELLOW}🔍 Testing: $test_name${NC}"
    echo "Command: $command"
    
    TOTAL=$((TOTAL + 1))
    
    output=$(eval "$command" 2>&1)
    
    if echo "$output" | grep -q "$expected_pattern"; then
        echo -e "${GREEN}✅ PASS${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "Output: $output"
    fi
}

echo "📋 Test 1: Multiple servers with different ports"
echo "Start server with configuration.conf (port 8080)"
echo "Expected: Server should start without errors"

echo -e "\n📋 Test 2: Multiple servers with different hostnames"
run_test "Different hostname test" \
    "curl -s --connect-timeout 3 --resolve example.com:80:127.0.0.1 http://example.com/ -w '%{http_code}'" \
    "200\|302\|404"

echo -e "\n📋 Test 3: Default error page (404)"
run_test "Custom 404 error page" \
    "curl -s http://localhost:80/nonexistent-file.html" \
    "404\|Not Found"

echo -e "\n📋 Test 4: Client body limit"
run_test "Body size limit test" \
    "curl -s -X POST -H 'Content-Type: text/plain' --data 'THIS_IS_A_VERY_LONG_BODY_THAT_EXCEEDS_THE_LIMIT_AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' http://localhost:80/api/ -w '%{http_code}'" \
    "413\|400\|500"

echo -e "\n📋 Test 5: Route to different directories"
run_test "Route to /api directory" \
    "curl -s http://localhost:80/api/ -w '%{http_code}'" \
    "200\|302"

echo -e "\n📋 Test 6: Default file in directory"
run_test "Directory default file" \
    "curl -s http://localhost:80/api/" \
    "index\|html\|200"

echo -e "\n📋 Test 7: Method restrictions"
run_test "DELETE method restriction" \
    "curl -s -X DELETE http://localhost:80/api/test.html -w '%{http_code}'" \
    "403\|405\|Method Not Allowed"

echo -e "\n📊 Configuration Test Results"
echo "=============================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"