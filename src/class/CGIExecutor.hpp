/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecutor.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 22:25:40 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 16:03:36 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

# include <map>
# include <vector>
# include <string>
# include <fcntl.h>
# include <unistd.h>
# include <signal.h>
# include <iostream>
# include "../incl/mini_structs.hpp"
# include "../incl/exceptions.hpp"
# include "../incl/macros.hpp"

class CGIExecutor
{
    std::map<int, std::string>  _env;
    std::map<int, std::string>  _args;
    std::string*                _body;
    
    public:
        CGIExecutor(std::vector<std::string>&, LocationSet&);
        ~CGIExecutor();
        void set_body(std::string *);
        int execute_cgi(void);
        static bool pass_input_to_cgi(std::string& input, size_t &wlen, int fd);
        static void wait_for_cgi(void);
};


#endif
