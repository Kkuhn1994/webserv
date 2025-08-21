# HTTP Request Testing Guide for Webserv

## 🧪 How to Test Your Request Class

### 1. Basic Unit Tests ✅
```bash
# Compile and run basic tests
g++ -std=c++98 -Wall -Wextra -Werror -I. test_simple.cpp src/Request.cpp -o test_simple
./test_simple
```

### 2. Integration Testing (when you have Server/Client classes)

#### Create a minimal server test:
```cpp
// In main.cpp or test_server.cpp
#include "include/Request.hpp"
// ... your Server and Client includes

int main() {
    Request req;
    Client client;
    Server server;
    
    // Test GET request
    std::string get_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "\r\n";
    
    req.add(&client, &server, get_request);
    
    if (req.ready() && req.is_valid() == 0) {
        std::cout << "✅ GET parsing works!" << std::endl;
        std::cout << "Method: " << req.get_method() << std::endl;
        std::cout << "Path: " << req.get_path() << std::endl;
    }
}
```

### 3. Manual Testing with Real HTTP Tools

#### A) Using `telnet` (Raw HTTP):
```bash
# Start your webserver first
./webserv config.conf

# In another terminal:
telnet localhost 8080

# Then type (press Enter twice after the last line):
GET /index.html HTTP/1.1
Host: localhost:8080

# Expected: Your server should parse this correctly
```

#### B) Using `curl` (Automated):
```bash
# Basic GET request
curl -v http://localhost:8080/

# GET with query parameters  
curl -v http://localhost:8080/search?q=test&category=all

# POST with form data
curl -v -X POST -d "username=test&password=123" http://localhost:8080/login

# POST with JSON
curl -v -X POST -H "Content-Type: application/json" -d '{"name":"test"}' http://localhost:8080/api

# DELETE request
curl -v -X DELETE http://localhost:8080/api/user/123

# Large POST (test body size limits)
curl -v -X POST -d "$(head -c 1000000 /dev/zero | tr '\0' 'A')" http://localhost:8080/upload
```

#### C) Using `nc` (netcat):
```bash
# Create a test request file
cat > test_request.txt << EOF
GET /test HTTP/1.1
Host: localhost:8080
User-Agent: TestClient/1.0

EOF

# Send to your server
nc localhost 8080 < test_request.txt
```

### 4. Test Different Request Types

#### Create test files for different scenarios:

**test_get.txt:**
```http
GET /index.html HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0
Accept: text/html
Connection: close

```

**test_post.txt:**
```http
POST /submit HTTP/1.1
Host: localhost:8080
Content-Type: application/x-www-form-urlencoded
Content-Length: 27

username=test&password=123
```

**test_chunked.txt:**
```http
POST /upload HTTP/1.1
Host: localhost:8080
Transfer-Encoding: chunked

7
Mozilla
9
Developer
0

```

### 5. Error Testing

#### Test malformed requests:
```bash
# Invalid method
echo -e "INVALID /test HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080

# Invalid HTTP version
echo -e "GET /test HTTP/2.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080

# Missing HTTP version
echo -e "GET /test\r\nHost: localhost\r\n\r\n" | nc localhost 8080

# Oversized URI (test MAX_URI_LENGTH)
echo -e "GET /$(head -c 3000 /dev/zero | tr '\0' 'x') HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```

### 6. Performance Testing

#### Test incremental parsing:
```python
#!/usr/bin/env python3
import socket
import time

def test_incremental():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 8080))
    
    # Send request piece by piece
    pieces = [
        "GET /test",
        " HTTP/1.1\r\n",
        "Host: localhost\r\n",
        "User-Agent: Python\r\n",
        "\r\n"
    ]
    
    for piece in pieces:
        sock.send(piece.encode())
        time.sleep(0.1)  # Small delay
    
    response = sock.recv(1024)
    print(response.decode())
    sock.close()

test_incremental()
```

### 7. Debugging Tips

#### Add debug output to your Request class:
```cpp
// In Request.cpp, add debug prints:
void Request::add(Client* client, Server* conf_o, std::string new_request) {
    #ifdef DEBUG
    std::cout << "DEBUG: Received data: '" << new_request << "'" << std::endl;
    #endif
    
    _valid = addp(client, conf_o, new_request);
    
    #ifdef DEBUG
    std::cout << "DEBUG: Parse result: " << _valid << std::endl;
    std::cout << "DEBUG: Method: " << _method << std::endl;
    std::cout << "DEBUG: Path: " << _path << std::endl;
    std::cout << "DEBUG: Ready: " << _end << std::endl;
    #endif
}
```

#### Compile with debug:
```bash
g++ -DDEBUG -g -std=c++98 -I. your_files.cpp -o webserv_debug
```

### 8. Expected Results

When testing, your Request class should:

✅ **Parse correctly:**
- Extract method, path, version
- Store headers case-insensitively  
- Handle incremental data
- Parse POST body correctly

✅ **Validate properly:**
- Reject invalid methods
- Reject invalid HTTP versions
- Enforce size limits
- Return appropriate error codes

✅ **Handle edge cases:**
- Empty lines at start
- Multiple spaces in request line
- Headers with various casings
- Large requests near limits

### 9. Common Issues to Check

❌ **Parsing problems:**
- Not handling \r\n correctly
- Case sensitivity in headers
- Incomplete request handling

❌ **Memory issues:**
- Buffer overflows
- Memory leaks in copy constructor
- String allocation problems

❌ **State management:**
- Not resetting state in clear()
- Incorrect _end flag handling
- Invalid status not being set

This testing approach will help you verify that your Request class works correctly in both isolated unit tests and real HTTP scenarios.
