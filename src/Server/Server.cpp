#include"Server.hpp"

void	Server::bindSocket(int fd, const String &ip, int port)
{
	S_address socket_info;
	socket_info.sin_family = PF_INET;
	socket_info.sin_port = htons((short)port);
	socket_info.sin_addr.s_addr = convertStringToBinary(ip);
	cout << "IP : " << ip << " PORT : " << port << endl;
	if(bind(fd, (const sockaddr*)&socket_info, sizeof(socket_info)) == -1)
	{
		close(fd);
		Logger::error(cerr, "bind : can't Bind this Port", "");
		exit(1);
	}
	stringstream ss;
	ss  << " URL ==> http://" << ip << ":" << port ;
	Logger::success(cout, ss.str(), "");
}

void	Server::listenPort(int Socketfd)
{
	if(listen(Socketfd, SOMAXCONN)  == -1)
	{
		close(Socketfd);
		throw runtime_error("listen : can't listen");
	}
}
int		Server::setSocket()
{
	int fdSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(fdSocket < -1)
		throw runtime_error("socket : can't open the file conection");
	int option = 1;
	if(fcntl(fdSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
	{
		close(fdSocket);
		throw runtime_error("fcntl : filed!");
	}
	if(setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)))
	{
		close(fdSocket);
		throw runtime_error("setsockopt : filed!");
	}
	return fdSocket;
}
