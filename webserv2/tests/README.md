# Request Class Tests

This directory contains all testing files for the Request class implementation.

## 📁 Directory Structure

```
tests/
├── Makefile              # Build system for tests
├── test_simple.cpp       # Basic unit tests (works standalone)
├── test_request.cpp      # Integration tests (needs Server/Client)
├── test_script.sh        # Automated test verification script
├── TESTING_GUIDE.md      # Comprehensive testing documentation
├── Makefile.test         # Alternative makefile (legacy)
└── README.md             # This file
```

## 🚀 Quick Start

### Run Basic Tests
```bash
cd tests/
make simple
```

### Run All Available Tests
```bash
cd tests/
make test
```

### Run Verification Script
```bash
cd tests/
./test_script.sh
```

## 📋 Available Make Targets

- `make simple` - Build and run basic unit tests
- `make integration` - Build integration tests (requires Server/Client classes)
- `make test` - Run all available tests
- `make script` - Run the verification script
- `make clean` - Clean build files
- `make debug` - Build with debug symbols
- `make help` - Show available targets

## 🧪 Test Types

### 1. Unit Tests (`test_simple.cpp`)
- ✅ **Works Now** - No dependencies on Server/Client
- Tests basic functionality:
  - Constructor/destructor
  - Getters/setters
  - Clear functionality
  - Error code handling
  - Method enums

### 2. Integration Tests (`test_request.cpp`)
- ⏳ **Needs Integration** - Requires Server/Client classes
- Tests full HTTP parsing:
  - GET/POST/DELETE requests
  - Header parsing
  - Body handling
  - Incremental parsing
  - Error scenarios

### 3. Verification Script (`test_script.sh`)
- ✅ **Works Now** - Automated verification
- Checks:
  - Compilation success
  - File structure
  - Function declarations
  - Implementation completeness

## 📖 Testing Documentation

See `TESTING_GUIDE.md` for comprehensive testing instructions including:
- Manual HTTP testing with curl/telnet
- Performance testing
- Error testing
- Debugging tips
- Real-world scenarios

## 🔧 Integration with Main Project

To use these tests with your main webserver:

1. **Implement Server/Client classes** in your main project
2. **Update integration tests** with correct class interfaces
3. **Run integration tests** to verify full HTTP parsing
4. **Use curl/telnet tests** from the guide with your running server

## 📊 Current Test Status

✅ **Unit Tests**: Passing  
⏳ **Integration Tests**: Waiting for Server/Client implementation  
✅ **Code Quality**: All checks pass  
✅ **Compilation**: Clean build  

## 🐛 Debugging

For debugging test failures:

```bash
# Build with debug info
make debug

# Run with verbose output
make simple

# Check compilation errors
make 2>&1 | head -20
```

## 📞 Usage Examples

```bash
# Basic workflow
cd tests/
make simple                 # Run unit tests
make script                 # Run verification
cat TESTING_GUIDE.md       # Read full guide

# When Server/Client are ready
make integration            # Full HTTP tests
```

This testing setup ensures your Request class is thoroughly validated before integration into the main webserver.
