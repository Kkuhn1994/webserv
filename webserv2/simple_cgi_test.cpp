#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

bool testCGI(const std::string& interpreter, const std::string& script, const std::string& name) {
    std::cout << "🧪 Testing " << name << "..." << std::endl;
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", "test=simple", 1);
        execl(interpreter.c_str(), interpreter.c_str(), script.c_str(), nullptr);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        bool success = WIFEXITED(status) && WEXITSTATUS(status) == 0;
        std::cout << (success ? "✅" : "❌") << " " << name << " result" << std::endl;
        return success;
    }
    return false;
}

int main() {
    std::cout << "🚀 Simple CGI Test\n" << std::endl;
    
    int passed = 0;
    int total = 2;
    
    // Test PHP
    if (testCGI("/opt/homebrew/bin/php", "tmp/www/test.php", "PHP")) {
        passed++;
    }
    
    // Test Python  
    if (testCGI("/usr/bin/python3", "tmp/www/test.py", "Python")) {
        passed++;
    }
    
    std::cout << "\n📊 Results: " << passed << "/" << total << " tests passed" << std::endl;
    std::cout << (passed == total ? "🎉 All tests passed!" : "❌ Some tests failed") << std::endl;
    
    return (passed == total) ? 0 : 1;
}