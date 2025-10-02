#!/bin/bash

# 42 Webserv Evaluation Tests - Basic HTTP Methods
echo "🌐 Basic HTTP Methods Tests"
echo "=========================="

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

echo -e "\n📋 Test 1: GET Request"
run_test "GET method" \
    "curl -s http://localhost:80/api/ -w '%{http_code}'" \
    "200\|302"

echo -e "\n📋 Test 2: POST Request"
run_test "POST method" \
    "curl -s -X POST -H 'Content-Type: text/plain' --data 'test data' http://localhost:80/api/ -w '%{http_code}'" \
    "200\|405\|403"

echo -e "\n📋 Test 3: DELETE Request"
run_test "DELETE method" \
    "curl -s -X DELETE http://localhost:80/api/test.txt -w '%{http_code}'" \
    "200\|204\|403\|405\|404"

echo -e "\n📋 Test 4: UNKNOWN Method (should not crash)"
run_test "UNKNOWN method" \
    "curl -s -X UNKNOWN http://localhost:80/api/ -w '%{http_code}'" \
    "400\|405\|501"

echo -e "\n📋 Test 5: File Upload Test"
echo "Creating test file for upload..."
echo "This is a test file for upload" > /tmp/test_upload.txt

run_test "File upload" \
    "curl -s -X POST -F 'file=@/tmp/test_upload.txt' http://localhost:80/api/ -w '%{http_code}'" \
    "200\|201\|405"

echo -e "\n📋 Test 6: File Download Test"
run_test "File download" \
    "curl -s http://localhost:80/api/index.html -w '%{http_code}'" \
    "200\|404"

echo -e "\n📊 HTTP Methods Test Results"
echo "============================"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"

# Cleanup
rm -f /tmp/test_upload.txt