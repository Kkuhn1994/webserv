#!/usr/bin/python3
import os
import sys

print("Content-Type: text/html\r\n")
print("<h1>Python CGI Test</h1>")
print(f"<p>Method: {os.environ.get('REQUEST_METHOD', 'GET')}</p>")
print(f"<p>Query String: {os.environ.get('QUERY_STRING', 'none')}</p>")
print(f"<p>Script Path: {os.environ.get('SCRIPT_FILENAME', 'unknown')}</p>")
print(f"<p>Gateway Interface: {os.environ.get('GATEWAY_INTERFACE', 'unknown')}</p>")
print(f"<p>Server Software: {os.environ.get('SERVER_SOFTWARE', 'unknown')}</p>")

if os.environ.get('REQUEST_METHOD') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', '0'))
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        print(f"<p>POST Data: {post_data}</p>")

print("<h2>All Environment Variables:</h2>")
print("<pre>")
for key, value in sorted(os.environ.items()):
    print(f"{key} = {value}")
print("</pre>")