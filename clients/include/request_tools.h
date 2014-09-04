#ifndef REQUEST_TOOLS_H
#define REQUEST_TOOLS_H


#ifdef __cplusplus
extern "C" 
{
#endif


int SendRequest (int socket_fd, const char * const req_s);

int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef REQUEST_TOOLS_H */
