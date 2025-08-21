# Makefile for Request Class Tests
# Run from webserv2/tests/ directory

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror
INCLUDES = -I../include
SRCDIR = ../src
OBJDIR = ../obj

# Ensure obj directory exists
$(shell mkdir -p $(OBJDIR))

# Test executables
SIMPLE_TEST = test_simple
INTEGRATION_TEST = test_request

# Source files
REQUEST_SRC = $(SRCDIR)/Request.cpp
REQUEST_OBJ = $(OBJDIR)/Request.o

.PHONY: all test clean simple integration help

# Default target
all: simple

# Help target
help:
	@echo "Available targets:"
	@echo "  simple      - Build and run basic unit tests"
	@echo "  integration - Build integration tests (requires Server/Client classes)"
	@echo "  test        - Run all available tests"
	@echo "  clean       - Clean build files"
	@echo "  help        - Show this help"

# Compile Request.cpp
$(REQUEST_OBJ): $(REQUEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build simple test
$(SIMPLE_TEST): test_simple.cpp $(REQUEST_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

# Build integration test (will fail without Server/Client classes)
$(INTEGRATION_TEST): test_request.cpp $(REQUEST_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

# Run simple tests
simple: $(SIMPLE_TEST)
	@echo "🧪 Running Simple Unit Tests..."
	@./$(SIMPLE_TEST)

# Try to build integration tests
integration: $(INTEGRATION_TEST)
	@echo "🧪 Running Integration Tests..."
	@./$(INTEGRATION_TEST)

# Run all tests
test: simple
	@echo "✅ All available tests completed!"

# Run the test script
script:
	@echo "🧪 Running Test Script..."
	@chmod +x test_script.sh
	@./test_script.sh

# Clean up
clean:
	rm -f $(SIMPLE_TEST) $(INTEGRATION_TEST)
	rm -f $(OBJDIR)/*.o

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: simple
	@echo "🐛 Debug build completed"
