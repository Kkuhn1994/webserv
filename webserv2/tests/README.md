# 42 Webserv Evaluation Tests

This directory contains comprehensive test suites for the 42 webserv project, covering all evaluation criteria.

## 📁 Test Files

### **Core Test Scripts:**
- `test_config.sh` - Configuration tests (ports, error pages, body limits)
- `test_http_methods.sh` - HTTP methods (GET, POST, DELETE, UNKNOWN)
- `test_cgi.sh` - CGI functionality with PHP/Python
- `test_browser.sh` - Browser compatibility tests
- `test_ports.sh` - Multiple ports and server tests
- `test_stress.sh` - Siege stress testing
- `test_redirection.sh` - Redirection functionality

### **Test Runners:**
- `run_evaluation_tests.sh` - Master test runner (run from tests/ directory)
- `../run_tests.sh` - Root launcher (run from project root)

### **Results:**
- `failed_tests.txt` - Detailed failure analysis and prioritization

## 🚀 How to Run Tests

### **Option 1: From Project Root (Recommended)**
```bash
# From webserv2/ directory
./run_tests.sh
```

### **Option 2: From Tests Directory**
```bash
# Change to tests directory
cd tests/

# Run all tests
./run_evaluation_tests.sh

# Run individual tests
./test_config.sh
./test_http_methods.sh
./test_cgi.sh
```

## 📋 Prerequisites

1. **Server Running:**
   ```bash
   # Start server first
   ./webserv conf/nginxconf_redirection_test
   ```

2. **Required Tools:**
   ```bash
   # Install siege for stress testing
   brew install siege
   ```

3. **Test Files:**
   - All test scripts automatically create necessary test files
   - CGI scripts are created in `../tmp/www/`
   - No manual setup required

## 🎯 Evaluation Coverage

These tests cover ALL 42 webserv evaluation criteria:

### **✅ Configuration Tests:**
- Multiple servers with different ports
- Different hostnames (`--resolve` flag)
- Default error pages
- Client body size limits
- Route configurations
- Method restrictions

### **✅ Basic HTTP Tests:**
- GET, POST, DELETE requests
- UNKNOWN method handling
- File upload/download
- Proper HTTP status codes

### **✅ CGI Tests:**
- PHP and Python CGI execution
- GET/POST parameter handling
- Error handling and timeouts
- Correct directory execution

### **✅ Browser Compatibility:**
- Static website serving
- HTTP headers validation
- Error page display
- Redirection handling

### **✅ Advanced Features:**
- Multiple port binding
- Hostname resolution
- Stress testing (>99.5% availability)
- Memory leak detection

## 🐛 Known Issues (from failed_tests.txt)

### **Critical Issues:**
1. HTTP status codes incorrect (404→302, 403→200)
2. Method restrictions not enforced
3. Unknown methods return 404 instead of 405

### **High Priority:**
4. Body size limits not implemented
5. CGI POST handling broken
6. Multiple ports not configured

### **Medium Priority:**
7. Stress test connection failures (89% availability)

## 📊 Test Results Format

- `✅ PASS` - Test passed
- `❌ FAIL` - Test failed (shows output)
- Status codes are checked for accuracy
- HTTP responses are validated

## 🔧 Debugging

To debug individual tests:
```bash
# Run with verbose output
curl -v http://localhost:80/test-url

# Check server logs in terminal where server is running
# Monitor network tab in browser developer tools
```

## 📝 Notes for Evaluation

- All tests designed for 42 evaluation sheet compliance
- Simple pass/fail criteria
- Covers mandatory and bonus requirements
- Ready for peer evaluation demonstration

Run `./run_evaluation_tests.sh` before every evaluation!