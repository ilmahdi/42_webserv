/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/21 21:42:54 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 17:26:21 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/ConfigParser.hpp"

/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/

ConfigParser::ConfigParser(const char* _s, std::vector<ServerSet>& _v) 
    : _config_file(_s), _server_set(_v)
{
    if (_config_file.fail())
        throw failed_open();

    _assign_handl_server["listen"] = &ConfigParser::listen_handle;
    _assign_handl_server["server_name"] = &ConfigParser::name_handle;
    _assign_handl_location["cgi"] = &ConfigParser::cgi_handle;
    _assign_handl_location["root"] = &ConfigParser::root_handle;
    _assign_handl_location["index"] = &ConfigParser::index_handle;
    _assign_handl_location["redirect"] = &ConfigParser::redirect_handle;
    _assign_handl_location["autoindex"] = &ConfigParser::autoindex_handle;
    _assign_handl_location["error_page"] = &ConfigParser::err_page_handle;
    _assign_handl_location["upload_dir"] = &ConfigParser::upload_dir_handle;
    _assign_handl_location["accepted_methods"] = &ConfigParser::acc_mtod_handle;
    _assign_handl_location["client_body_max_size"] = &ConfigParser::cb_max_size_handle;
}

ConfigParser::~ConfigParser()
{
}

/******************************************************************************/
/*                           server handlers methods                          */
/******************************************************************************/
void ConfigParser::listen_handle(std::string &line)
{
    std::vector<std::string>    sq_str;
    std::string                 str_hol;
    size_t                      rc1 = 0;
    size_t                      rc2;

    get_sequence_str(line, sq_str);
    if (sq_str.size() != 1)
        throw err_token_args();
    rc2 = sq_str[0].find(":");
    if (rc2 != std::string::npos)
    {
        str_hol = sq_str[0].substr(0, rc2);
        if (str_hol.empty())
            throw err_token_args();
        _server_set.back()._address = str_hol;
        rc1 = rc2 + 1;
    }
    rc2 = sq_str[0].find(";", rc1);
    str_hol = sq_str[0].substr(rc1, rc2 - rc1);
    if (str_hol.empty())
        throw err_token_args();
    if (str_hol.find(".") != std::string::npos)
        _server_set.back()._address = str_hol;
    else
    {
        if (str_hol.find_first_not_of("0123456789") != std::string::npos)
            throw err_token_args();
        _server_set.back()._port = ::strtoul(str_hol.data(), nullptr, 10);
    }
}
void ConfigParser::name_handle(std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (!sq_str.size())
        throw err_token_args();
    _server_set.back()._server_name.insert(sq_str.begin(), sq_str.end());
}
/******************************************************************************/
/*                           location handlers methods                        */
/******************************************************************************/
void ConfigParser::cgi_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;
    
    get_sequence_str(line, sq_str);
    if (sq_str.size() != 2)
        throw err_token_args();
    location_set._cgi[sq_str[0]] = sq_str[1];
}
void ConfigParser::err_page_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;
    
    get_sequence_str(line, sq_str);
    if (sq_str.size() != 2)
        throw err_token_args();
    if (sq_str[0].find_first_not_of("0123456789") != std::string::npos)
        throw err_token_args();
    int tmp = ::strtoul(sq_str[0].data(), nullptr, 10);
    location_set._err_page[tmp] = sq_str[1];
}
void ConfigParser::redirect_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (sq_str.size() != 2)
        throw err_token_args();
        
    if (sq_str[1].find_first_not_of("0123456789") != std::string::npos)
        throw err_token_args();
    if (sq_str[0][sq_str[0].length() - 1] != '/')
        sq_str[0].push_back('/');
    location_set._redirect = make_pair(sq_str[0], ::strtoul(sq_str[1].data(), nullptr, 10));
}
void ConfigParser::index_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (!sq_str.size())
        throw err_token_args();
    location_set._index.insert(sq_str.begin(), sq_str.end());
}
void ConfigParser::acc_mtod_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (!sq_str.size())
        throw err_token_args();
    location_set._acc_mtod.insert(sq_str.begin(), sq_str.end());
}
void ConfigParser::root_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (sq_str.size() != 1)
        throw err_token_args();
    location_set._root = sq_str[0];
}
void ConfigParser::upload_dir_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (sq_str.size() != 1)
        throw err_token_args();
    location_set._upload_dir = sq_str[0];
}
void ConfigParser::cb_max_size_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string>    sq_str;
    size_t                      value;
    char                        c;

    get_sequence_str(line, sq_str);
    if (sq_str.size() != 1)
        throw err_token_args();
    value = 1;
    c = static_cast<char>(tolower(sq_str[0].back()));
    if (strchr("kmg", c) != NULL)
    {
        sq_str[0].pop_back();
        switch (c) {
            case 'k':
                value *= 1024;
                break;
            case 'm':
                value *= 1024 * 1024;
                break;
            case 'g':
                value *= 1024 * 1024 * 1024;
                break;
        }
    }
    if (sq_str[0].find_first_not_of("0123456789") != std::string::npos)
        throw err_token_args();
    location_set._cb_max_size = ::strtoul(sq_str[0].data(), nullptr, 10) * value;
    
}
void ConfigParser::autoindex_handle(LocationSet& location_set, std::string &line)
{
    std::vector<std::string> sq_str;

    get_sequence_str(line, sq_str);
    if (sq_str.size() != 1)
        throw err_token_args();
    std::transform(sq_str[0].begin(), sq_str[0].end(), sq_str[0].begin(), ::tolower);
    if (sq_str[0] == "on")
        location_set._autoindex = true;
    else if (sq_str[0] == "off")
        location_set._autoindex = false;
    else
        throw err_token_args();
}

void ConfigParser::url_path_handle(std::string &line_hol)
{
    std::string                 line;
    std::vector<std::string>    sq_str;

    while (std::getline(_config_file, line) || !is_empty(line_hol))
    {
        if ((is_empty(line) && _config_file.good()) || is_comment(line))
            continue;
        line_hol += line;
        size_t r_code = line_hol.find("{");
        if (r_code != std::string::npos)
        {
            line = line_hol.substr(0, r_code + 1);
            line_hol = &line_hol[r_code + 1];
            line[r_code] = ';';
            get_sequence_str(line, sq_str);
            if (sq_str.size() != 1)
                throw err_token_args();
            // if (sq_str[0][sq_str[0].length() - 1] != '/')
            //     sq_str[0].push_back('/');
            _server_set.back()._locations.back()._url_path = sq_str[0];
            return;
        }
    }
    throw err_token_args();
}
/******************************************************************************/
/*                              parse config methods                          */
/******************************************************************************/

void ConfigParser::main_config_file_parser(void)
{
    bool        is_block;
    std::string line_hol;
    std::string line;
    
    is_block = false;
    while (std::getline(_config_file, line) || !is_empty(line_hol))
    {
        if ((is_empty(line) && _config_file.good()) || is_comment(line))
            continue;
        line_hol += line;
        while (1)
        {
            if (is_block)
            {
                is_block = false;
                end_block(line_hol);
                if (is_empty(line_hol))
                    break;
            }
            if (line_hol.find("{") == std::string::npos)
            {
                if (!_config_file.good())
                    throw not_srever_block();
                break;
            }
            size_t rc = is_str_start_with(line_hol, "server{");
            if (rc)
            {
                line_hol = &line_hol[rc];
                server_block_parser(line_hol);
                is_block = true;
            }
            else
                throw unexpected_token(line_hol);
        }
    }
    if (_server_set.empty())
        throw empty_config_file();
}

void ConfigParser::server_block_parser(std::string &line_hol)
{
    _server_set.push_back(ServerSet());
    _server_set.back()._locations.push_back(LocationSet());
    bool        is_block;
    std::string key_word;
    std::string line;
    
    is_block = false;
    while (std::getline(_config_file, line) || !is_empty(line_hol))
    {   
        if ((is_empty(line) && _config_file.good()) || is_comment(line))
            continue;
        line_hol += line;
        while (1)
        {
            if (is_block)
                end_block(line_hol);
            is_block = false;
            if (is_str_start_with(line_hol, "}"))
                return ;
            
            size_t rc = is_str_start_with(line_hol, "location");
            if (rc)
            {
                location_block_parser(line_hol);
                is_block = true;
                continue;
            }
            rc = line_hol.find(";");
            if (rc == std::string::npos)
                break;

            get_first_word(line_hol, key_word);
            try
            {
                (this->*_assign_handl_server.at(key_word))(line_hol);
                line_hol = &line_hol[rc + 1];
            }
            catch(const std::exception& e)
            {
                location_block_main_parser(line_hol);
            }
        }
    }
}

void ConfigParser::location_block_main_parser(std::string &line_hol)
{
    bool is_block;
    std::string key_word;
    std::string line;

    is_block = false;
    while (std::getline(_config_file, line) || !is_empty(line_hol))
    {
        if ((is_empty(line) && _config_file.good()) || is_comment(line))
            continue;
        line_hol += line;
        while (1)
        {
            if (is_block)
                end_block(line_hol);
            is_block = false;
            if (is_str_start_with(line_hol, "}"))
                return;
            size_t rc = is_str_start_with(line_hol, "location");
            if (rc)
            {
                location_block_parser(line_hol);
                is_block = true;
                continue;
            }
            rc = line_hol.find(";");
            if (rc == std::string::npos)
                break;
            get_first_word(line_hol, key_word);
            try
            {
                (this->*_assign_handl_location.at(key_word))(_server_set.back()._locations.front(), line_hol);
                line_hol = &line_hol[rc + 1];
            }
            catch (const std::exception &e)
            {
                throw unexpected_token(key_word);
            }
        }
    }
}

void ConfigParser::location_block_parser(std::string &line_hol)
{
    _server_set.back()._locations.push_back(_server_set.back()._locations.front());
    std::string key_word;
    std::string line;

    url_path_handle(line_hol);

    while (std::getline(_config_file, line) || !is_empty(line_hol))
    {
        if ((is_empty(line) && _config_file.good()) || is_comment(line))
            continue;
        line_hol += line;
        while (1)
        {
            if (is_str_start_with(line_hol, "}"))
                return /*(location_set._complet_location(_s_data.location.front()), (void)0)*/;
            size_t rc = line_hol.find(";");
            if (rc == std::string::npos)
            {
                rc = line_hol.find_first_of("{}");
                if (rc != std::string::npos || !_config_file.good())
                {
                    get_first_word(line_hol, key_word);
                    throw unexpected_token(key_word);
                }
                break;
            }
            get_first_word(line_hol, key_word);
            try
            {
                (this->*_assign_handl_location.at(key_word))(_server_set.back()._locations.back(), line_hol);
                line_hol = &line_hol[rc + 1];
            }
            catch (const std::exception &e)
            {
                throw unexpected_token(key_word);
            }
        }
    }
}

/******************************************************************************/
/*                                  display method                            */
/******************************************************************************/

void ConfigParser::print(void)
{
    for (size_t i = 0; i < _server_set.size(); ++i)
    {
        std::cout << "address: " << _server_set[i]._address << '\n';
        std::cout << "port: " << _server_set[i]._port << '\n';
        std::cout << "server names: ";
        for (std::set<std::string>::iterator it = _server_set[i]._server_name.begin(); it != _server_set[i]._server_name.end(); ++it)
            std::cout << *it << ' ';
        std::cout << '\n';
        std::cout << "------------------------------------------------------\n";
        std::cout << "------------------------------------------------------\n";
        for (size_t j = 0; j < _server_set[i]._locations.size(); ++j)
        {
            std::cout << "url_path: " << _server_set[i]._locations[j]._url_path << '\n';
            std::cout << "autoindex: " << std::boolalpha << _server_set[i]._locations[j]._autoindex << '\n';
            std::cout << "cb_max_size: " << _server_set[i]._locations[j]._cb_max_size << '\n';
            std::cout << "root: " << _server_set[i]._locations[j]._root << '\n';
            std::cout << "acc_mtod: ";
            for (std::set<std::string>::iterator it = _server_set[i]._locations[j]._acc_mtod.begin(); it != _server_set[i]._locations[j]._acc_mtod.end(); ++it)
                std::cout << *it << ' ';
            std::cout << '\n';
            std::cout << "index: ";
            for (std::set<std::string>::iterator it = _server_set[i]._locations[j]._index.begin(); it != _server_set[i]._locations[j]._index.end(); ++it)
                std::cout << *it << ' ';
            std::cout << '\n';
            std::cout << "error page: ";
            std::map<int, std::string>::iterator it = _server_set[i]._locations[j]._err_page.begin();
            for (; it != _server_set[i]._locations[j]._err_page.end(); ++it)
                std::cout << "\n      " << it->first << ' ' << it->second;
        
            std::cout << "\nredirect: ";
                std::cout << "\n      " << _server_set[i]._locations[j]._redirect.first << "  " << _server_set[i]._locations[j]._redirect.second;
            std::cout << '\n';
            std::cout << "------------------------------------------------------\n";
        }
    }
}
