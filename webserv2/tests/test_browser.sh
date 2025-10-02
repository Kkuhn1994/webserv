#!/bin/bash

# 42 Webserv Evaluation Tests - Browser Compatibility
echo "🌐 Browser Compatibility Tests"
echo "============================="

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
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

echo -e "${BLUE}📋 Browser Compatibility Test Instructions:${NC}"
echo "1. Open your browser (Chrome, Firefox, Safari)"
echo "2. Test these URLs manually:"
echo "   - http://localhost:80/ (should redirect)"
echo "   - http://localhost:80/api/ (should show content)"
echo "   - http://localhost:80/nonexistent.html (should show 404 page)"
echo "   - http://localhost:80/api/test.php (should show CGI output)"
echo ""
echo "3. Check browser developer tools (F12 -> Network tab) for:"
echo "   - Correct HTTP status codes"
echo "   - Proper response headers"
echo "   - Content-Type: text/html"
echo ""

echo -e "\n📋 Test 1: Static website serving"
run_test "Static file serving" \
    "curl -s -H 'User-Agent: Mozilla/5.0' http://localhost:80/api/index.html" \
    "html\|200"

echo -e "\n📋 Test 2: Wrong URL (404 error)"
run_test "404 error page" \
    "curl -s -H 'User-Agent: Mozilla/5.0' http://localhost:80/wrong-url.html -w '%{http_code}'" \
    "404"

echo -e "\n📋 Test 3: Directory listing"
run_test "Directory listing" \
    "curl -s -H 'User-Agent: Mozilla/5.0' http://localhost:80/api/" \
    "index\|Directory\|200"

echo -e "\n📋 Test 4: Redirected URL"
run_test "Redirection" \
    "curl -s -H 'User-Agent: Mozilla/5.0' http://localhost:80/ -w '%{http_code}'" \
    "302"

echo -e "\n📋 Test 5: HTTP Headers check"
run_test "HTTP headers" \
    "curl -s -I http://localhost:80/api/ | head -5" \
    "HTTP/1.1\|Content-Type\|Content-Length"

echo -e "\n📋 Test 6: Multiple requests (browser-like)"
run_test "Multiple requests" \
    "curl -s http://localhost:80/api/ && curl -s http://localhost:80/api/test.php" \
    "html\|php\|test"

echo -e "\n${BLUE}📋 Manual Browser Tests (Open these in your browser):${NC}"
echo "✓ http://localhost:80/ - Should redirect automatically"
echo "✓ http://localhost:80/api/ - Should show directory or index page"
echo "✓ http://localhost:80/nonexistent - Should show custom 404 error page"
echo "✓ http://localhost:80/api/test.php - Should show CGI output"
echo "✓ Developer tools should show proper HTTP status codes"

echo -e "\n📊 Browser Compatibility Test Results"
echo "===================================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"