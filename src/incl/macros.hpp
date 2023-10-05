/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 21:39:49 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/12 14:28:25 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MACROS_HPP
#define MACROS_HPP

/******************************************************************************/
/*                               general macros                               */
/******************************************************************************/
#define BUF_SIZE        1024
#define URL_LIMIT       1024
#define NB_OEVENTS      256
#define LISN_BACKLOG    128

/******************************************************************************/
/*                         HTTP request header fields                         */
/******************************************************************************/
#define HR_METHOD   0
#define HR_URL      1
#define HR_RURL     2
#define HR_HOST     3
#define HR_CTYP     4
#define HR_CNTLEN   5
#define HR_TENCOD   6
#define HR_QUERIES  7
#define HR_CONNECT  8
#define HR_IFMODIF  9
#define HR_ETAG     10
#define HR_COOKIE   11

#define HS_STCODE   0
#define HS_CNTLEN   1
#define HS_LOCATN   2
#define HS_ALLOWD   3
#define HS_CTYP     4
#define HS_LCRLF    5
#define HS_CONNECT  6
#define HS_LTMODIF  7
#define HS_AUTONDX  8
#define HS_ETAG     9

/******************************************************************************/
/*                         HTTP response status codes                         */
/******************************************************************************/
#define SC_200 "200 ok"
#define SC_201 "201 Created"
#define SC_204 "204 No Content"
#define SC_301 "301 Moved Permanently"
#define SC_302 "302 Found"
#define SC_304 "304 Not Modified"
#define SC_307 "307 Temporary Redirect"
#define SC_308 "308 Permanent Redirect"
#define SC_400 "400 Bad Request"
#define SC_403 "403 Forbidden"
#define SC_404 "404 Not Found"
#define SC_405 "405 Method Not Allowed"
#define SC_409 "409 Conflict"
#define SC_411 "411 Length Required"
#define SC_413 "413 Payload Too Large"
#define SC_414 "414 URI Too Long"
#define SC_415 "415 Unsupported Media Type"
#define SC_500 "500 Internal Server Error"
#define SC_501 "501 Not Implemented"
#define SC_502 "502 Bad Gateway"
#define SC_505 "505 HTTP Version Not Supported"
/******************************************************************************/
/*                              CGI Variable Name                             */
/******************************************************************************/
#define ENV_CONTENT_TYPE       0   
#define ENV_CONTENT_LENGTH     1
#define ENV_PATH_INFO          2
#define ENV_PATH_TRANSLATED    3
#define ENV_UPLOAD_DIR         4
#define ENV_QUERY_STRING       5
#define ENV_REQUEST_METHOD     6
#define ENV_SCRIPT_FILENAME    7
#define ENV_SCRIPT_NAME        8
#define ENV_SERVER_NAME        9
#define ENV_SERVER_SOFTWARE    10
#define ENV_SERVER_PROTOCOL    11   
#define ENV_GATEWAY_INTERFACE  12     
#define ENV_HTTP_COOKIE        13
#define ENV_REDIRECT_STATUS    14
#define ENV_PHP_INI_SCAN_DIR   15  


#endif
