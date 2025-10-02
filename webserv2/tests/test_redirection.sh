#!/bin/bash

##nginx_redirection_test

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
    
    if  echo "$output" | grep "$expected_pattern"; then
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

ITERATIONS=10
START_TIME=$(date +%s%N)


for (( i = 1; i <= ITERATIONS; i++ )); do
    # Test 1: Basic Site
    run_test "Basic Site" \
        'curl -s -H "Host: example.com" http://localhost:80/api' \
        "index3.html"

    # Test 2: One Redirection
    run_test "One Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/ -i' \
        "HTTP/1.1 302 Found"

    # Test 2.1: One Redirection
    run_test "One Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/ -i' \
        "Location: http://localhost/api"

    # Test 2.2: One Redirection
    run_test "One Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/ -L' \
        "index3.html"

    # Test 3: Two Redirection
    run_test "Two Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/ -i' \
        "HTTP/1.1 302 Found"

    # Test 3.1: Two Redirection
    run_test "Two Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/ -i' \
        "Location: http://localhost/"

    # Test 3.2: Two Redirection
    run_test "Two Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/doubleRedirect -L --max-redirs 1 -i' \
        "HTTP/1.1 302 Found"

    # Test 3.3: Two Redirection
    run_test "Two Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/doubleRedirect -L --max-redirs 1 -i' \
        "Location: http://localhost/"

    # Test 3.4: Two Redirection
    run_test "Two Redirection" \
        'curl -s -H "Host: example.com" http://localhost:80/doubleRedirect -L' \
        "index3.html"

    # Test 4: 1 Redirection + CGI
    run_test "Redirection and CGI" \
        'curl -s -H "Host: example.com" http://localhost:80/redirectAndCGI/test.php -L' \
        "Hello from WebServ CGI"

    # Test 4: 1 Redirection + 404 Not Found
    run_test "Redirection + 404 Not Found" \
        'curl -s -H "Host: example.com" http://localhost:80/redirectAndCGI/unavaillable.php -L -i' \
        "HTTP/1.1 404 Not Found"

    # Test 4: 1 Redirection + 404 Not Found
    run_test "404 Not Found Default" \
        'curl -s -H "Host: example.com" http://localhost:80/api/unavaillable.php -i' \
        "404 Not Found Default"

    # Test 4: 1 Redirection + 404 Not Found
    run_test "403 Not Found Default" \
        'curl -s -H "Host: example.com" http://localhost:80/api/permissionDenied.html -i' \
        "403 Forbidden Default"
done

END_TIME=$(date +%s%N)
RUNTIME=$((END_TIME - START_TIME))


echo -e "\n📊 Test Results Summary"
echo "======================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"

if [ $PASSED -eq $TOTAL ]; then
    echo -e "\n🎉 ${GREEN}All tests passed! Your CGI system is working perfectly!${NC}"
else
    echo -e "\n⚠️  ${YELLOW}Some tests failed. Check the output above for details.${NC}"

echo -e "⏱️ Total Runtime: ${RUNTIME} ns"
fi


