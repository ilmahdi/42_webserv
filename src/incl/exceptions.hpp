/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 17:34:06 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 17:25:14 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

# include <string>
# include <exception>
# include <sys/errno.h>

class failed_open : public std::exception
{
    public:
        const char *what() const throw();
};

class empty_config_file : public std::exception
{
    public:
        const char *what() const throw();
};

class unclosed_block : public std::exception
{
    public:
        const char *what() const throw();
};

class not_srever_block : public std::exception
{
    public:
        const char *what() const throw();
};

class unexpected_token : public std::exception
{
    std::string msg;

    public:
        const char *what() const throw();
        unexpected_token(std::string &_msg);
        ~unexpected_token() throw();
};

class err_token_args : public std::exception
{
    public:
        const char *what() const throw();
};

class err_strerror : public std::exception
{
    std::string msg;

    public:
        const char *what() const throw();
        err_strerror(std::string _msg);
        ~err_strerror() throw();
};

class response_status : public std::exception
{
    std::string msg;

    public:
        const char *what() const throw();
        response_status(std::string _msg);
        ~response_status() throw();
};

class server_error : public std::exception
{
    std::string msg;

    public:
        const char *what() const throw();
        server_error(std::string _msg);
        ~server_error() throw();
};

class return_unvalid : public std::exception
{
    public:
        const char *what() const throw();
};


#endif
