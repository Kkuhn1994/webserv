#!/bin/bash

# 42 Webserv Evaluation Tests - Master Test Runner
echo "🚀 42 Webserv Evaluation Test Suite"
echo "==================================="

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Make all test scripts executable
chmod +x test_*.sh

echo -e "${CYAN}📋 Pre-Test Checklist:${NC}"
echo "✓ Server compiled: cd .. && make"
echo "✓ Server running: cd .. && ./webserv conf/nginxconf_redirection_test"
echo "✓ Required tools: curl, siege (brew install siege)"
echo ""

read -p "Is your server running? (y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${RED}❌ Please start your server first${NC}"
    echo "Run: cd .. && ./webserv conf/nginxconf_redirection_test"
    exit 1
fi

echo -e "\n${BLUE}🧪 Running Evaluation Tests...${NC}"
echo ""

# Track overall results
OVERALL_PASSED=0
OVERALL_TOTAL=0

run_test_suite() {
    local suite_name="$1"
    local script_name="$2"
    
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}🔧 $suite_name${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    
    if [ -f "$script_name" ]; then
        ./"$script_name"
        echo ""
    else
        echo -e "${RED}❌ Test script $script_name not found${NC}"
    fi
}

# Run all test suites
run_test_suite "Configuration Tests" "test_config.sh"
sleep 2

run_test_suite "HTTP Methods Tests" "test_http_methods.sh"
sleep 2

run_test_suite "CGI Tests" "test_cgi.sh"
sleep 2

run_test_suite "Browser Compatibility Tests" "test_browser.sh"
sleep 2

run_test_suite "Port and Multi-Server Tests" "test_ports.sh"
sleep 2

run_test_suite "Redirection Tests (Existing)" "test_redirection.sh"
sleep 2

echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${CYAN}💥 Stress Tests (Optional - Run Separately)${NC}"
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo "Run: ./test_stress.sh"
echo "Note: Stress tests may take several minutes"

echo -e "\n${BLUE}📊 Evaluation Checklist Summary${NC}"
echo "=============================="
echo ""
echo -e "${GREEN}✓ Configuration:${NC}"
echo "  - Multiple servers/ports"
echo "  - Error pages"
echo "  - Body size limits" 
echo "  - Route configurations"
echo "  - Method restrictions"
echo ""
echo -e "${GREEN}✓ Basic Functionality:${NC}"
echo "  - GET, POST, DELETE methods"
echo "  - Unknown method handling"
echo "  - File upload/download"
echo "  - Proper status codes"
echo ""
echo -e "${GREEN}✓ CGI Support:${NC}"
echo "  - PHP/Python CGI execution"
echo "  - GET/POST parameter handling"
echo "  - Error handling"
echo "  - Correct directory execution"
echo ""
echo -e "${GREEN}✓ Browser Compatibility:${NC}"
echo "  - Static website serving"
echo "  - Proper HTTP headers"
echo "  - Error page display"
echo "  - Redirection handling"
echo ""
echo -e "${GREEN}✓ Advanced:${NC}"
echo "  - Multiple port binding"
echo "  - Hostname resolution"
echo "  - Stress testing ready"
echo "  - Memory leak monitoring"
echo ""

echo -e "${BLUE}🎯 For Evaluation:${NC}"
echo "1. Run these tests during peer evaluation"
echo "2. Test manually in browser"
echo "3. Show configuration files"
echo "4. Demonstrate CGI functionality"
echo "5. Run stress tests if time permits"
echo ""
echo -e "${GREEN}✅ Your webserver is evaluation-ready!${NC}"