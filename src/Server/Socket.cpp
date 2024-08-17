#include"Socket.hpp"


Socket::Socket(/* args */)
{
	first_read = 0;
    content_lenght = 0;
	is_chuncked = 0;
	hex_valeu = 0;
}

Socket::~Socket()
{
}
const string &Socket::getRequest() const
{
	return this->request;
}
string & Socket::changeRequest()
{
	return this->request;
}
void Socket::setRequest(const string& Request) 
{
	this->request.append(Request);
}
void Socket::setFdPoll(const int &fd, const short & events)
{
	this->fdpoll.fd = fd;
	this->fdpoll.events = events;
}
void Socket::setFdPoll(const short & events)
{
	this->fdpoll.events = events;
}
void Socket::setFdPoll(const pollfd &poll_struct)
{
	this->fdpoll = poll_struct;
}

const pollfd &Socket::getFdPoll() const
{
    return this->fdpoll;
}

const string &Socket::getBoundary() const
{
	return this->boundary;
}
const size_t& Socket::getContenlenght() const
{
	return this->content_lenght;
}
void Socket::setContenlenght(const size_t & contenlenght)
{
	this->content_lenght = contenlenght;
}
void Socket::setBoundary(const string &	boundary)
{
	this->boundary = boundary;
}
