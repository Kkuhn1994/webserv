// Raw TCP Data → add() → addp() → {
//     Parse Request Line → _method, _path, _version
//     Parse Headers → _m map
//     Parse Body (POST) → _body
//     Validate Against Config → _valid status
// } → Ready for Response Generation

#include "../include/Request.hpp"

Request::Request() : _request(""),_path(""), _method(""), _version(""), _end(false), _valid(0), _body(""), _line(""), _chunked(-2), _path_o("")
{

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
    std::map<const std::string, std::string>::iterator it = _m.find(key);
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
    addp(client, conf_o, new_request);
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
}

int Request::addp(Client* client, Server* conf_o, std::string r)
{
}

int Request::checkHeader(Client *client, Server *conf_o, std::string r)
{

}

//check Host

//get_request_line

//get_request_first_line

//init_header_map

//check_file
bool Request::file_exists(const char *file)
{
    FILE *f = fopen(file, "r");
    if (f)
    {
        fclose(f);
        return (true);
    }
    return (false);
}