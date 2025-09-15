#include "include/CGIExecutor.hpp"
#include <iostream>
#include <fstream>

void createTestScript() {
    // Create a simple PHP test script
    std::ofstream php("test_script.php");
    php << "<?php\n";
    php << "echo \"Content-Type: text/html\\r\\n\\r\\n\";\n";
    php << "echo \"<h1>CGI Test Successful!</h1>\\n\";\n";
    php << "echo \"<p>Method: \" . $_SERVER['REQUEST_METHOD'] . \"</p>\\n\";\n";
    php << "echo \"<p>Query: \" . $_SERVER['QUERY_STRING'] . \"</p>\\n\";\n";
    php << "if ($_SERVER['REQUEST_METHOD'] === 'POST') {\n";
    php << "    $input = file_get_contents('php://stdin');\n";
    php << "    echo \"<p>POST Data: \" . htmlspecialchars($input) . \"</p>\\n\";\n";
    php << "}\n";
    php << "?>\n";
    php.close();
}

int main() {
    std::cout << "=== Independent CGI Handler Test ===" << std::endl;
    
    // Create test script
    createTestScript();
    
    CGIExecutor executor;
    
    // Test 1: GET request
    std::cout << "\n--- Test 1: GET Request ---" << std::endl;
    CGIRequest request1;
    request1.method = "GET";
    request1.scriptPath = "./test_script.php";
    request1.queryString = "name=test&value=123";
    request1.headers["Content-Type"] = "text/html";
    
    CGIResponse response1 = executor.execute(request1);
    
    if (response1.success) {
        std::cout << "SUCCESS!" << std::endl;
        std::cout << "Headers: " << response1.headers << std::endl;
        std::cout << "Body: " << response1.body << std::endl;
    } else {
        std::cout << "FAILED: " << response1.errorMessage << std::endl;
    }
    
    // Test 2: POST request
    std::cout << "\n--- Test 2: POST Request ---" << std::endl;
    CGIRequest request2;
    request2.method = "POST";
    request2.scriptPath = "./test_script.php";
    request2.body = "username=john&password=secret";
    request2.headers["Content-Type"] = "application/x-www-form-urlencoded";
    
    CGIResponse response2 = executor.execute(request2);
    
    if (response2.success) {
        std::cout << "SUCCESS!" << std::endl;
        std::cout << "Headers: " << response2.headers << std::endl;
        std::cout << "Body: " << response2.body << std::endl;
    } else {
        std::cout << "FAILED: " << response2.errorMessage << std::endl;
    }
    
    // Test 3: File extension detection
    std::cout << "\n--- Test 3: File Detection ---" << std::endl;
    std::cout << "test.php is CGI: " << (CGIExecutor::isCGIFile("test.php") ? "YES" : "NO") << std::endl;
    std::cout << "test.html is CGI: " << (CGIExecutor::isCGIFile("test.html") ? "YES" : "NO") << std::endl;
    std::cout << "script.py is CGI: " << (CGIExecutor::isCGIFile("script.py") ? "YES" : "NO") << std::endl;
    
    return 0;
}
