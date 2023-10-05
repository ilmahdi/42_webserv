/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyUnchunker.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 10:14:55 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 15:56:25 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/BodyUnchunker.hpp"

/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/
BodyUnchunker::BodyUnchunker(std::string *_cv) : _case(0), _is_done(0), _remain_len(0),
                                        _begin_h(0), _chunk_len(0), _rbody(_cv)
{ 
}

BodyUnchunker::~BodyUnchunker()
{   
}

/******************************************************************************/
/*                                case-handlers                               */
/******************************************************************************/

void BodyUnchunker::extract_chunk_len(std::string& chunk)
{
    size_t      pos;
    std::string chunk_len_str;
    
    pos = chunk.find_first_of("\n", _begin_h);
    if (pos == std::string::npos)
        throw return_unvalid();
    chunk_len_str = chunk.substr(_begin_h, pos - _begin_h);
    if (chunk_len_str[chunk_len_str.size() - 1] == '\r')
        chunk_len_str.pop_back();
    _chunk_len = ::strtoul(chunk_len_str.data(), nullptr, 16);
    if (!_chunk_len)
    {
        _is_done = true;
        throw return_unvalid();
    }
    _begin_h = pos + 1;
    _case = 1;
}

void BodyUnchunker::extract_chunk_str(std::string& chunk)
{
    if (chunk.length() < _begin_h + _chunk_len)
    {
        _remain_len = _begin_h + _chunk_len - chunk.length();
        _chunk_len = chunk.length() - _begin_h;
    }
        
    _rbody->append(chunk.begin() + _begin_h, chunk.begin() + _begin_h + _chunk_len);
    _begin_h += _chunk_len;
    _case = 2;
}

void BodyUnchunker::skip_delemiter(std::string& chunk)
{
    size_t  pos;

    pos = chunk.find_first_of("\n", _begin_h);
    if (pos == std::string::npos)
        throw return_unvalid();
    if (pos - _begin_h > 1)
        throw response_status(SC_404);
    _begin_h = pos + 1;
    _case = 0;
}


void BodyUnchunker::append_remain_str(std::string& chunk)
{
    _chunk_len = _remain_len;
    _remain_len = 0;
    if (chunk.length() < _chunk_len)
    {
        _remain_len = _chunk_len - chunk.length();
        _chunk_len = chunk.length();
    }
    _rbody->append(chunk.begin(), chunk.begin() + _chunk_len);
    _begin_h = _chunk_len;
    _case = 2;
}

/******************************************************************************/
/*                                parse chunked                               */
/******************************************************************************/
int BodyUnchunker::parse_chunked_body(std::string& chunk)
{
    try
    {
        _begin_h = 0;
        if (_remain_len)
            append_remain_str(chunk);
        while (_begin_h < chunk.length())
        {
            switch (_case)
            {
                case 0:
                    extract_chunk_len(chunk);
                    break;
                case 1:
                    extract_chunk_str(chunk);
                    break;
                case 2:
                    skip_delemiter(chunk);
                    break;
            }
        }
    }
    catch(const std::exception& e)
    {
        if (_is_done)
            return (-1);
        else
            return (_begin_h);
    }
    return (-2);
    
}
