#include "../include/Request.hpp"
#include <iostream>
#include <cassert>
#include <string>

void test_chunked_compilation() {
    std::cout << "\n🧪 Testing Request Class with Chunked Support...\n" << std::endl;
    
    Request req;
    
    // Test basic functionality
    std::cout << "Testing basic Request functionality..." << std::endl;
    
    // Test constructor and basic methods
    assert(req.get_method_enum() == UNKNOWN);
    assert(req.get_path() == "");
    assert(req.get_body() == "");
    assert(req.is_valid() == 0);
    assert(req.ready() == false);
    assert(req.body_complete() == false);
    std::cout << "✅ Constructor and getters work!" << std::endl;
    
    // Test path manipulation
    req.set_path("/test/chunked");
    assert(req.get_path() == "/test/chunked");
    std::cout << "✅ Path setter/getter works!" << std::endl;
    
    // Test clear functionality
    req.clear();
    assert(req.get_path() == "");
    assert(req.get_method_enum() == UNKNOWN);
    std::cout << "✅ Clear functionality works!" << std::endl;
    
    // Test error code handling
    assert(req.get_error_code() == REQ_OK);
    std::cout << "✅ Error code handling works!" << std::endl;
    
    std::cout << "\n🎉 Request class with chunked encoding compiles and runs!" << std::endl;
    std::cout << "\n📋 What was tested:" << std::endl;
    std::cout << "  - Request class compilation with chunked support" << std::endl;
    std::cout << "  - Basic public method functionality" << std::endl;
    std::cout << "  - Constructor/destructor" << std::endl;
    std::cout << "  - Path manipulation" << std::endl;
    std::cout << "  - State management" << std::endl;
    std::cout << "\n✅ Ready for integration with your webserver!" << std::endl;
}

int main() {
    try {
        test_chunked_compilation();
        std::cout << "\n✅ All compilation tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
