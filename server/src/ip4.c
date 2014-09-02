
int ConnectToSocket (void)
{
	/* create socket */
	return socket (AF_INET, SOCK_STREAM, 0)) < 0)
}


bool BindToPort (int socket, int port)
{
	struct sockaddr_in socket_address;
	
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons (port);     // short, network byte order
	socket_address.sin_addr.s_addr = inet_addr ("10.12.110.57");
	
	memset (socket_address.sin_zero, '\0', sizeof (socket_address.sin_zero));

	return bind (socket, (struct sockaddr *) &socket_address, sizeof (socket_address));
}
