/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLauncher.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 11:27:06 by eabdelha          #+#    #+#             */
/*   Updated: 2023/01/15 17:07:35 by eabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../class/ServerLauncher.hpp"

/******************************************************************************/
/*                            construct-destruct                              */
/******************************************************************************/

ServerLauncher::ServerLauncher(std::vector<ServerSet>* _v) : _server_set(_v)
{
    _kq = ::kqueue();
    if (_kq < 0)
        throw err_strerror(std::string("kqueue: "));
}
ServerLauncher::~ServerLauncher()
{
}

/******************************************************************************/
/*                            core server loop                                */
/******************************************************************************/

void ServerLauncher::core_server_loop(void)
{
    int                         nev;
    std::vector<struct kevent>  o_events;
    
    o_events.resize(NB_OEVENTS);
    for (;;)
    {
        nev =  this->kevent(o_events);
        
        for (size_t i = 0; i < static_cast<size_t>(nev); ++i)
        {
            int     fd = static_cast<int>(o_events[i].ident);
            int     ndata = static_cast<int>(o_events[i].data);
            void*   handler = o_events[i].udata;
            
            if (o_events[i].flags & EV_ERROR || o_events[i].flags & EV_EOF)
            {
                toggle_event(fd, EVFILT_TIMER, EV_DELETE);
                toggle_event(fd, o_events[i].filter, EV_DELETE);
                _rhandl.erase(fd);
                _shandl.erase(fd);
                _responses.erase(fd);
                close(fd);
            }   
            else if (o_events[i].filter == EVFILT_READ)
            {
                if (!handler)
                {
                    try
                    {
                        this->accept(fd);
                    }
                    catch (std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else
                {
                    toggle_event(fd, EVFILT_TIMER, EV_DELETE);
                    initiate_timeout(fd);
                    
                    if (recv_event_handler(fd, ndata, RECV_CAST(handler)))
                    {
                        int cgi_fd = RECV_CAST(handler)->get_cgi_fd();
                        if (cgi_fd)
                        {
                            toggle_event(fd, EVFILT_READ, EV_DELETE);
                            _icgi[cgi_fd] = &_rhandl[fd];
                            toggle_event(cgi_fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, _icgi[cgi_fd]);
                        }
                    }
                }
            }
            else if (o_events[i].filter == EVFILT_WRITE)
            {
                try
                {
                    if (_icgi.find(fd) != _icgi.end())
                        recv_event_handler(fd, ndata, RECV_CAST(handler));
                    else
                    {
                        toggle_event(fd, EVFILT_TIMER, EV_DELETE);
                        initiate_timeout(fd);
                        SEND_CAST(handler)->operator()(fd, ndata);
                    }
                    if (SEND_CAST(handler)->get_is_done())
                    {
                        toggle_event(fd, EVFILT_WRITE, EV_DELETE);
                        _responses.erase(fd);
                        if (SEND_CAST(handler)->get_is_close())
                        {
                            close(fd);
                        }
                        else
                        {
                            toggle_event(fd, EVFILT_READ, EV_ADD | EV_CLEAR, &_rhandl[fd]);
                            _rhandl[fd].set_response(&_responses[fd]);
                        }
                        _shandl.erase(fd);
                    }
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    toggle_event(fd, EVFILT_WRITE, EV_DELETE);
                    if (!SEND_CAST(handler)->get_is_close())
                        _rhandl.erase(fd);
                    _shandl.erase(fd);
                    _responses.erase(fd);
                    close(fd);
                }
            }
            else if (o_events[i].filter == EVFILT_TIMER)
            {
                toggle_event(fd, EVFILT_READ, EV_DELETE);
                toggle_event(fd, EVFILT_WRITE, EV_DELETE);
                toggle_event(fd, EVFILT_TIMER, EV_DELETE);
                _rhandl.erase(fd);
                _shandl.erase(fd);
                _responses.erase(fd);
                close(fd);
            }
        }
    }
}
bool ServerLauncher::recv_event_handler(int fd, int ndata, RecvHandler* handler)
{
    try
    {
        handler->operator()(fd, ndata);
        if (handler->get_is_done())
        {
            int             sock_fd;
            SendHandler*    send_handl;
            
            sock_fd = handler->get_sock_fd();
            send_handl = &_shandl[sock_fd];
            
            send_handl->set_response(&_responses[sock_fd]);
            send_handl->set_is_close(handler->get_is_close());
            
            toggle_event(sock_fd, EVFILT_READ, EV_DELETE);
            toggle_event(sock_fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, send_handl);
            if (!handler->get_cgi_fd())
                _icgi.erase(fd);
            if (handler->get_is_close())
                _rhandl.erase(sock_fd);
            else
                handler->init_variables();
            return (0);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        toggle_event(fd, EVFILT_READ, EV_DELETE);
        _rhandl.erase(fd);
        _responses.erase(fd);
        close(fd);
        return (0);
    }
    return (1);
}

/******************************************************************************/
/*                            system call wrappers                            */
/******************************************************************************/

void ServerLauncher::launch_routines(void)
{
    std::map<int, int> pair_port_fd;
    
    for (size_t i = 0; i < _server_set->size(); ++i)
    {
        if (pair_port_fd.find((*_server_set)[i]._port) != pair_port_fd.end())
        {
            _pair_fd_servers[pair_port_fd[(*_server_set)[i]._port]].push_back(&(*_server_set)[i]);
            continue;
        }
        this->socket();
        this->setsockopt();
        this->fcntl();
        this->bind((*_server_set)[i]);
        this->listen();
        pair_port_fd[(*_server_set)[i]._port] = _sock_fds.back();
        _pair_fd_servers[_sock_fds.back()].push_back(&(*_server_set)[i]);
    }
    enable_socket_monitoring();
}

void ServerLauncher::socket(void)
{
    
    _sock_fds.push_back(::socket(AF_INET, SOCK_STREAM, 0));
    if (_sock_fds.back() < 0)
        throw err_strerror(std::string("socket: "));
}
void ServerLauncher::setsockopt(void)
{
    int enable = 1;
    
    if (::setsockopt(_sock_fds.back(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw err_strerror(std::string("setsockopt: "));
    if (::setsockopt(_sock_fds.back(), SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(int)) < 0)
        throw err_strerror(std::string("setsockopt: "));
}
void ServerLauncher::fcntl(void)
{
    if (::fcntl(_sock_fds.back(), F_SETFL,  O_NONBLOCK) < 0)
        throw err_strerror(std::string("fcntl: "));
}
void ServerLauncher::bind(ServerSet &serv)
{
    struct sockaddr_in addr;
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(serv._port);
    addr.sin_addr.s_addr = inet_addr(serv._address.data());
    if (::bind(_sock_fds.back(), (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw err_strerror(std::string("bind: "));
    print_date();
    std::cout << "Listening on [" << serv._address << ':' << serv._port << ']' << "\033[0m" << std::endl;
}
void ServerLauncher::listen(void)
{
    if (::listen(_sock_fds.back(), LISN_BACKLOG) < 0)
        throw err_strerror(std::string("listen: "));
}

int ServerLauncher::kevent(std::vector<struct kevent> &o_events)
{
    int rc = ::kevent(_kq, NULL, 0, o_events.data(), NB_OEVENTS, 0);
    if (rc < 0)
        throw err_strerror(std::string("kevent: "));
    return (rc);
}

void ServerLauncher::accept(int fd)
{
    struct kevent   event;
    
    int rc = ::accept(fd, NULL, NULL);
    if (rc < 0)
        throw err_strerror(std::string("accept: "));

    initiate_timeout(rc);
    EV_SET(&event, rc, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, &_rhandl[rc]);
    _rhandl[rc].set_servers(&_pair_fd_servers[fd]);
    _rhandl[rc].set_response(&_responses[rc]);
    _rhandl[rc].set_sock_fd(rc);
    ::kevent(_kq, &event, 1, NULL, 0, 0);
}


/******************************************************************************/
/*                            utility functions                               */
/******************************************************************************/

void ServerLauncher::enable_socket_monitoring(void)
{
    std::vector<struct kevent>  i_events;
    
    for (size_t i = 0; i < _sock_fds.size(); ++i)
    {
        struct kevent tmp = {};
        i_events.push_back(tmp);
        EV_SET(&i_events.back(), _sock_fds[i], EVFILT_READ, EV_ADD , 0, 0, NULL);
    }
    ::kevent(_kq, &i_events[0], static_cast<int>(i_events.size()), NULL, 0, 0);
}

void ServerLauncher::toggle_event(int fd, int16_t filter, uint16_t flags, void *udata)
{
    struct kevent event;

    EV_SET(&event, fd, filter, flags, 0, 0, udata);
    ::kevent(_kq, &event, 1, NULL, 0, 0);
}
void ServerLauncher::initiate_timeout(int fd)
{
    struct kevent   event;
    struct timespec timeout;
    size_t          time_limit;

    timeout.tv_sec = 70; 
    timeout.tv_nsec = 0;
    time_limit = static_cast<size_t>(timeout.tv_sec) * 1000 + static_cast<size_t>(timeout.tv_nsec) / 1000000;

    EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_CLEAR, 0, time_limit, nullptr);
    ::kevent(_kq, &event, 1, NULL, 0, 0);
}
