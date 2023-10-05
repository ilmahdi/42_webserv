/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 11:01:10 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 17:26:00 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/exceptions.hpp"

const char *failed_open::what() const throw()
{
    return ("error: failed to open config file!.");
}

const char *empty_config_file::what() const throw()
{
    return ("error: empty config file!.");
}

const char *unclosed_block::what() const throw()
{
    return ("error: unclosed block in config file!.");
}

const char *not_srever_block::what() const throw()
{
    return ("error: missing opening bracket in config file!.");
}


unexpected_token::unexpected_token(std::string &_msg) : msg("error: unexpected token: " + _msg + "!.")
{
}
unexpected_token::~unexpected_token() throw()
{
}
const char *unexpected_token::what() const throw()
{
    return (msg.data());
}

const char *err_token_args::what() const throw()
{
    return ("error: invalid token args in config file!.");
}


err_strerror::err_strerror(std::string _msg) : msg(_msg + ::strerror(errno))
{
}
err_strerror::~err_strerror() throw()
{
}
const char *err_strerror::what() const throw()
{
    return (msg.data());
}


response_status::response_status(std::string _msg) : msg(_msg)
{
}
response_status::~response_status() throw()
{
}
const char *response_status::what() const throw()
{
    return (msg.data());
}


server_error::server_error(std::string _msg) : msg(_msg)
{
}
server_error::~server_error() throw()
{
}
const char *server_error::what() const throw()
{
    return (msg.data());
}

const char *return_unvalid::what() const throw()
{
    return ("");
}