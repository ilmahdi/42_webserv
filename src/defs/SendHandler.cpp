/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 12:07:14 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 17:05:53 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/SendHandler.hpp"

/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/

SendHandler::SendHandler() : _is_done(0), _is_body(0), _wlength(0)
{
}
SendHandler::~SendHandler()
{
}

/******************************************************************************/
/*                              getters-setters                               */
/******************************************************************************/

void SendHandler::set_response(Response* sv)
{
    _response = sv;
    _buf_switch = _response->_head.data();
    _buf_switch_len = _response->_head.length();
}
bool SendHandler::get_is_done(void)
{
    return (_is_done);
}
void SendHandler::set_is_done(bool _sv)
{
    _is_done = _sv;
}
bool SendHandler::get_is_close(void)
{
    return (_is_close);
}
void SendHandler::set_is_close(bool _sv)
{
    _is_close = _sv;
}
/******************************************************************************/
/*                                 fonctor                                    */
/******************************************************************************/

void SendHandler::operator()(int fd, int ndata)
{
    int wdata = 0;
    
    do
    {
        _wlength += wdata = ::send(fd, (void *)(&_buf_switch[_wlength]), _buf_switch_len - _wlength, 0);
        if (wdata == -1) 
            throw server_error(std::string("error: send: ") + ::strerror(errno));
        if (wdata == 0) 
            throw server_error(std::string("send: connection closed by client."));
        ndata -= wdata;
    } while (_wlength < _buf_switch_len && ndata > 0);

    if (!_is_body && _wlength == _buf_switch_len && _response->_body_len)
    {
        _buf_switch = _response->_body;
        _buf_switch_len = _response->_body_len;
        _wlength = 0;
        _is_body = true;
    }
    else if (_wlength == _buf_switch_len)
        _is_done = true;
}
