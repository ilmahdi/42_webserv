/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_structs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 14:14:44 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/11 12:38:43 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINI_STRUCTS_HPP
#define MINI_STRUCTS_HPP


# include <set>
# include <map>
# include <vector>
# include <string>
# include <utility>
# include <sys/mman.h>


struct LocationSet
{
    std::map<std::string, std::string>  _cgi;
    std::map<int, std::string>          _err_page;
    std::pair<std::string, int>         _redirect;
    std::set<std::string>               _acc_mtod;
    std::set<std::string>               _index;
    std::string                         _root;
    std::string                         _url_path;
    std::string                         _upload_dir;
    size_t                              _cb_max_size;
    bool                                _autoindex;
    LocationSet() : _cb_max_size(1000000000), _autoindex(0)
    {
        _err_page[400] = "./eva/error_pages/ep_SC_400.html";
        _err_page[403] = "./eva/error_pages/ep_SC_403.html";
        _err_page[404] = "./eva/error_pages/ep_SC_404.html";
        _err_page[405] = "./eva/error_pages/ep_SC_405.html";
        _err_page[409] = "./eva/error_pages/ep_SC_409.html";
        _err_page[411] = "./eva/error_pages/ep_SC_411.html";
        _err_page[413] = "./eva/error_pages/ep_SC_413.html";
        _err_page[414] = "./eva/error_pages/ep_SC_414.html";
        _err_page[415] = "./eva/error_pages/ep_SC_415.html";
        _err_page[500] = "./eva/error_pages/ep_SC_500.html";
        _err_page[501] = "./eva/error_pages/ep_SC_501.html";
        _err_page[502] = "./eva/error_pages/ep_SC_502.html";
        _err_page[505] = "./eva/error_pages/ep_SC_505.html";
    }
    ~LocationSet() {}
};


struct ServerSet
{
    std::vector<LocationSet>    _locations;
    std::set<std::string>       _server_name;
    std::string                 _address;
    int                         _port;
    ServerSet() : _address("0.0.0.0"), _port(8080) {}
    ~ServerSet() {}
};

struct Response
{
    std::string _head;
    char*       _body;
    size_t      _body_len;
    bool        _is_mapped;
    Response() : _body(nullptr), _body_len(0), _is_mapped(0) 
    {
    }
    ~Response() 
    {
        if (_is_mapped)
            munmap(static_cast<void*>(_body), _body_len);
    }
};

#endif
