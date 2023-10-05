/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 16:59:26 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 17:06:38 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/RequestParser.hpp"

/******************************************************************************/
/*                          init static variables                             */
/******************************************************************************/
typedef void (*handler)(std::vector<std::string>&, const std::string&);
std::map<std::string, handler>  RequestParser::_assign_handl;
std::set<std::string>           RequestParser::_methods_list;

bool init_assign_handl(std::map<std::string, handler> &vmap)
{
    vmap["host"] = RequestParser::host_handle;
    vmap["content-type"] = RequestParser::ctyp_handle;
    vmap["content-length"] = RequestParser::clen_handle;
    vmap["transfer-encoding"] = RequestParser::tencod_handle;
    vmap["connection"] = RequestParser::connect_handle;
    vmap["if-modified-since"] = RequestParser::if_modif_handle;
    vmap["if-none-match"] = RequestParser::if_nmatch_handle;
    vmap["cookie"] = RequestParser::cookie_handle;
    return (1);
}
bool init_methods_listl(std::set<std::string> &vset)
{
    vset.insert("GET");
    vset.insert("POST");
    vset.insert("DELETE");
    return (1);
}
bool dummy1 = init_assign_handl(RequestParser::_assign_handl);
bool dummy2 = init_methods_listl(RequestParser::_methods_list);
/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/
RequestParser::RequestParser(std::string* _cv1, std::vector<std::string>* _cv2)
    : _header(_cv1), _r_fields(_cv2)
{
}
RequestParser::~RequestParser()
{
}

/******************************************************************************/
/*                              getters-setters                               */
/******************************************************************************/

void RequestParser::set_header(std::string* _sv)
{
    _header = _sv;
}
void RequestParser::set_r_fields(std::vector<std::string>* _sv)
{
    _r_fields = _sv;   
}

/******************************************************************************/
/*                            assign handle methods                           */
/******************************************************************************/

void RequestParser::host_handle(std::vector<std::string> &fileds, const std::string &header)
{
    std::string word;

    get_first_word(header, word);
    if (is_empty(word))
        throw response_status(SC_400);
    fileds[HR_HOST] = word;
}

void RequestParser::tencod_handle(std::vector<std::string> &fileds, const std::string &header)
{
    std::string word;
    
    get_first_word(header, word);
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    if (word != "chunked")
        throw response_status(SC_415);
    fileds[HR_TENCOD] = word;
}

void RequestParser::clen_handle(std::vector<std::string> &fileds, const std::string &header)
{
    std::string word;
    
    get_first_word(header, word);
    if (word.find_first_not_of("0123456789") != std::string::npos || is_empty(word))
        throw response_status(SC_400);
    fileds[HR_CNTLEN] = word;
}

void RequestParser::ctyp_handle(std::vector<std::string> &fileds, const std::string &header)
{
    size_t      pos;

    pos = header.find_first_not_of(" ");
    fileds[HR_CTYP] = header.substr(pos, header.find_first_of("\r\n") - pos);
}

void RequestParser::connect_handle(std::vector<std::string> &fileds, const std::string &header)
{
    std::string word;
    
    get_first_word(header, word);
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    fileds[HR_CONNECT] = word;
}
void RequestParser::if_modif_handle(std::vector<std::string> &fileds, const std::string &header)
{
    size_t      pos;

    pos = header.find_first_not_of(" ");
    fileds[HR_IFMODIF] = header.substr(pos, header.find_first_of("\r\n") - pos);
}
void RequestParser::if_nmatch_handle(std::vector<std::string> &fileds, const std::string &header)
{
    size_t      pos1;
    size_t      pos2;

    pos1 = header.find_first_of("\"");
    pos2 = header.find_last_of("\"");

    if (pos1 != std::string::npos && pos2 != std::string::npos)
        fileds[HR_ETAG] = header.substr(pos1 + 1, pos2 - pos1 - 1);
}
void RequestParser::cookie_handle(std::vector<std::string> &fileds, const std::string &header)
{
    size_t      pos;

    pos = header.find_first_not_of(" ");
    fileds[HR_COOKIE] = header.substr(pos, header.find_first_of("\r\n") - pos);
}
/******************************************************************************/
/*                           header parsing functions                         */
/******************************************************************************/
void RequestParser::parse_first_line(void)
{
    std::vector<std::string> str_sq;
    get_sequence_str_v2(*_header, str_sq);
    if (str_sq.size() != 3)
        throw response_status(SC_400);
    if (_methods_list.find(str_sq[0]) == _methods_list.end())
        throw response_status(SC_405);
    if (str_sq[1].length() > URL_LIMIT)
        throw response_status(SC_414);
    if (str_sq[2] != "HTTP/1.1")
        throw response_status(SC_505);
    (*_r_fields)[HR_METHOD] = str_sq[0];
    (*_r_fields)[HR_URL] = str_sq[1];
    
    size_t pos = (*_r_fields)[HR_URL].find("%20");

    while (pos != std::string::npos)
    {
       (*_r_fields)[HR_URL].erase(pos, 3);
       (*_r_fields)[HR_URL].insert(pos, 1, ' ');
       pos = (*_r_fields)[HR_URL].find("%20", pos - 2);
    }
}

void RequestParser::parse_header(void)
{
    for (size_t i = 0; (*_header)[i]; ++i)
    {
        size_t p1 = _header->find_first_not_of(" ", i);
        size_t p2 = _header->find_first_of(":\r\n", p1);
        
        i = p2;
        if ((*_header)[p2] == ':')
        {
            std::map<std::string, handler>::iterator it;
            std::string str  = _header->substr(p1, p2 - p1);
            
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            it = _assign_handl.find(str);
            if (it != _assign_handl.end())
                it->second(*_r_fields, std::string(&(*_header)[p2 + 1]));
            i = _header->find_first_of("\n", p2);
        }
    }
    if ((*_r_fields)[HR_METHOD] == "POST")
        if ((*_r_fields)[HR_CNTLEN].empty() && (*_r_fields)[HR_TENCOD].empty())
            throw response_status(SC_400);
}
