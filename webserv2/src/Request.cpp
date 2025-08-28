// Raw TCP Data → add() → addp() → {
//     Parse Request Line → _method, _path, _version
//     Parse Headers → _m map
//     Parse Body (POST) → _body
//     Validate Against Config → _valid status
// } → Ready for Response Generation

#include "../include/Request.hpp"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <algorithm>

Request::Request() : _body(""), _path(""), _path_o(""), _method_enum(UNKNOWN), _method(""), _request(""), _version(""),
                     _line(""), _end(false), _valid(0), _error_code(REQ_OK), _content_length(0), _body_bytes_received(0), 
                     _body_complete(false), _chunked(-2), _chunk_state(CHUNK_SIZE), _current_chunk_size(0), 
                     _total_request_size(0), _header_count(0)
{
    init_header_map();
}
Request::~Request() {};

Request::Request(const Request &src)
{
    *this = src;
}

Request &Request::operator=(const Request &src)
{
    if (this != &src)
    {
        _m = src._m;
        _body = src._body;
        _path = src._path;
        _path_o = src._path_o;
        _method_enum = src._method_enum;
        _method = src._method;
        _request = src._request;
        _version = src._version;
        for (int i = 0; i < NB_HEADERS; ++i)
            _header[i] = src._header[i];
        _line = src._line;
        _end = src._end;
        _valid = src._valid;
        _error_code = src._error_code;
        _content_length = src._content_length;
        _body_bytes_received = src._body_bytes_received;
        _body_complete = src._body_complete;
        _chunked = src._chunked;
        _chunk_state = src._chunk_state;
        _current_chunk_size = src._current_chunk_size;
        _total_request_size = src._total_request_size;
        _header_count = src._header_count;
    }
    return *this;
}

std::string Request::get_path() const
{
    return _path;
}

std::string Request::get_path_o() const
{
    return _path_o;
}

void Request::set_path(std::string newpath)
{
    _path = newpath;
    return ;
}

std::string Request::get_body() const
{
    return _body;
}

HttpMethod Request::get_method_enum() const
{
    return _method_enum;
}

std::string Request::get_method() const
{
    return _method;
}

std::string Request::get_version() const
{
    return _version;
}

std::string Request::get_header(std::string key)
{
    std::map<std::string, std::string>::iterator it = _m.find(key);
    if (it != _m.end())
        return it->second;
    return "";
}

std::string Request::get_request() const
{
    return _request;
}

int Request::is_valid() const
{
    return _valid;
}

RequestError Request::get_error_code() const
{
    return _error_code;
}

bool Request::ready() const
{
    return _end;
}

bool Request::body_complete() const
{
    return _body_complete;
}

void Request::add(Client* client, Server* conf_o, std::string new_request)
{
    _valid = addp(client, conf_o, new_request);
    
    if (_valid == 0 && _end == true && _method == "POST")
    {
        std::string content_type = get_header("Content-Type");
        if (content_type != "text/plain" && 
            content_type.find("multipart/form-data") != 0 && 
            content_type != "application/x-www-form-urlencoded")
        {
            _valid = 400;
            _error_code = REQ_BAD_REQUEST;
        }
    }
}

void Request::clear()
{
    _m.clear();
    _body.clear();
    _path.clear();
    _path_o.clear();
    _method.clear();
    _request.clear();
    _version.clear();
    for (int i = 0; i < NB_HEADERS; ++i)
        _header[i].clear();
    _line.clear();
    _end = false;
    _valid = 0;
    _error_code = REQ_OK;
    _content_length = 0;
    _body_bytes_received = 0;
    _body_complete = false;
    _chunked = -2;
    _chunk_state = CHUNK_SIZE;
    _current_chunk_size = 0;
    _total_request_size = 0;
    _header_count = 0;
    _method_enum = UNKNOWN;
    init_header_map();
}

int Request::addp(Client* client, Server* conf_o, std::string r)
{
    std::string::size_type nl;
        if (_request == "" && r == "\r\n")
        return 0;
            if (_line == "" && _request.find("\r\n\r\n") == std::string::npos && 
        (r == "" || r.substr(0, 1) == " "))
        return 400;
        
    r = _line + r;
    _line = "";
    
    // Process line by line
    while ((nl = r.find(NL)) != std::string::npos)
    {
        std::string current_line = r.substr(0, nl);
        
        // First line: parse request line
        if (_request.empty())
        {
            if (!get_request_first_line(current_line))
                return 400;
        }
        // End of headers
        else if (current_line.empty() && _request.find("\r\n\r\n") == std::string::npos)
        {
            int ret = checkHeader(client, conf_o, r);
            if (ret != -1)
                return ret;
        }
        // Parse header lines
        else if (!_end)
        {
            if (!get_request_line(current_line))
                return 400;
        }
        
        _request += r.substr(0, nl + NLSIZE);
        r.erase(0, nl + NLSIZE);
    }
    
    // Handle remaining data
    if (_method == "POST" && _end && !r.empty())
    {
        // Handle chunked transfer encoding
        if (_chunked == -1)
        {
            parse_chunk_data(r);
        }
        // Handle regular content-length based body
        else
        {
            _body += r;
            _body_bytes_received += r.length();
            
            if (_body_bytes_received >= _content_length)
            {
                _body_complete = true;
                if (_body_bytes_received > _content_length)
                {
                    _line = _body.substr(_content_length);
                    _body = _body.substr(0, _content_length);
                }
            }
        }
    }
    else
    {
        _line = r;
    }
    
    return 0;
}

int Request::checkHeader(Client *client, Server *conf_o, std::string r)
{
    (void)r;
    (void)client;
    (void)conf_o;
    
    _path_o = _path;
    if (_method == "GET" || _method == "DELETE")
    {
        _end = true;
        return 0;
    }
    
    if (_method == "POST")
    {
        std::string content_length_str = get_header("Content-Length");
        std::string transfer_encoding = get_header("Transfer-Encoding");
        
        // Check for chunked encoding (required by subject)
        if (transfer_encoding == "chunked")
        {
            _chunked = -1;
            _chunk_state = CHUNK_SIZE;
        }
        // Check for content-length
        else if (!content_length_str.empty())
        {
            _content_length = std::atoi(content_length_str.c_str());
            if (_content_length > MAX_BODY_SIZE)
                return 413; // Request too large
        }
        else
        {
            return 411; // Content-Length header is missing
        }
        
        _end = true;
    }
    
    return -1;
}

//check Host
int Request::checkHost(std::string ip, std::string port, std::vector<std::string> server_name)
{
    std::string host_header = get_header("Host");
    
    if (host_header.empty())
        return 400; // Bad request
    
    // Check if host matches IP:port
    if (host_header == ip + ":" + port)
        return 0;
    
    // Check server names
    for (size_t i = 0; i < server_name.size(); i++)
    {
        if (server_name[i] == host_header || 
            server_name[i] + ":" + port == host_header)
            return 0;
    }
    
    // Check if it's just IP (for port 80)
    if (port == "80" && host_header == ip)
        return 0;
        
    return 400; // Host not found
}
bool Request::get_request_line(std::string r)
{
    size_t colon_pos = r.find(": ");
    if (colon_pos == std::string::npos)
        return false;
    
    std::string header_name = r.substr(0, colon_pos);
    std::string header_value = r.substr(colon_pos + 2);
        if (header_name.find(" ") != std::string::npos)
        return false;
    for (size_t i = 0; i < header_name.length(); i++)
        header_name[i] = std::tolower(header_name[i]);
    
    _m[header_name] = header_value;
    _header_count++;
    
    if (_header_count > MAX_HEADER_COUNT)
        return false;
    
    return true;
}

bool Request::get_request_first_line(std::string r)
{
    if (r == NL)
        return true;
    
    // Parse: METHOD PATH HTTP/VERSION
    size_t first_space = r.find(" ");
    if (first_space == std::string::npos)
        return false;
    
    // Extract method
    _method = r.substr(0, first_space);
    _method_enum = parse_method(_method);
    if (_method_enum == UNKNOWN)
        return false;
    
    // Find second space
    size_t second_space = r.find(" ", first_space + 1);
    if (second_space == std::string::npos)
        return false;
    
    // Extract path
    _path = r.substr(first_space + 1, second_space - first_space - 1);
    _path_o = _path;
    
    // Validate URI length
    if (_path.length() > MAX_URI_LENGTH)
        return false;
    
    // Extract version
    _version = r.substr(second_space + 1);
    
    // Validate HTTP version
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
        return false;
    
    return true;
}

//init_header_map
void Request::init_header_map()
{
    _header[0] = "accept-charsets";
    _header[1] = "accept-language";
    _header[2] = "auth-scheme";
    _header[3] = "authorization";
    _header[4] = "content-language";
    _header[5] = "content-length";
    _header[6] = "content-location";
    _header[7] = "content-type";
    _header[8] = "date";
    _header[9] = "host";
    _header[10] = "last-modified";
    _header[11] = "location";
    _header[12] = "referer";
    _header[13] = "retry-after";
    _header[14] = "transfer-encoding";
    _header[15] = "user-agent";
    _header[16] = "connection";
    _header[17] = "accept";
    _header[18] = "cookie";
    _header[19] = "accept-encoding";
    _header[20] = "from";
    
    // Initialize map with empty values
    for (int i = 0; i < NB_HEADERS; i++)
        _m[_header[i]] = "";
}

bool Request::is_header(std::string s) const
{
    // Convert to lowercase for comparison
    std::string lower_s = s;
    for (size_t i = 0; i < lower_s.length(); i++)
        lower_s[i] = std::tolower(lower_s[i]);
    
    for (int i = 0; i < NB_HEADERS; i++)
    {
        if (_header[i] == lower_s)
            return true;
    }
    return false;
}

bool Request::file_exists(const char *file)
{
    FILE *f = fopen(file, "r");
    if (f)
    {
        fclose(f);
        return true;
    }
    return false;
}

// Helper function implementations
HttpMethod Request::parse_method(const std::string& method_str)
{
    if (method_str == "GET")
        return GET;
    else if (method_str == "POST")
        return POST;
    else if (method_str == "DELETE")
        return DELETE;
    else
        return UNKNOWN;
}

std::string Request::decode_uri(const std::string& uri)
{
    std::string decoded;
    for (size_t i = 0; i < uri.length(); i++)
    {
        if (uri[i] == '%' && i + 2 < uri.length())
        {
            // Convert hex to char
            std::string hex = uri.substr(i + 1, 2);
            char ch = static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
            decoded += ch;
            i += 2;
        }
        else
        {
            decoded += uri[i];
        }
    }
    return decoded;
}

bool Request::validate_request_size() const
{
    return _total_request_size <= MAX_REQUEST_SIZE;
}

bool Request::validate_uri_length() const
{
    return _path.length() <= MAX_URI_LENGTH;
}

bool Request::validate_header_count() const
{
    return _header_count <= MAX_HEADER_COUNT;
}

void Request::parse_chunk_data(const std::string& data)
{
    std::string remaining = data;
    
    while (!remaining.empty())
    {
        if (_chunk_state == CHUNK_SIZE)
        {
            // Look for CRLF to get chunk size line
            size_t crlf_pos = remaining.find("\r\n");
            if (crlf_pos == std::string::npos)
            {
                // Not enough data yet, store for next call
                _line += remaining;
                break;
            }
            
            // Parse chunk size (hexadecimal)
            std::string size_line = _line + remaining.substr(0, crlf_pos);
            _line.clear();
            
            // Convert hex string to integer
            _current_chunk_size = std::strtol(size_line.c_str(), NULL, 16);
            
            // If chunk size is 0, we're done - CGI expects EOF
            if (_current_chunk_size == 0)
            {
                _chunk_state = CHUNK_TRAILER;
                _body_complete = true;
            }
            else
            {
                _chunk_state = CHUNK_DATA;
            }
            
            remaining.erase(0, crlf_pos + 2);
        }
        else if (_chunk_state == CHUNK_DATA)
        {
            // Read chunk data and add to body (unchunked)
            size_t bytes_to_read = std::min(remaining.length(), _current_chunk_size);
            _body += remaining.substr(0, bytes_to_read);
            _current_chunk_size -= bytes_to_read;
            remaining.erase(0, bytes_to_read);
            
            // If we've read the full chunk, expect CRLF
            if (_current_chunk_size == 0)
            {
                _chunk_state = CHUNK_CRLF;
            }
        }
        else if (_chunk_state == CHUNK_CRLF)
        {
            // Expect CRLF after chunk data
            if (remaining.length() >= 2 && remaining.substr(0, 2) == "\r\n")
            {
                remaining.erase(0, 2);
                _chunk_state = CHUNK_SIZE;
            }
            else
            {
                // Invalid chunk format
                set_error(REQ_BAD_REQUEST);
                return;
            }
        }
        else if (_chunk_state == CHUNK_TRAILER)
        {
            // Read trailer headers until final CRLF
            size_t crlf_pos = remaining.find("\r\n");
            if (crlf_pos == std::string::npos)
            {
                _line += remaining;
                break;
            }
            
            std::string trailer_line = _line + remaining.substr(0, crlf_pos);
            _line.clear();
            
            if (trailer_line.empty())
            {
                _body_complete = true;
                remaining.erase(0, crlf_pos + 2);
                break;
            }
            remaining.erase(0, crlf_pos + 2);
        }
    }
}

void Request::reset_error_state()
{
    _error_code = REQ_OK;
    _valid = 0;
}

void Request::set_error(RequestError error)
{
    _error_code = error;
    _valid = static_cast<int>(error);
}
