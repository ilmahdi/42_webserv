/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2023/01/12 14:57:47 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

# include <map>
# include <set>
# include <vector>
# include <string>
# include <iostream>
# include "../incl/macros.hpp"
# include "../incl/utils.hpp"
# include "../incl/exceptions.hpp"

class RequestParser
{
    public:
        typedef void (*handler)(std::vector<std::string>&, const std::string&);
        static std::map<std::string, handler>   _assign_handl;
        static std::set<std::string>            _methods_list;
        
    private:
        std::string*                _header;
        std::vector<std::string>*   _r_fields;
    
    public:
        RequestParser(std::string*, std::vector<std::string>*);
        ~RequestParser();
        void set_header(std::string*);
        void set_r_fields(std::vector<std::string>*);
        void parse_header(void);
        void parse_first_line(void);
        static void host_handle(std::vector<std::string> &, const std::string &);
        static void ctyp_handle(std::vector<std::string> &, const std::string &);
        static void clen_handle(std::vector<std::string> &, const std::string &);
        static void tencod_handle(std::vector<std::string> &, const std::string &);
        static void connect_handle(std::vector<std::string> &, const std::string &);
        static void if_modif_handle(std::vector<std::string> &, const std::string &);
        static void if_nmatch_handle(std::vector<std::string> &, const std::string &);
        static void cookie_handle(std::vector<std::string> &, const std::string &);
};

#endif
