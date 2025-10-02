#!/bin/bash

# Webserv Evaluation Tests - Root Launcher
echo "🚀 42 Webserv Evaluation Tests"
echo "=============================="

# Colors
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${BLUE}📁 Running tests from tests/ directory${NC}"
echo ""

# Change to tests directory and run the master test runner
cd tests/

# Check if test scripts exist
if [ ! -f "run_evaluation_tests.sh" ]; then
    echo "❌ Test scripts not found in tests/ directory"
    exit 1
fi

echo -e "${CYAN}🧪 Launching evaluation test suite...${NC}"
echo ""

# Run the master test suite
./run_evaluation_tests.sh

# Return to root directory
cd ..

echo ""
echo -e "${BLUE}📋 Note: All tests run from tests/ directory${NC}"
echo "Individual tests can be run with: cd tests/ && ./test_name.sh"