#!/bin/bash

# 42 Webserv Evaluation Tests - CGI
echo "🐍 CGI Tests"
echo "============"

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
        echo "Output preview: $(echo "$output" | head -1)"
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo "Output: $output"
    fi
}

# Create test CGI scripts
echo "📝 Creating test CGI scripts..."

# Simple PHP script
cat > ../tmp/www/test_simple.php << 'EOF'
#!/opt/homebrew/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html><body>";
echo "<h1>PHP CGI Test</h1>";
echo "<p>Current time: " . date('Y-m-d H:i:s') . "</p>";
echo "<p>Method: " . $_SERVER['REQUEST_METHOD'] . "</p>";
echo "</body></html>";
?>
EOF

# PHP script with parameters
cat > ../tmp/www/test_params.php << 'EOF'
#!/opt/homebrew/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html><body>";
echo "<h1>PHP Parameters Test</h1>";
echo "<p>GET parameters:</p><ul>";
foreach($_GET as $key => $value) {
    echo "<li>$key = $value</li>";
}
echo "</ul>";
echo "<p>POST data length: " . strlen(file_get_contents('php://input')) . "</p>";
echo "</body></html>";
?>
EOF

# Error CGI script (infinite loop)
cat > ../tmp/www/test_error.php << 'EOF'
#!/opt/homebrew/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html><body><h1>Error Test</h1>";
// This will cause an error
$undefined_variable = $nonexistent_array['key'];
echo "</body></html>";
?>
EOF

# Python CGI script
cat > ../tmp/www/test.py << 'EOF'
#!/usr/bin/env python3
print("Content-Type: text/html\r\n")
print("<html><body>")
print("<h1>Python CGI Test</h1>")
print("<p>This is a Python CGI script</p>")
print("</body></html>")
EOF

chmod +x ../tmp/www/*.php ../tmp/www/*.py

echo -e "\n📋 Test 1: Basic PHP CGI with GET"
run_test "PHP CGI GET" \
    "curl -s http://localhost:80/api/test_simple.php" \
    "PHP CGI Test\|Current time"

echo -e "\n📋 Test 2: PHP CGI with GET parameters"
run_test "PHP CGI with parameters" \
    "curl -s 'http://localhost:80/api/test_params.php?name=test&value=123'" \
    "Parameters Test\|name = test"

echo -e "\n📋 Test 3: PHP CGI with POST"
run_test "PHP CGI POST" \
    "curl -s -X POST -d 'postdata=hello' http://localhost:80/api/test_params.php" \
    "Parameters Test\|POST data length"

echo -e "\n📋 Test 4: Python CGI"
run_test "Python CGI" \
    "curl -s http://localhost:80/api/test.py" \
    "Python CGI Test"

echo -e "\n📋 Test 5: CGI Error Handling"
run_test "CGI error handling" \
    "curl -s http://localhost:80/api/test_error.php -w '%{http_code}'" \
    "500\|Error"

echo -e "\n📋 Test 6: Non-existent CGI file"
run_test "Non-existent CGI" \
    "curl -s http://localhost:80/api/nonexistent.php -w '%{http_code}'" \
    "404"

echo -e "\n📊 CGI Test Results"
echo "=================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"