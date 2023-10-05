/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RecvHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 12:07:27 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/12 10:55:26 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RECV_HANDLER_HPP
#define RECV_HANDLER_HPP

# include <vector>
# include <cstring>
# include <iostream>
# include <sys/socket.h>
# include "../incl/utils.hpp"
# include "../incl/exceptions.hpp"
# include "../incl/mini_structs.hpp"
# include "../class/SendHandler.hpp"
# include "../class/RequestParser.hpp"
# include "../class/RequestProcessor.hpp"
# include "../class/CGIExecutor.hpp"
#include "../class/BodyUnchunker.hpp"

class RecvHandler
{
    bool                        _is_done;
    bool                        _is_head;
    bool                        _is_cgi;
    bool                        _is_chunk;
    bool                        _do_one;
    bool                        _is_exec;
    bool                        _is_close;
    size_t                      _rlength;
    size_t                      _cgi_wlen;
    int                         _icgi_fd;
    int                         _sock_fd;
    std::string                 _rhead;
    std::string                 _rbody;
    std::string                 _chunk_h;
    Response*                   _response;
    std::vector<std::string>    _r_fields;
    std::vector<std::string>    _s_fields;
    std::vector<ServerSet*>*    _servers;
    LocationSet*                _location;
    BodyUnchunker*              _unchunker;
    
    public:
        RecvHandler();  
        ~RecvHandler();
        void init_variables(void);
        void operator()(int fd, int ndata);
        void recv_head(int fd, int &ndata);
        void recv_body(int fd, int ndata);
        void recv_chunked_body(int fd, int ndata);
        void build_post_response(void);
        void set_servers(std::vector<ServerSet*>*);
        void set_response(Response*);
        void set_sock_fd(int);
        void set_is_done(bool);
        bool get_is_done(void);
        int get_sock_fd(void);
        int get_cgi_fd(void);
        bool get_is_close(void);
        ServerSet *get_matched_server(std::vector<ServerSet*> *server_list);
        LocationSet *get_matched_location(std::vector<ServerSet*> *server_list);
};

#endif
