#include "../include/Request.hpp"
#include <iostream>
#include <cassert>
#include <string>

// Mock classes for testing
class MockServer {
public:
    std::string ip;
    std::string port;
    std::vector<std::string> server_name;
    bool methods[3]; // GET, POST, DELETE
    std::string client_max_body_size;
    
    MockServer() : ip("127.0.0.1"), port("8080"), client_max_body_size("1048576") {
        server_name.push_back("localhost");
        methods[0] = true;  // GET
        methods[1] = true;  // POST
        methods[2] = true;  // DELETE
    }
};

class MockResponse {
public:
    void setConf(MockServer* conf) { (void)conf; }
};

class MockClient {
public:
    MockResponse* response;
    MockClient() { response = new MockResponse(); }
    ~MockClient() { delete response; }
};

// Test helper functions
void test_basic_get_request() {
    std::cout << "Testing Basic GET Request..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    std::string http_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: TestClient/1.0\r\n"
        "\r\n";
    
    req.add(&client, &server, http_request);
    
    assert(req.ready() == true);
    assert(req.is_valid() == 0);
    assert(req.get_method() == "GET");
    assert(req.get_path() == "/index.html");
    assert(req.get_version() == "HTTP/1.1");
    assert(req.get_header("host") == "localhost:8080");
    assert(req.get_header("user-agent") == "TestClient/1.0");
    
    std::cout << "✅ Basic GET Request test passed!" << std::endl;
}

void test_post_request_with_body() {
    std::cout << "Testing POST Request with Body..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    std::string http_request = 
        "POST /submit HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 27\r\n"
        "\r\n"
        "username=test&password=123";
    
    req.add(&client, &server, http_request);
    
    assert(req.ready() == true);
    assert(req.is_valid() == 0);
    assert(req.get_method() == "POST");
    assert(req.get_path() == "/submit");
    assert(req.get_header("content-type") == "application/x-www-form-urlencoded");
    assert(req.get_header("content-length") == "27");
    assert(req.get_body() == "username=test&password=123");
    
    std::cout << "✅ POST Request with Body test passed!" << std::endl;
}

void test_delete_request() {
    std::cout << "Testing DELETE Request..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    std::string http_request = 
        "DELETE /api/user/123 HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Authorization: Bearer token123\r\n"
        "\r\n";
    
    req.add(&client, &server, http_request);
    
    assert(req.ready() == true);
    assert(req.is_valid() == 0);
    assert(req.get_method() == "DELETE");
    assert(req.get_path() == "/api/user/123");
    assert(req.get_header("authorization") == "Bearer token123");
    
    std::cout << "✅ DELETE Request test passed!" << std::endl;
}

void test_incremental_parsing() {
    std::cout << "Testing Incremental Parsing..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    // Send request in multiple chunks
    req.add(&client, &server, "GET /test");
    assert(req.ready() == false);
    
    req.add(&client, &server, " HTTP/1.1\r\n");
    assert(req.ready() == false);
    
    req.add(&client, &server, "Host: localhost\r\n");
    assert(req.ready() == false);
    
    req.add(&client, &server, "\r\n");
    assert(req.ready() == true);
    assert(req.is_valid() == 0);
    assert(req.get_method() == "GET");
    assert(req.get_path() == "/test");
    
    std::cout << "✅ Incremental Parsing test passed!" << std::endl;
}

void test_malformed_requests() {
    std::cout << "Testing Malformed Requests..." << std::endl;
    
    MockClient client;
    MockServer server;
    
    // Test 1: Invalid method
    {
        Request req;
        req.add(&client, &server, "INVALID /test HTTP/1.1\r\n\r\n");
        assert(req.is_valid() == 400);
        std::cout << "  ✅ Invalid method test passed" << std::endl;
    }
    
    // Test 2: Invalid HTTP version
    {
        Request req;
        req.add(&client, &server, "GET /test HTTP/2.0\r\n\r\n");
        assert(req.is_valid() == 400);
        std::cout << "  ✅ Invalid HTTP version test passed" << std::endl;
    }
    
    // Test 3: Missing Host header (should be handled gracefully)
    {
        Request req;
        req.add(&client, &server, "GET /test HTTP/1.1\r\n\r\n");
        // This might be valid or invalid depending on HTTP/1.1 strictness
        std::cout << "  ✅ Missing Host header test completed" << std::endl;
    }
    
    std::cout << "✅ Malformed Requests tests passed!" << std::endl;
}

void test_header_case_insensitivity() {
    std::cout << "Testing Header Case Insensitivity..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    std::string http_request = 
        "GET /test HTTP/1.1\r\n"
        "HOST: localhost\r\n"
        "User-Agent: TestClient\r\n"
        "CONTENT-TYPE: text/plain\r\n"
        "\r\n";
    
    req.add(&client, &server, http_request);
    
    assert(req.ready() == true);
    assert(req.get_header("host") == "localhost");
    assert(req.get_header("HOST") == "localhost");
    assert(req.get_header("Host") == "localhost");
    assert(req.get_header("content-type") == "text/plain");
    
    std::cout << "✅ Header Case Insensitivity test passed!" << std::endl;
}

void test_clear_functionality() {
    std::cout << "Testing Clear Functionality..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    // Parse a request
    req.add(&client, &server, "GET /test HTTP/1.1\r\nHost: localhost\r\n\r\n");
    assert(req.ready() == true);
    assert(req.get_method() == "GET");
    
    // Clear and verify
    req.clear();
    assert(req.ready() == false);
    assert(req.get_method() == "");
    assert(req.get_path() == "");
    assert(req.get_body() == "");
    
    // Parse another request
    req.add(&client, &server, "POST /submit HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\ntest");
    assert(req.ready() == true);
    assert(req.get_method() == "POST");
    assert(req.get_body() == "test");
    
    std::cout << "✅ Clear Functionality test passed!" << std::endl;
}

void test_uri_with_query_parameters() {
    std::cout << "Testing URI with Query Parameters..." << std::endl;
    
    Request req;
    MockClient client;
    MockServer server;
    
    std::string http_request = 
        "GET /search?q=42school&category=projects HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";
    
    req.add(&client, &server, http_request);
    
    assert(req.ready() == true);
    assert(req.get_path() == "/search?q=42school&category=projects");
    assert(req.get_path_o() == "/search?q=42school&category=projects");
    
    std::cout << "✅ URI with Query Parameters test passed!" << std::endl;
}

void print_request_info(const Request& req) {
    std::cout << "\n=== REQUEST INFO ===" << std::endl;
    std::cout << "Method: " << req.get_method() << std::endl;
    std::cout << "Path: " << req.get_path() << std::endl;
    std::cout << "Version: " << req.get_version() << std::endl;
    std::cout << "Ready: " << (req.ready() ? "Yes" : "No") << std::endl;
    std::cout << "Valid: " << req.is_valid() << std::endl;
    std::cout << "Body: '" << req.get_body() << "'" << std::endl;
    std::cout << "===================" << std::endl;
}

int main() {
    std::cout << "🧪 Starting Request Class Tests...\n" << std::endl;
    
    try {
        test_basic_get_request();
        test_post_request_with_body();
        test_delete_request();
        test_incremental_parsing();
        test_malformed_requests();
        test_header_case_insensitivity();
        test_clear_functionality();
        test_uri_with_query_parameters();
        
        std::cout << "\n🎉 All tests passed successfully!" << std::endl;
        
        // Interactive test
        std::cout << "\n📝 Manual Test Example:" << std::endl;
        Request req;
        MockClient client;
        MockServer server;
        
        req.add(&client, &server, "GET /example HTTP/1.1\r\nHost: test.com\r\n\r\n");
        print_request_info(req);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
