#include <ForceFeedback/ForceFeedback.h>
#include <GSS/GSS.h>
#include <tkDecls.h>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/errno.h>
#include <IOKit/IOCFPlugIn.h>
#include <sys/_types/_fd_zero.h>
#include <bootstrap.h>
#include <Foundation/Foundation.h>
#include <curses.h>
#include <sys/_types/_timeval.h>
#include <stddef.h>
#include <sys/socket.h>
#include <dispatch/io.h>
#include "sckutil.h"

void activate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFD);
    if(flags == -1)
        ERR_EXIT("fcntl");
    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if(ret == -1)
        ERR_EXIT("fcntl");
}
void deactivate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFD);
    if(flags == -1)
        ERR_EXIT("fcntl");
    flags &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if(ret == -1)
        ERR_EXIT("fcntl");
}
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0)
    {
        fd_set read_fdset;
        struct timeval timeout;
        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do{
            ret = select(fd+1, &read_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINRT);

        if(ret == 0){
            ret = -1;
            errno = ETIMEOUT;
        }else if(ret == 1){
            ret = 0;
        }
    }
	return ret;
}
int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0)
    {
        fd_set write_fdset;
        struct timeval timeout;
        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do{
            ret = select(fd+1, &write_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINRT);

        if(ret == 0){
            ret = -1;
            errno = ETIMEOUT;
        }else if(ret == 1){
            ret = 0;
        }
    }
    return ret;
}
int accept_timeout(int fd, struct scokaddr_in *addr, unsigned int wait_seconds)
{
	return 0;
}
int connect_timeout(int fd, struct scokaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if(wait_seconds > 0)
        activate_nonblock(fd);
    ret = connect(fd, (struct sockaddr *)addr, addrlen);
    if(ret == -1 && errno == EINPROGRESS)
    {
        fd_set connect_fdset;
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            ret = select(fd+1, NULL, &connect_fdset, NULL, &timeout);
        }while(ret == -1 && errno == EINTR);
        if(ret == 0){
            ret = -1;
            errno = ETIMEDOUT;
        }else if(ret < 0){
            ret = -1;
        }else if(ret == 1){
            int err;
            socklen_t socklen = sizeof(err);
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
            if(sockoptret == -1){
                return -1;
            }else if(err == 0){
                ret = 0;
            }else{
                errno = err;
                ret = -1;
            }
        }

    }
    if(wait_seconds > 0){
        deactivate_nonblock(fd);
    }
	return ret;
}

ssize_t readn(int fd, void *buf, size_t count)
{
	return 0;
}
ssize_t writen(int fd, const void *buf, size_t count)
{
	return 0;
}
ssize_t resv_peek(int sockfd, void *buf, size_t len)
{
	return 0;
}
ssize_t readline(int sockfd, void *buf, size_t maxlen)
{
	return 0;
}

