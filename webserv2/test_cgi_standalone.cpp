#include "include/CGIExecutor.hpp"
#include <iostream>
#include <cassert>

void testPHPCGI() {
    std::cout << "\n=== Testing PHP CGI ===" << std::endl;
    
    // Check if PHP is available first
    std::string phpInterpreter = CGIExecutor::getInterpreter("test.php");
    if (phpInterpreter.empty()) {
        std::cout << "⚠️  PHP interpreter not found - skipping PHP tests" << std::endl;
        return;
    }
    
    CGIExecutor executor;
    CGIRequest request;
    
    // Setup PHP test
    request.method = "GET";
    request.scriptPath = "tmp/www/test.php";
    request.queryString = "name=TestUser&id=123";
    request.body = "";
    request.headers["Content-Type"] = "text/html";
    
    // Execute
    CGIResponse response = executor.execute(request);
    
    // Check results
    std::cout << "Success: " << (response.success ? "YES" : "NO") << std::endl;
    std::cout << "Exit Code: " << response.exitCode << std::endl;
    if (!response.success) {
        std::cout << "Error: " << response.errorMessage << std::endl;
    }
    std::cout << "Headers: " << response.headers << std::endl;
    std::cout << "Body: " << response.body << std::endl;
    
    assert(response.success && "PHP CGI should execute successfully");
}

void testPythonCGI() {
    std::cout << "\n=== Testing Python CGI ===" << std::endl;
    
    CGIExecutor executor;
    CGIRequest request;
    
    // Setup Python test
    request.method = "POST";
    request.scriptPath = "tmp/www/test.py";
    request.queryString = "param1=value1&param2=value2";
    request.body = "POST data goes here";
    request.headers["Content-Type"] = "application/x-www-form-urlencoded";
    
    // Execute
    CGIResponse response = executor.execute(request);
    
    // Check results
    std::cout << "Success: " << (response.success ? "YES" : "NO") << std::endl;
    std::cout << "Exit Code: " << response.exitCode << std::endl;
    if (!response.success) {
        std::cout << "Error: " << response.errorMessage << std::endl;
    }
    std::cout << "Headers: " << response.headers << std::endl;
    std::cout << "Body: " << response.body << std::endl;
    
    assert(response.success && "Python CGI should execute successfully");
}

void testCGIFileDetection() {
    std::cout << "\n=== Testing CGI File Detection ===" << std::endl;
    
    // Test valid CGI files
    assert(CGIExecutor::isCGIFile("test.php") && "Should detect .php files");
    assert(CGIExecutor::isCGIFile("script.py") && "Should detect .py files");
    assert(CGIExecutor::isCGIFile("program.pl") && "Should detect .pl files");
    
    // Test invalid files
    assert(!CGIExecutor::isCGIFile("test.html") && "Should not detect .html files");
    assert(!CGIExecutor::isCGIFile("script.txt") && "Should not detect .txt files");
    assert(!CGIExecutor::isCGIFile("noextension") && "Should not detect files without extension");
    
    std::cout << "✅ CGI file detection works correctly" << std::endl;
}

void testInterpreterDetection() {
    std::cout << "\n=== Testing Interpreter Detection ===" << std::endl;
    
    std::string phpInterpreter = CGIExecutor::getInterpreter("test.php");
    std::string pythonInterpreter = CGIExecutor::getInterpreter("test.py");
    std::string perlInterpreter = CGIExecutor::getInterpreter("test.pl");
    
    std::cout << "PHP interpreter: " << (phpInterpreter.empty() ? "NOT FOUND" : phpInterpreter) << std::endl;
    std::cout << "Python interpreter: " << pythonInterpreter << std::endl;
    std::cout << "Perl interpreter: " << (perlInterpreter.empty() ? "NOT FOUND" : perlInterpreter) << std::endl;
    
    // Only assert on Python since it's definitely available
    assert(!pythonInterpreter.empty() && "Should find Python interpreter");
    
    if (phpInterpreter.empty()) {
        std::cout << "⚠️  PHP not found - this is OK for testing" << std::endl;
    }
    if (perlInterpreter.empty()) {
        std::cout << "⚠️  Perl not found - this is OK for testing" << std::endl;
    }
}

void testErrorHandling() {
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    
    CGIExecutor executor;
    CGIRequest request;
    
    // Test non-existent file
    request.method = "GET";
    request.scriptPath = "nonexistent.php";
    request.queryString = "";
    request.body = "";
    
    CGIResponse response = executor.execute(request);
    
    std::cout << "Non-existent file - Success: " << (response.success ? "YES" : "NO") << std::endl;
    std::cout << "Error message: " << response.errorMessage << std::endl;
    
    assert(!response.success && "Should fail for non-existent files");
}

int main() {
    std::cout << "🚀 Starting Standalone CGI Tests" << std::endl;
    
    try {
        testCGIFileDetection();
        testInterpreterDetection();
        testErrorHandling();
        testPHPCGI();
        testPythonCGI();
        
        std::cout << "\n🎉 All CGI tests passed!" << std::endl;
        std::cout << "✅ Your CGI system is working perfectly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}