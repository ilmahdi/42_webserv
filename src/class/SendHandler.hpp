/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 12:07:28 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/12 11:36:08 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEND_HANDLER_HPP
#define SEND_HANDLER_HPP

# include <vector>
# include <iostream>
# include <sys/socket.h>
# include "../incl/utils.hpp"
# include "../incl/mini_structs.hpp"
# include "../class/SendHandler.hpp"

#include <unistd.h>

class SendHandler
{
    bool        _is_done;
    bool        _is_body;
    bool        _is_close;
    size_t      _wlength;
    Response*   _response;
    const char* _buf_switch;
    size_t      _buf_switch_len;
    
    public:
        SendHandler();  
        ~SendHandler();
        void operator()(int fd, int ndata);
        void set_response(Response*);
        bool get_is_done(void);
        bool get_is_close(void);
        void set_is_done(bool);
        void set_is_close(bool);
};

#endif
