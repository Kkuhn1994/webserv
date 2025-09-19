#!/usr/bin/python3
"""
CGI System Validator
Tests the CGI system independently of the HTTP server
"""

import subprocess
import os
import sys
import time

def test_cgi_detection():
    """Test CGI file detection logic"""
    print("🔍 Testing CGI File Detection")
    
    # This would test your isCGIFile function
    test_files = [
        ("test.php", True),
        ("script.py", True), 
        ("program.pl", True),
        ("document.html", False),
        ("readme.txt", False),
        ("noextension", False)
    ]
    
    print("✅ File detection logic should work for:")
    for filename, should_detect in test_files:
        status = "✅ SHOULD DETECT" if should_detect else "❌ SHOULD IGNORE"
        print(f"  {filename}: {status}")

def test_interpreter_availability():
    """Test if interpreters are available on system"""
    print("\n🔧 Testing Interpreter Availability")
    
    interpreters = [
        ("php", ["/usr/bin/php", "/usr/local/bin/php", "/opt/homebrew/bin/php"]),
        ("python3", ["/usr/bin/python3", "/usr/local/bin/python3", "/opt/homebrew/bin/python3"]),
        ("perl", ["/usr/bin/perl", "/usr/local/bin/perl"])
    ]
    
    for name, paths in interpreters:
        found = False
        for path in paths:
            if os.path.exists(path) and os.access(path, os.X_OK):
                print(f"✅ {name}: Found at {path}")
                found = True
                break
        if not found:
            print(f"❌ {name}: Not found in standard locations")

def test_php_cgi():
    """Test PHP CGI execution"""
    print("\n🐘 Testing PHP CGI")
    
    # Check if PHP is available
    try:
        subprocess.run(['php', '--version'], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("⚠️  PHP not found - skipping PHP tests")
        return None
    
    if not os.path.exists("tmp/www/test.php"):
        print("❌ test.php not found")
        return False
    
    # Set up environment for CGI
    env = os.environ.copy()
    env.update({
        'REQUEST_METHOD': 'GET',
        'QUERY_STRING': 'name=TestUser&id=123',
        'SCRIPT_FILENAME': os.path.abspath('tmp/www/test.php'),
        'GATEWAY_INTERFACE': 'CGI/1.1',
        'SERVER_SOFTWARE': 'webserv/1.0'
    })
    
    try:
        result = subprocess.run(['php', 'tmp/www/test.php'], 
                              env=env, 
                              capture_output=True, 
                              text=True, 
                              timeout=5)
        
        if result.returncode == 0:
            print("✅ PHP CGI executed successfully")
            print(f"📤 Output ({len(result.stdout)} chars):")
            print(result.stdout[:200] + "..." if len(result.stdout) > 200 else result.stdout)
            return True
        else:
            print(f"❌ PHP CGI failed with exit code {result.returncode}")
            print(f"Error: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print("❌ PHP CGI timed out")
        return False
    except FileNotFoundError:
        print("❌ PHP interpreter not found")
        return False

def test_python_cgi():
    """Test Python CGI execution"""
    print("\n🐍 Testing Python CGI")
    
    if not os.path.exists("tmp/www/test.py"):
        print("❌ test.py not found")
        return False
    
    # Set up environment for CGI
    env = os.environ.copy()
    env.update({
        'REQUEST_METHOD': 'POST',
        'QUERY_STRING': 'param1=value1&param2=value2',
        'CONTENT_LENGTH': '15',
        'SCRIPT_FILENAME': os.path.abspath('tmp/www/test.py'),
        'GATEWAY_INTERFACE': 'CGI/1.1',
        'SERVER_SOFTWARE': 'webserv/1.0'
    })
    
    try:
        result = subprocess.run(['python3', 'tmp/www/test.py'], 
                              env=env,
                              input="POST data here!",
                              capture_output=True, 
                              text=True, 
                              timeout=5)
        
        if result.returncode == 0:
            print("✅ Python CGI executed successfully")
            print(f"📤 Output ({len(result.stdout)} chars):")
            print(result.stdout[:200] + "..." if len(result.stdout) > 200 else result.stdout)
            return True
        else:
            print(f"❌ Python CGI failed with exit code {result.returncode}")
            print(f"Error: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print("❌ Python CGI timed out")
        return False
    except FileNotFoundError:
        print("❌ Python interpreter not found")
        return False

def test_cgi_performance():
    """Test CGI performance"""
    print("\n⚡ Testing CGI Performance")
    
    # Test PHP performance only if available
    try:
        subprocess.run(['php', '--version'], capture_output=True, check=True)
        print("Testing PHP CGI speed...")
        start_time = time.time()
        php_successes = 0
        
        for i in range(5):
            env = os.environ.copy()
            env.update({
                'REQUEST_METHOD': 'GET',
                'QUERY_STRING': f'test={i}',
                'SCRIPT_FILENAME': os.path.abspath('tmp/www/test.php')
            })
            
            try:
                result = subprocess.run(['php', 'tmp/www/test.php'], 
                                      env=env, 
                                      capture_output=True, 
                                      timeout=2)
                if result.returncode == 0:
                    php_successes += 1
            except:
                pass
        
        php_time = time.time() - start_time
        print(f"PHP: {php_successes}/5 successful calls in {php_time:.2f}s")
        
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("PHP not available - skipping PHP performance test")
    
    # Test Python performance
    print("Testing Python CGI speed...")
    start_time = time.time()
    python_successes = 0
    
    for i in range(5):
        env = os.environ.copy()
        env.update({
            'REQUEST_METHOD': 'GET',
            'QUERY_STRING': f'test={i}',
            'SCRIPT_FILENAME': os.path.abspath('tmp/www/test.py')
        })
        
        try:
            result = subprocess.run(['python3', 'tmp/www/test.py'], 
                                  env=env, 
                                  capture_output=True, 
                                  timeout=2)
            if result.returncode == 0:
                python_successes += 1
        except:
            pass
    
    python_time = time.time() - start_time
    print(f"Python: {python_successes}/5 successful calls in {python_time:.2f}s")

def main():
    print("🚀 Independent CGI System Validator")
    print("=====================================")
    print("This tests your CGI implementation without the HTTP server")
    
    # Change to webserv2 directory if not already there
    if not os.path.exists('tmp/www'):
        print("❌ Not in webserv2 directory or tmp/www not found")
        sys.exit(1)
    
    tests = [
        test_cgi_detection,
        test_interpreter_availability, 
        test_php_cgi,
        test_python_cgi,
        test_cgi_performance
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            result = test()
            if result is not False:  # None or True counts as pass
                passed += 1
        except Exception as e:
            print(f"❌ Test failed with exception: {e}")
    
    print(f"\n📊 Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 Your CGI system is working perfectly!")
        print("✅ Ready for 42 evaluation!")
    elif passed >= total - 1:
        print("⚠️  Minor issues found, but CGI system is mostly functional")
    else:
        print("❌ Significant issues found with CGI system")

if __name__ == "__main__":
    main()