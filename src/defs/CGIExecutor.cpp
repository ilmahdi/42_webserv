/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecutor.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 22:25:38 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 15:56:23 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/CGIExecutor.hpp"

/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/

CGIExecutor::CGIExecutor(std::vector<std::string>& _r_fields, LocationSet& _location)
{
    char*       pwd;
    std::string ext;

    pwd = getcwd(NULL, 0);
    ext = _r_fields[HR_RURL].substr(_r_fields[HR_RURL].find_last_of(".") + 1, \
    _r_fields[HR_RURL].length() - _r_fields[HR_RURL].find_last_of(".") - 1);
    
    _env[ENV_CONTENT_TYPE] = std::string("CONTENT_TYPE=") + _r_fields[HR_CTYP].data();
    _env[ENV_CONTENT_LENGTH] = std::string("CONTENT_LENGTH=") + _r_fields[HR_CNTLEN].data();
    _env[ENV_PATH_INFO] = std::string("PATH_INFO=") + pwd + "/";
    _env[ENV_PATH_TRANSLATED] = std::string("PATH_TRANSLATED=") + pwd + "/";
    if (!_location._upload_dir.empty())
        _env[ENV_UPLOAD_DIR] = std::string("UPLOAD_DIR=") + pwd + "/" + _location._upload_dir + "/";
    _env[ENV_QUERY_STRING] = std::string("QUERY_STRING=") + _r_fields[HR_QUERIES].data();
    _env[ENV_REQUEST_METHOD] = std::string("REQUEST_METHOD=") + _r_fields[HR_METHOD].data();
    _env[ENV_SCRIPT_FILENAME] = std::string("SCRIPT_FILENAME=") + _r_fields[HR_RURL];
    _env[ENV_SCRIPT_NAME] = std::string("SCRIPT_NAME=") + _r_fields[HR_RURL];
    _env[ENV_SERVER_NAME] = std::string("SERVER_NAME=") + "127.0.0.1";
    _env[ENV_SERVER_SOFTWARE] = std::string("SERVER_SOFTWARE=") + "nginy/1.33.7";
    _env[ENV_SERVER_PROTOCOL] = std::string("SERVER_PROTOCOL=") + "HTTP/1.1";
    _env[ENV_GATEWAY_INTERFACE] = std::string("GATEWAY_INTERFACE=") + "CGI/1.1";
    _env[ENV_HTTP_COOKIE] = std::string("HTTP_COOKIE=") + _r_fields[HR_COOKIE];
    _env[ENV_REDIRECT_STATUS] = std::string("REDIRECT_STATUS=") + "0";
    if (ext == "php")
        _env[ENV_PHP_INI_SCAN_DIR] = std::string("PHP_INI_SCAN_DIR=") + pwd + "/config/";
    
    if (_location._cgi[ext][0] != '/')
        _args[0] = std::string(pwd) + "/" + _location._cgi[ext]; 
    else
        _args[0] = _location._cgi[ext]; 
    _args[1] = _r_fields[HR_RURL];
    
    _body = nullptr;
}
CGIExecutor::~CGIExecutor()
{
}

void CGIExecutor::set_body(std::string *_sv)
{
    _body = _sv;
}
/******************************************************************************/
/*                                 execute cgi                                */
/******************************************************************************/
void sigpipe_handler(int signum) 
{
    (void)signum;
}

int CGIExecutor::execute_cgi(void)
{
    std::map<int, std::string>::iterator    it;
    const char*                             env[21];
    const char*                             args[3];
    int                                     fds[2];
    int                                     pid;
    int                                     i;
    
    it = _env.begin();
    for (i = 0; it != _env.end(); ++it, ++i)
        env[i] = it->second.data();
    env[i] = nullptr;
    
    it = _args.begin();
    for (i = 0; it != _args.end(); ++it, ++i)
        args[i] = it->second.data();
    args[i] = nullptr;

    if (_body)
        if (pipe(fds) == -1)
            throw server_error(std::string("error: pipe: ") + ::strerror(errno));
    pid = fork();
    if (pid == -1)
            throw server_error(std::string("error: pipe: ") + ::strerror(errno));
    if (!pid)
    {
        // for (size_t i = 0; i < _env.size(); ++i) out(env[i]) for (size_t i = 0; i < _args.size(); ++i) out(args[i])
        int fd_out = open("/tmp/cgi_tmp_file", O_WRONLY | O_TRUNC | O_CREAT, 
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(fd_out, 1);
        close(fd_out);
        if (_body)
        {
            dup2(fds[0], 0);
            close(fds[0]);
            close(fds[1]);
        }
        execve(args[0], (char**)args, (char**)env);
        std::cerr << "error: execve: " <<  ::strerror(errno) << '\n';
        exit(1);
    }
    if (_body)
    {
        close(fds[0]);
        if (::fcntl(fds[1], F_SETFL,  O_NONBLOCK) < 0)
            throw server_error(std::string("error: fcntl: ") + ::strerror(errno));
        return (fds[1]);
    }
    return (-1);
}


bool CGIExecutor::pass_input_to_cgi(std::string& input, size_t &wlen, int fd)
{
    signal(SIGPIPE, sigpipe_handler);
    int wdata = 0;
    if (input.length() != wlen)
    {
        wlen += wdata = write(fd, input.data() + wlen, input.length() - wlen);
        if (wdata == -1)
            std::cerr << "error: write: " <<  ::strerror(errno) << '\n';
    }
    
    if (input.length() != wlen && wdata > 0)
        return (1);
        
    close(fd);
    wait_for_cgi();
    return (0);
}

void CGIExecutor::wait_for_cgi(void)
{
    int status;
    
    waitpid(-1, &status, 0);
    if (WIFSIGNALED(status))
        throw response_status(SC_502);
    if (WIFEXITED(status))
    {
        status = WEXITSTATUS(status);
        if (status != 0)
            throw response_status(SC_502);
    }
}
