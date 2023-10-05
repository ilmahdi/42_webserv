/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_response.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/15 21:42:10 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 15:56:12 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../incl/utils.hpp"
#include "../incl/macros.hpp"


std::map<std::string, std::string> mime_map;

bool init_mime_map(std::map<std::string, std::string> &mime_map_)
{
        mime_map_["html"] = "text/html";
        mime_map_["htm"] = "text/html";
        mime_map_["shtml"] = "text/html";
        mime_map_["css"] = "text/css";
        mime_map_["xml"] = "text/xml";
        mime_map_["gif"] = "image/gif";
        mime_map_["jpeg"] = "image/jpeg";
        mime_map_["jpg"] = "image/jpeg";
        mime_map_["js"] = "application/javascript";
        mime_map_["atom"] = "application/atom+xml";
        mime_map_["rss"] = "application/rss+xml";

        mime_map_["mml"] = "text/mathml";
        mime_map_["txt"] = "text/plain";
        mime_map_["jad"] = "text/vnd.sun.j2me.app-descriptor";
        mime_map_["wml"] = "text/vnd.wap.wml";
        mime_map_["htc"] = "text/x-component";

        mime_map_["png"] = "image/png";
        mime_map_["svg"] = "image/svg+xml";
        mime_map_["svgz"] = "image/svg+xml";
        mime_map_["tif"] = "image/tiff";
        mime_map_["tiff"] = "image/tiff";
        mime_map_["wbmp"] = "image/vnd.wap.wbmp";
        mime_map_["webp"] = "image/webp";
        mime_map_["ico"] = "image/x-icon";
        mime_map_["jng"] = "image/x-jng";
        mime_map_["bmp"] = "image/x-ms-bmp";

        mime_map_["woff"] = "font/woff";
        mime_map_["woff2"] = "font/woff2";

        mime_map_["jar"] = "application/java-archive";
        mime_map_["war"] = "application/java-archive";
        mime_map_["ear"] = "application/java-archive";
        mime_map_["json"] = "application/json";
        mime_map_["hqx"] = "application/mac-binhex40";
        mime_map_["doc"] = "application/msword";
        mime_map_["pdf"] = "application/pdf";
        mime_map_["ps"] = "application/postscript";
        mime_map_["eps"] = "application/postscript";
        mime_map_["ai"] = "application/postscript";
        mime_map_["rtf"] = "application/rtf";
        mime_map_["m3u8"] = "application/vnd.apple.mpegurl";
        mime_map_["kml"] = "application/vnd.google-earth.kml+xml";
        mime_map_["kmz"] = "application/vnd.google-earth.kmz";
        mime_map_["xls"] = "application/vnd.ms-excel";
        mime_map_["eot"] = "application/vnd.ms-fontobject";
        mime_map_["ppt"] = "application/vnd.ms-powerpoint";
        mime_map_["odg"] = "application/vnd.oasis.opendocument.graphics";
        mime_map_["odp"] = "application/vnd.oasis.opendocument.presentation";
        mime_map_["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
        mime_map_["odt"] = "application/vnd.oasis.opendocument.text";

        mime_map_["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        mime_map_["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        mime_map_["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

        mime_map_["wmlc"] = "application/vnd.wap.wmlc";
        mime_map_["7z"] = "application/x-7z-compressed";
        mime_map_["cco"] = "application/x-cocoa";
        mime_map_["jardiff"] = "application/x-java-archive-diff";
        mime_map_["jnlp"] = "application/x-java-jnlp-file";
        mime_map_["run"] = "application/x-makeself";
        mime_map_["pl"] = "application/x-perl";
        mime_map_["pm"] = "application/x-perl";
        mime_map_["prc"] = "application/x-pilot";
        mime_map_["pdb"] = "application/x-pilot";
        mime_map_["rar"] = "application/x-rar-compressed";
        mime_map_["rpm"] = "application/x-redhat-package-manager";
        mime_map_["sea"] = "application/x-sea";
        mime_map_["swf"] = "application/x-shockwave-flash";
        mime_map_["sit"] = "application/x-stuffit";
        mime_map_["tcl"] = "application/x-tcl";
        mime_map_["tk"] = "application/x-tcl";
        mime_map_["der"] = "application/x-x509-ca-cert";
        mime_map_["pem"] = "application/x-x509-ca-cert";
        mime_map_["crt"] = "application/x-x509-ca-cert";
        mime_map_["xpi"] = "application/x-xpinstall";
        mime_map_["xhtml"] = "application/xhtml+xml";
        mime_map_["xspf"] = "application/xspf+xml";
        mime_map_["zip"] = "application/zip";

        mime_map_["bin"] = "application/octet-stream";
        mime_map_["exe"] = "application/octet-stream";
        mime_map_["dll"] = "application/octet-stream";
        mime_map_["deb"] = "application/octet-stream";
        mime_map_["dmg"] = "application/octet-stream";
        mime_map_["iso"] = "application/octet-stream";
        mime_map_["img"] = "application/octet-stream";
        mime_map_["msi"] = "application/octet-stream";
        mime_map_["msp"] = "application/octet-stream";
        mime_map_["msm"] = "application/octet-stream";

        mime_map_["mid"] = "audio/midi";
        mime_map_["midi"] = "audio/midi";
        mime_map_["kar"] = "audio/midi";
        mime_map_["mp3"] = "audio/mpeg";
        mime_map_["ogg"] = "audio/ogg";
        mime_map_["m4a"] = "audio/x-m4a";
        mime_map_["ra"] = "audio/x-realaudio";

        mime_map_["3gpp"] = "video/3gpp";
        mime_map_["3gp"] = "video/3gpp";
        mime_map_["ts"] = "video/mp2t";
        mime_map_["mp4"] = "video/mp4";
        mime_map_["mpeg"] = "video/mpeg";
        mime_map_["mpg"] = "video/mpeg";
        mime_map_["mov"] = "video/quicktime";
        mime_map_["webm"] = "video/webm";
        mime_map_["flv"] = "video/x-flv";
        mime_map_["m4v"] = "video/x-m4v";
        mime_map_["mng"] = "video/x-mng";
        mime_map_["asx"] = "video/x-ms-asf";
        mime_map_["asf"] = "video/x-ms-asf";
        mime_map_["wmv"] = "video/x-ms-wmv";
        mime_map_["avi"] = "video/x-msvideo";
        return (1);
}

bool dummy3 = init_mime_map(mime_map);

std::string get_content_type(const char *file)
{
    std::string ext;

    ext = get_file_extention(std::string(file));
    if (!ext.empty())
    {
        std::map<std::string, std::string>::iterator it;

        it = mime_map.find(ext);
        if (it != mime_map.end())
            return (it->second);
    }
    return (std::string());
}

void build_header(std::string &header, std::vector<std::string> &s_fields)
{
    // header.reserve(1024);
    header.assign("HTTP/1.1 ");
    header.append(s_fields[HS_STCODE]);
    header.append("\r\n");
    header.append("Server: nginy/1.33.7\r\n");
    header.append("Date: ");
    header.append(get_date());
    header.append("\r\n");
    if (!s_fields[HS_CTYP].empty())
    {
        header.append("Content-Type: ");
        header.append(s_fields[HS_CTYP]);
        header.append("\r\n");
    }
    if (!s_fields[HS_CNTLEN].empty())
    {
        header.append("Content-Length: ");
        header.append(s_fields[HS_CNTLEN]);
        header.append("\r\n");
    }
    if (!s_fields[HS_LOCATN].empty())
    {
        header.append("Location: ");
        header.append(s_fields[HS_LOCATN]);
        header.append("\r\n");
    }
    if (!s_fields[HS_ALLOWD].empty())
    {
        header.append("Allow: ");
        header.append(s_fields[HS_ALLOWD]);
        header.append("\r\n");
    }
    if (!s_fields[HS_CONNECT].empty())
    {
        header.append("Connection: ");
        header.append(s_fields[HS_CONNECT]);
        header.append("\r\n");
    }
    if (!s_fields[HS_LTMODIF].empty())
    {
        header.append("Last-Modified: ");
        header.append(s_fields[HS_LTMODIF]);
        header.append("\r\n");
    }
    if (!s_fields[HS_ETAG].empty())
    {
        header.append("ETag: ");
        header.append(s_fields[HS_ETAG]);
        header.append("\r\n");
    }
    header.append(s_fields[HS_LCRLF]);
    if (!s_fields[HS_AUTONDX].empty())
        header.append(s_fields[HS_AUTONDX]);
}

void build_body_error(Response &response, std::vector<std::string> &s_fields, std::map<int, std::string> &err_page)
{
    std::string &file = err_page[::strtoul(s_fields[HS_STCODE].substr(0, 3).data(), nullptr, 10)];
    
    if (file.empty())
        return;
    mmap_file(response, file.data());
    s_fields[HS_CNTLEN] = to_str(response._body_len);
    s_fields[HS_CTYP] = get_content_type(file.data());
}

size_t get_pos_string(const char *str, size_t slen, const char *tof)
{
    for (size_t i = 0; i < slen; ++i)
    {
        size_t j = 0;
        size_t p = i;
        while (str[i++] == tof[j++])
        {
            if (!tof[j])
                return (p + j);
        }
        i = p;
    }
    return (std::string::npos);
}

void mmap_file(Response &response, const char *file)
{
    int         fd;
    struct stat st;
    
    fd = open(file, O_RDONLY);
    if (fd == -1) 
        throw server_error(std::string("error: open: ") + ::strerror(errno));
    if (fstat(fd, &st) < 0)
    {
        close(fd);
        throw server_error(std::string("error: fstat: ") + ::strerror(errno));
    }
    response._body_len = st.st_size;
    if (response._body_len)
    {
        response._body = (char*)mmap(NULL, response._body_len, PROT_READ, MAP_PRIVATE, fd, 0);
        if (response._body == MAP_FAILED)
        {
            close(fd);
            throw server_error(std::string("error: mmap: ") + ::strerror(errno));
        }
        response._is_mapped = true;
    }
    close(fd);
}

void get_status_from_cgi_response(const std::string &cgi_head, std::string &s_status)
{
    size_t pos = cgi_head.find("Status:");

    if (pos != std::string::npos)
    {
        pos = cgi_head.find_first_not_of(" ", pos + 7);
        s_status = cgi_head.substr(pos, cgi_head.find_first_of("\r\n") - pos);
    }
}
