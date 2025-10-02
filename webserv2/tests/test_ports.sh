#!/bin/bash

# 42 Webserv Evaluation Tests - Port and Multi-Server
echo "🔌 Port and Multi-Server Tests"
echo "=============================="

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}📋 Port Tests Instructions:${NC}"
echo ""
echo "1. Test multiple ports in configuration:"
echo "   - Setup servers on different ports (80, 8080, 8081)"
echo "   - Each should serve different content"
echo "   - Test: curl http://localhost:PORT/"
echo ""
echo "2. Test same port multiple times:"
echo "   - Add same port twice in config"
echo "   - Server should reject this or handle gracefully"
echo ""
echo "3. Test multiple servers with common ports:"
echo "   - Start two server instances"
echo "   - Second should fail to bind to same port"
echo ""

# Test different ports
PORTS=(80 8080 8081)
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

echo -e "\n📋 Test 1: Different ports accessibility"
for port in "${PORTS[@]}"; do
    run_test "Port $port accessibility" \
        "curl -s --connect-timeout 3 http://localhost:$port/ -w '%{http_code}'" \
        "200\|302\|404"
done

echo -e "\n📋 Test 2: Different hostnames"
run_test "Hostname resolution" \
    "curl -s --connect-timeout 3 --resolve example.com:80:127.0.0.1 http://example.com/ -w '%{http_code}'" \
    "200\|302\|404"

echo -e "\n📋 Test 3: Port binding test"
echo -e "${BLUE}Manual test:${NC}"
echo "1. Start your server normally"
echo "2. Try to start another instance with same config"
echo "3. Second instance should fail with 'Address already in use'"

echo -e "\n📋 Configuration File Examples:"
echo ""
echo -e "${BLUE}Multiple ports config:${NC}"
cat << 'EOF'
server {
    listen 80;
    server_name localhost;
    root /tmp/www;
}

server {
    listen 8080;
    server_name localhost;
    root /var/www;
}

server {
    listen 8081;
    server_name example.com;
    root /opt/www;
}
EOF

echo ""
echo -e "${BLUE}Same port multiple times (should fail):${NC}"
cat << 'EOF'
server {
    listen 80;
    server_name site1.com;
}

server {
    listen 80;  # This should cause an error
    server_name site2.com;
}
EOF

echo -e "\n📊 Port Test Results"
echo "==================="
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$((TOTAL - PASSED))${NC}"
echo -e "Total:  $TOTAL"

echo -e "\n${BLUE}📋 Manual Tests to Perform:${NC}"
echo "1. Modify configuration to use multiple ports"
echo "2. Test each port with browser/curl"
echo "3. Try starting two server instances (should fail)"
echo "4. Test hostname resolution with --resolve flag"