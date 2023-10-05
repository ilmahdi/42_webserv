/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyUnchunker.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 10:11:57 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/12 10:55:22 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BODY_UNCHUNKER_HPP
#define BODY_UNCHUNKER_HPP

# include <vector>
# include <string>
# include "../incl/macros.hpp"
# include "../incl/exceptions.hpp"

class BodyUnchunker
{   
    int             _case;
    bool            _is_done;
    size_t          _remain_len;
    size_t          _begin_h;
    size_t          _chunk_len;
    std::string*    _rbody;
    
    public:
        BodyUnchunker(std::string*);
        ~BodyUnchunker();
        void skip_delemiter(std::string&);
        void extract_chunk_len(std::string&);
        void extract_chunk_str(std::string&);
        void append_remain_str(std::string&);
        int parse_chunked_body(std::string&);
};

#endif
