#pragma once

#include "Server.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

class Server;
class Client;

#define NB_HEADERS 21
#define NL "\r\n"
#define NLSIZE 2

// Security and size limits
#define MAX_URI_LENGTH 2048
#define MAX_BODY_SIZE 1048576
#define MAX_HEADER_COUNT 100
#define MAX_REQUEST_SIZE 8192

enum HttpMethod {
    GET = 0,
    POST,
    DELETE,
    UNKNOWN
};

enum RequestError {
    REQ_OK = 200,
    REQ_BAD_REQUEST = 400,
};

enum ChunkState {
    CHUNK_SIZE,
    CHUNK_DATA,
    CHUNK_CRLF,
    CHUNK_TRAILER,
    CHUNK_COMPLETE
};

class Request
{
public:    
    Request();
    
    Request(const Request &src);
    
    Request &operator=(const Request &src);
    
    ~Request();
    
    // Path getters/setters
    std::string get_path() const;
    
    std::string get_path_o() const;
    
    void        set_path(std::string newpath);
    
    // Content getters
    std::string get_body() const;
    
    HttpMethod  get_method_enum() const;
    
    std::string get_method() const;
    
    std::string get_version() const;
    
    std::string get_header(std::string key);
    
    std::string get_request() const;
    
    // State checking
    int is_valid() const;
    
    RequestError get_error_code() const;
    
    bool ready() const;
    
    bool body_complete() const;
    
    // Main operations
    void add(Client* client, Server* conf_o, std::string new_request);
    
    void clear();

private:
    std::map<std::string, std::string> _m;
    std::string _body;

    // Serving and original path
    std::string _path;
    std::string _path_o;
    HttpMethod  _method_enum;
    std::string _method;
    std::string _request;
    std::string _version;
    std::string _header[NB_HEADERS];

    // Request state management
    std::string _line;
    bool        _end;
    int         _valid;
    RequestError _error_code;
    
    // Content-Length tracking
    size_t      _content_length;
    size_t      _body_bytes_received;
    bool        _body_complete;
    
    // Chunked transfer encoding
    int         _chunked;
    ChunkState  _chunk_state;
    size_t      _current_chunk_size;
    
    // tracking
    size_t      _total_request_size;
    size_t      _header_count;
    
    // parsing
    int addp(Client* client, Server* conf_o, std::string r);

    int checkHeader(Client* client, Server* conf_o, std::string r);
    
    int checkHost(std::string ip, std::string port, std::vector<std::string> server_name);
    
    bool get_request_line(std::string r);
    
    bool get_request_first_line(std::string r);
    
    void init_header_map();
    
    bool is_header(std::string s) const;
    
    bool file_exists(const char *file);
    
    // helpers
    HttpMethod parse_method(const std::string& method_str);
    
    std::string decode_uri(const std::string& uri);
    
    bool validate_request_size() const;
    
    bool validate_uri_length() const;
    
    bool validate_header_count() const;
    
    void parse_chunk_data(const std::string& data);
    
    void reset_error_state();
    
    void set_error(RequestError error);
};

