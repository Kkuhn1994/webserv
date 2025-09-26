#!/bin/bash

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

echo -e "\n🚀 Starting Redirection Tests..."

# Test 1: Basic PHP CGI
run_test "Basic Site" \
    'curl -s -H "Host: example.com" http://localhost:80/api' \
    "index3.html"

# Test 1: Basic PHP CGI
run_test "Basic Site" \
    'curl -s -H "Host: example.com" http://localhost:80/' \
    "index3.html"