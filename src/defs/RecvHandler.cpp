/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RecvHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 12:10:10 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 16:24:45 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/RecvHandler.hpp"

/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/

RecvHandler::RecvHandler() : _is_done(0), _is_head(1), _is_cgi(0), _is_chunk(0), _do_one(0), _is_exec(0), _is_close(1),
_rlength(0), _cgi_wlen(0), _icgi_fd(0), _servers(nullptr), _location(nullptr), _unchunker(nullptr)
{
    _rhead.resize(BUF_SIZE);
    _r_fields.resize(12);
    _s_fields.resize(10);
    _s_fields[HS_LCRLF] = "\r\n";
}
RecvHandler::~RecvHandler()
{
    delete _unchunker;
}

void RecvHandler::init_variables(void)
{
    _is_done = 0;
    _is_head = 1;
    _is_cgi = 0;
    _is_chunk = 0;
    _do_one = 0;
    _is_exec = 0;
    _rlength = 0;
    _cgi_wlen = 0;
    _icgi_fd = 0;
    _is_close = 1;
    _rhead.clear();
    _rhead.resize(BUF_SIZE);
    _rbody.clear();
    _chunk_h.clear();
    _r_fields.clear();
    _r_fields.resize(12);
    _s_fields.clear();
    _s_fields.resize(10);
    _location = nullptr;
    delete _unchunker;
    _unchunker = nullptr;
    _s_fields[HS_LCRLF] = "\r\n";
}
/******************************************************************************/
/*                              getters-setters                               */
/******************************************************************************/
ServerSet *RecvHandler::get_matched_server(std::vector<ServerSet*> *server_list)
{
    if (server_list->size() != 1)
    {
        for (size_t i = 0; i < server_list->size(); ++i)
        {
            std::set<std::string>::iterator it;
            it = (*server_list)[i]->_server_name.find(_r_fields[HR_HOST]);
            if (it != (*server_list)[i]->_server_name.end())
                return ((*server_list)[i]);
        }
    }
    return ((*server_list)[0]);
}

LocationSet *RecvHandler::get_matched_location(std::vector<ServerSet*> *server_list)
{
    ServerSet   *server;
    std::string url;

    server = get_matched_server(server_list);
    url = _r_fields[HR_URL];
    while (url.length() && server->_locations.size() > 1)
    {
        for (size_t i = 1; i < server->_locations.size(); ++i)
        {
            if (url == server->_locations[i]._url_path)
            {
                if (!server->_locations[i]._redirect.first.empty())
                {
                    _r_fields[HR_URL].erase(0, server->_locations[i]._url_path.length());
                    if (server->_locations[i]._redirect.first[server->_locations[i]._redirect.first.size() - 1]== '/')
                        server->_locations[i]._redirect.first.pop_back();
                    _r_fields[HR_URL].insert(0, server->_locations[i]._redirect.first);
                    _s_fields[HS_LOCATN] = _r_fields[HR_URL];
                    if (server->_locations[i]._redirect.second == 301)
                        throw response_status(SC_301);
                    else
                        throw response_status(SC_302);
                }
                return (&server->_locations[i]);
            }
        }
        size_t pos = url.find_last_of("/");
        if (pos == 0 && url.length() > 1)
            pos++;
        if (pos == std::string::npos)
            pos = 0;
        url = url.substr(0, pos);
    }
    return (&server->_locations.front());
}

void RecvHandler::set_servers(std::vector<ServerSet*>* _sv)
{
    _servers = _sv;
}
void RecvHandler::set_response(Response* _sv)
{
    _response = _sv;
}
void RecvHandler::set_sock_fd(int _sv)
{
    _sock_fd = _sv;
}
int RecvHandler::get_sock_fd(void)
{
    return (_sock_fd);
}
bool RecvHandler::get_is_done(void)
{
    return (_is_done);
}
int RecvHandler::get_cgi_fd(void)
{
    return (_icgi_fd);
}
bool RecvHandler::get_is_close(void)
{
    return (_is_close);
}
void RecvHandler::set_is_done(bool _sv)
{
    _is_done = _sv;
}

/******************************************************************************/
/*                                 fonctor                                    */
/******************************************************************************/

void RecvHandler::operator()(int fd, int ndata)
{
    if (_is_head)
        recv_head(fd, ndata);
    if (!_is_head && !_is_exec)
    {
        if (_is_chunk)
            recv_chunked_body(fd, ndata);
        else if (!_is_chunk)
            recv_body(fd, ndata);
    }
    if (_is_exec)
        build_post_response();
}

/******************************************************************************/
/*                                  recv head                                 */
/******************************************************************************/

void RecvHandler::recv_head(int fd, int &ndata)
{
    size_t  hif = _rlength;
    int     rdata = 0;
    size_t  dcrlf;
   
    do
    {
        if (_rlength == _rhead.length())
            _rhead.resize(_rlength * 2);
        _rlength += rdata = ::recv(fd, (void *)(_rhead.data() + _rlength), _rhead.size() - _rlength, 0);
        if (rdata == -1)
            throw server_error(std::string("error: recv: ") + ::strerror(errno));
        if (rdata == 0)
            throw server_error(std::string("recv: Connection closed by client."));
        ndata -= rdata;
    } while (ndata > 0);
    dcrlf = find_dcrlf(_rhead, hif);
    if (dcrlf != std::string::npos)
    {
        try
        {
            try
            {
                RequestParser request_parser(&_rhead, &_r_fields);
                
                request_parser.parse_first_line();
                request_parser.parse_header();
                _location = get_matched_location(_servers);
                _is_close = _r_fields[HR_CONNECT] == "close";
                
                RequestProcessor request_processor(_response, &_r_fields, &_s_fields, _location);
                _is_head = _is_done = request_processor.process_request();
                
                if (_is_done)
                    build_header(_response->_head, _s_fields);
                _rlength -= dcrlf;
                _rbody = &_rhead[dcrlf];
            }
            catch (const response_status &e)
            {
                if (!_location)
                    _location = get_matched_location(_servers);
                _s_fields[HS_STCODE] = e.what();
                build_body_error(*_response, _s_fields, _location->_err_page);
                build_header(_response->_head, _s_fields);
                _is_done = true;
            }
        }
        catch (const server_error &e)
        {
            if (!_location)
                _location = get_matched_location(_servers);
            std::cerr << e.what() << '\n';
            _s_fields[HS_STCODE] = SC_500;
            build_body_error(*_response, _s_fields, _location->_err_page);
            build_header(_response->_head, _s_fields);
            _is_done = true;
        }
        if (!_is_head)
        {
            if (!_r_fields[HR_CNTLEN].empty())
                _rbody.resize(stoul(_r_fields[HR_CNTLEN]));
            else if (_r_fields[HR_TENCOD] == "chunked")
            {
                _unchunker = new BodyUnchunker(&_rbody);
                _is_chunk = true;
            }
        }
    }
}
/******************************************************************************/
/*                                  recv body                                 */
/******************************************************************************/
void RecvHandler::recv_body(int fd, int ndata)
{
    int     rdata = 0;
    
    do
    {
        if (!_is_cgi && ndata > 0)
        {
            _rlength += rdata = ::recv(fd, (void *)(_rbody.data() + _rlength), _rbody.size() - _rlength, 0);
            if (rdata == -1)
                throw server_error(std::string("error: recv: ") + ::strerror(errno));
            if (rdata == 0)
                throw server_error(std::string("recv: Connection closed by client."));
            ndata -= rdata;
        }
        if (_rlength == _rbody.length())
        {
            _is_exec = true;
            return;
        }
    } while (ndata > 0);
}

/******************************************************************************/
/*                            recv chunked body                               */
/******************************************************************************/

void RecvHandler::recv_chunked_body(int fd, int ndata)
{
    int     rdata = 0;
    std::string chunk;
    
    if (!_do_one)
    {
        chunk = _rbody;
        _rbody.clear();
        _do_one = true;
    }
    if (!_chunk_h.empty())
    {
        chunk = _chunk_h;
        _chunk_h.clear();
    }
    
    if (ndata > 0)
    {
        _rlength = chunk.length();
        chunk.resize(ndata + chunk.length());
        do
        {
            _rlength += rdata = ::recv(fd, (void *)(chunk.data() + _rlength), ndata, 0);
            if (rdata == -1)
                throw server_error(std::string("error: recv: ") + ::strerror(errno));
            if (rdata == 0)
                throw server_error(std::string("recv: Connection closed by client."));
            ndata -= rdata;
        } while (ndata > 0);
    }
    int  rc = _unchunker->parse_chunked_body(chunk);
    if (rc == -1)
    {
        _r_fields[HR_CNTLEN] = to_str(_rbody.length());
        _is_exec = true;
        return;
    }
    else if (rc >= 0)
        _chunk_h = chunk.substr(rc, chunk.length() - rc);
}

/******************************************************************************/
/*                             build POST response                            */
/******************************************************************************/
void RecvHandler::build_post_response(void)
{
    try
    {
        try
        {
            if (!_is_cgi)
            {
                if (is_cgi(_r_fields, _location->_cgi))
                {
                    CGIExecutor cgi(_r_fields, *_location);
    
                    cgi.set_body(&_rbody);
                    _icgi_fd = cgi.execute_cgi();
                }
                else
                    throw response_status(SC_403);
            }
            if (!CGIExecutor::pass_input_to_cgi(_rbody, _cgi_wlen, _icgi_fd))
            {
                mmap_file(*_response, "/tmp/cgi_tmp_file");
                _s_fields[HS_STCODE] = SC_201;
        
                size_t pos;
                pos = get_pos_string(_response->_body, _response->_body_len, "\n\n");
                if (pos == std::string::npos)
                    pos = get_pos_string(_response->_body, _response->_body_len, "\r\n\r\n");
                if (pos == std::string::npos)
                    pos = 0;
                if (pos)
                {
                    _s_fields[HS_LCRLF] = "";
                    get_status_from_cgi_response(std::string(_response->_body, _response->_body + pos), _s_fields[HS_STCODE]);
                }
                    
                size_t clen = _response->_body_len - pos;
                _s_fields[HS_CNTLEN] = to_str(clen);
                build_header(_response->_head, _s_fields);
                _is_done = true;
            }
            else if (!_is_cgi)
                _is_cgi = true;
        }
        catch (const response_status &e)
        {
            _s_fields[HS_STCODE] = e.what();
            build_body_error(*_response, _s_fields, _location->_err_page);
            build_header(_response->_head, _s_fields);
            _is_done = true;
        }
    }
    catch (const server_error &e)
    {
        std::cerr << e.what() << '\n';
        _s_fields[HS_STCODE] = SC_500;
        build_body_error(*_response, _s_fields, _location->_err_page);
        build_header(_response->_head, _s_fields);
        _is_done = true;
    }
}

