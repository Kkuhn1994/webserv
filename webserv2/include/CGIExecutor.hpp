#ifndef CGIEXECUTOR_HPP
#define CGIEXECUTOR_HPP

#include <string>
#include <map>
#include <vector>

// Simple data structures for CGI communication
struct CGIRequest {
    std::string method;           // GET, POST, etc.
    std::string scriptPath;       // Full path to CGI script
    std::string queryString;      // URL parameters
    std::string body;             // POST data
    std::map<std::string, std::string> headers;  // HTTP headers
    std::map<std::string, std::string> env;      // Additional environment variables
};

struct CGIResponse {
    std::string headers;          // CGI output headers
    std::string body;             // CGI output body
    int exitCode;                 // Process exit status
    bool success;                 // Whether execution succeeded
    std::string errorMessage;     // Error description if failed
};

class CGIExecutor {
public:
    CGIExecutor();
    ~CGIExecutor();
    
    // Main execution method - completely independent
    CGIResponse execute(const CGIRequest& request);
    
    // Utility methods
    static bool isCGIFile(const std::string& path);
    static std::string getInterpreter(const std::string& path);
    
    // Configuration
    void setTimeout(int seconds);
    void setMaxOutputSize(size_t bytes);
    
private:
    int m_timeout;
    size_t m_maxOutputSize;
    
    // Internal methods
    void setupEnvironment(const CGIRequest& request);
    std::string readFromPipe(int fd);
    bool writeToScript(int fd, const std::string& data);
    CGIResponse handleError(const std::string& message);
};

#endif
