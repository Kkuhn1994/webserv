#include "../include/CGIExecutor.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <algorithm>

CGIExecutor::CGIExecutor() : m_timeout(30), m_maxOutputSize(1024 * 1024) {
    // Default: 30 second timeout, 1MB max output
}

CGIExecutor::~CGIExecutor() {
}

CGIResponse CGIExecutor::execute(const CGIRequest& request) {
    // Validate input
    if (request.scriptPath.empty()) {
        return handleError("Empty script path");
    }
    
    if (access(request.scriptPath.c_str(), R_OK) != 0) {
        return handleError("Script file not readable: " + request.scriptPath);
    }
    
    std::string interpreter = getInterpreter(request.scriptPath);
    if (interpreter.empty()) {
        return handleError("No interpreter found for: " + request.scriptPath);
    }
    
    // Create pipes for communication
    int input_pipe[2], output_pipe[2], error_pipe[2];
    if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1 || pipe(error_pipe) == -1) {
        return handleError("Failed to create pipes");
    }
    
    // Fork process
    pid_t pid = fork();
    if (pid == -1) {
        close(input_pipe[0]); close(input_pipe[1]);
        close(output_pipe[0]); close(output_pipe[1]);
        close(error_pipe[0]); close(error_pipe[1]);
        return handleError("Failed to fork process");
    }
    std::cout << interpreter<< "\n" << std::flush;
    if (pid == 0) {
        // Child process: Execute CGI script
        
        // Set up environment
        setupEnvironment(request);
        
        // Redirect pipes
        dup2(input_pipe[0], STDIN_FILENO);
        dup2(output_pipe[1], STDOUT_FILENO);
        dup2(error_pipe[1], STDERR_FILENO);
        
        // Close all pipe ends
        close(input_pipe[0]); close(input_pipe[1]);
        close(output_pipe[0]); close(output_pipe[1]);
        close(error_pipe[0]); close(error_pipe[1]);
        
        // Execute interpreter
        if(interpreter == "/usr/bin/php-cgi")
        {
            
            execl("/usr/bin/php-cgi", "php-cgi", nullptr);
        }
        else
        {
            execl(interpreter.c_str(), interpreter.c_str(), request.scriptPath.c_str(), nullptr);
        }
        
        
        // exec failed
        perror("execl failed");
        exit(1);
    } else {
        // Parent process: Communicate with CGI
        
        // Close unused pipe ends
        close(input_pipe[0]);
        close(output_pipe[1]);
        close(error_pipe[1]);
        
        // Send request body to script
        if (!request.body.empty()) {
            writeToScript(input_pipe[1], request.body);
        }
        close(input_pipe[1]);
        
        // Read output with timeout
        std::string output = readFromPipe(output_pipe[0]);
        std::string errorOutput = readFromPipe(error_pipe[0]);
        
        close(output_pipe[0]);
        close(error_pipe[0]);
        
        // Wait for child process
        int status;
        waitpid(pid, &status, 0);
        
        // Parse response
        CGIResponse response;
        response.exitCode = WEXITSTATUS(status);
        response.success = (response.exitCode == 0);
        
        if (!response.success) {
            response.errorMessage = "Script exited with code " + std::to_string(response.exitCode);
            if (!errorOutput.empty()) {
                response.errorMessage += ": " + errorOutput;
            }
            return response;
        }
        
        // Separate headers from body
        size_t headerEnd = output.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            headerEnd = output.find("\n\n");
            if (headerEnd != std::string::npos) {
                response.headers = output.substr(0, headerEnd);
                response.body = output.substr(headerEnd + 2);
            } else {
                // No headers found, treat everything as body
                response.headers = "Content-Type: text/html\r\n";
                response.body = output;
            }
        } else {
            response.headers = output.substr(0, headerEnd);
            response.body = output.substr(headerEnd + 4);
        }
        
        return response;
    }
}

bool CGIExecutor::isCGIFile(const std::string& path) {
    // Check file extensions
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return false;
    
    std::string ext = path.substr(dot);
    std::cout << "ext " << ext << "\n";
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == ".php" || ext == ".py" || ext == ".pl" || ext == ".this_is_a_patch_not_a_fix");
}

std::string CGIExecutor::getInterpreter(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return "";
    
    std::string ext = path.substr(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".php") {
        // Try different PHP locations
        if (access("/usr/bin/php-cgi", X_OK) == 0) return "/usr/bin/php-cgi";
        if (access("/opt/homebrew/bin/php", X_OK) == 0) return "/opt/homebrew/bin/php";
        if (access("/usr/bin/php", X_OK) == 0) return "/usr/bin/php";
        if (access("/usr/local/bin/php", X_OK) == 0) return "/usr/local/bin/php";
    } else if (ext == ".py") {
        if (access("/usr/bin/python3", X_OK) == 0) return "/usr/bin/python3";
        if (access("/usr/bin/python", X_OK) == 0) return "/usr/bin/python";
        if (access("/opt/homebrew/bin/python3", X_OK) == 0) return "/opt/homebrew/bin/python3";
    } else if (ext == ".pl") {
        if (access("/usr/bin/perl", X_OK) == 0) return "/usr/bin/perl";
        if (access("/opt/homebrew/bin/perl", X_OK) == 0) return "/opt/homebrew/bin/perl";
    }
    
    return "";
}

void CGIExecutor::setTimeout(int seconds) {
    m_timeout = seconds;
}

void CGIExecutor::setMaxOutputSize(size_t bytes) {
    m_maxOutputSize = bytes;
}

void CGIExecutor::setupEnvironment(const CGIRequest& request) {
    // Standard CGI environment variables
    setenv("REDIRECT_STATUS", "200",1);
    setenv("REQUEST_METHOD", request.method.c_str(), 1);
    std::cout << request.queryString.c_str() << "\n";
    setenv("QUERY_STRING", request.queryString.c_str(), 1);
    setenv("CONTENT_LENGTH", std::to_string(request.body.length()).c_str(), 1);
    std::string CGIFullPath = "/home/kkuhn/webserv/webserv2/" + request.scriptPath;
    std::cout << CGIFullPath << "\n";
    setenv("SCRIPT_FILENAME", CGIFullPath.c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("SERVER_SOFTWARE", "webserv/1.0", 1);
    
    // Set content type from headers if available
    auto contentType = request.headers.find("Content-Type");
    if (contentType != request.headers.end()) {
        setenv("CONTENT_TYPE", contentType->second.c_str(), 1);
    } else {
        setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
    }
    
    // Add custom environment variables
    for (const auto& env : request.env) {
        setenv(env.first.c_str(), env.second.c_str(), 1);
    }
}

std::string CGIExecutor::readFromPipe(int fd) {
    std::string result;
    char buffer[4096];
    ssize_t bytes;
    
    // Set non-blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    fd_set readfds;
    struct timeval timeout;
    
    while (result.size() < m_maxOutputSize) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        timeout.tv_sec = m_timeout;
        timeout.tv_usec = 0;
        
        int ready = select(fd + 1, &readfds, nullptr, nullptr, &timeout);
        if (ready <= 0) break; // Timeout or error
        
        bytes = read(fd, buffer, sizeof(buffer));
        if (bytes <= 0) break; // EOF or error
        
        result.append(buffer, bytes);
    }
    
    return result;
}

bool CGIExecutor::writeToScript(int fd, const std::string& data) {
    size_t written = 0;
    while (written < data.length()) {
        ssize_t bytes = write(fd, data.c_str() + written, data.length() - written);
        if (bytes <= 0) return false;
        written += bytes;
    }
    return true;
}

CGIResponse CGIExecutor::handleError(const std::string& message) {
    CGIResponse response;
    response.success = false;
    response.exitCode = -1;
    response.errorMessage = message;
    response.headers = "Content-Type: text/html\r\n";
    response.body = "<h1>500 Internal Server Error</h1><p>" + message + "</p>";
    return response;
}
