#include "Servers.hpp"
#include "../Request/Request.hpp"
#include "../Request/Response.hpp"




string makeRespose(const Socket &socket, const ServerData &serversData)
{
	ServerPattern	server;
	Request	req;
	

	req.parseRequest(socket.getRequest());
	server = ServerData::getServer(serversData, socket.ipAndPort, req.header("Host")).front();
	Response	res(req, server);
	res.setMimeType(server.mimeTypes);

	string content;
	try
	{
		res.isFormed();
		res.isMatched();
        res.isMethodAllowed();
		res.isRedirected();
        res.whichMethod();
	}
	catch (int status)
	{
		string response = res.getResponse();

		if (response.empty()) {
			string file = res.pathToServe + res.isFound(res.pathToServe);
			res.setStatusCode(status);
			res.setMessage(res.getStatusMessage(status));
			res.setHeader("Content-Type", "text/html");

			string fileName = res.getErrorFile(status);
			if (!fileName.empty()) 
				res.redirection(301, fileName);
			else if (status != 200) {
				if (status >= 300 && status < 400)
					res.redirection(status, res.getRedirection());
				else
					res.setBody("<h1 style=\"text-align: center;\" >" + String::toString(status) + " " + res.getStatusMessage(status) + "</h1>");
			}
			else {
				// success OK
				if (res.getBody().empty()) {
					content = readF(file);
					res.setBody(content);
					res.setHeader("Content-Type", res.getMimeType(req.extention(file)));
					res.setHeader("Content-Length", String::toString(content.size()));
				}
				else
					res.setHeader("Content-Length", String::toString(res.getBody().size()));
			}
			res.makeResponse();
		}
		else {
			Request parseCgiRes;
			// parseCgiRes.parse;
		}
		// cout << "res: " << response << endl;
	}
	return res.getResponse();
}


Servers::Servers(ServerData	srvers)
{
	this->servers = srvers;
}

Servers::~Servers(void)
{
}


size_t	Servers::SocketsSize(void) const 
{
	return this->master.size();
}

const Socket&	Servers::getSocket(size_t index) const 
{
	return this->master[index];
}

const vector<Socket>&	Servers::getMasterSockets(void) const 
{
	return this->master;
}

void	Servers::initSockets(vector<String>&	allport)
{
	for(size_t i = 0; i < allport.size();i++)
	{
		int	fd;
		try
		{
			fd = Server::setSocket();
			vector<String> infos = allport[i].split(':');
			Server::bindSocket(fd, infos.front(), (int)strtol(infos.back().c_str(), NULL, 10));
			Server::listenPort(fd);
		}
		catch(runtime_error &e)
		{
			cout << e.what() << endl;continue;
		}
		fdSockets.push_back(fd);
	}
}

vector<String> removeDuplicatePorts(const vector<String>& allPorts)
{
	vector<String> uniquePorts;
	vector<String>::const_iterator begin = allPorts.begin();
	vector<String>::const_iterator end = allPorts.end();
	while (begin != end)
	{
		if (find(begin + 1, end, *begin) == end)
			uniquePorts.push_back(*begin);
		begin++;
	}
	return (uniquePorts);
}

vector<String>	Servers::getAllPorts(void) const
{
	vector<String>			allport;
	vector <ServerPattern>	allservers = servers.getAllServers();
	for (size_t i = 0; i < allservers.size(); i++)
	{
		vector <Data> ports = allservers[i].getData("listen");
		for (size_t j = 0; j < ports.size(); j++)
			allport.push_back(ports[j].getValue().split().front());
	}
	allport = removeDuplicatePorts(allport);
	return allport;
}

void	Servers::runAllServers(void)
{
	allport = getAllPorts();
	initSockets(allport);
}


void	Servers::readyToWrite(size_t &index, vector<pollfd> &poll_fd)
{
	ssize_t write_value = write(poll_fd[index].fd, master[index].respond.c_str(), master[index].respond.size());
	if(write_value <= 0)
	{
		close(poll_fd[index].fd);
		erase(index, master);
		erase(index, poll_fd);
		index--;
	}
	if(write_value > 0)
	{
		master[index].respond.erase(0, (size_t)write_value);
		if(master[index].respond.empty() == true)
		{
			erase(index, master);
			close(poll_fd[index].fd);
			erase(index, poll_fd);
			index--;
		}
	}
}

short	Servers::Revents(size_t index) const 
{
	return this->master[index].getFdPoll().revents;
}

void Servers::acceptConection(size_t index)
{
	int newfd = accept(fdSockets[index], NULL, NULL);
	if(newfd < 0)
		throw runtime_error("accept : filed!");
	
	if(fcntl(newfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
	{
		close(newfd);
		throw runtime_error("fcntl : filed!");
	}
	Socket tmp;
	tmp.setFdPoll(newfd, POLLIN);
	tmp.ipAndPort = allport[index];
	master.push_back(tmp);
}

void Servers::readyToRead(size_t i, vector<pollfd> &poll_fd)
{
	for (size_t j = 0; j < fdSockets.size(); j++)
	{
		if(poll_fd[i].fd == fdSockets[j])
		{
			acceptConection(j);
			return;
		}
	}
	ReadRequest read_request(master[i]);
	try
	{
		read_request.Read();
	}
	catch (ReadRequest::ReadException&)
	{
		master[i].respond = makeRespose(master[i], servers);
		master[i].setFdPoll(POLLOUT);
	}
	catch (closeException&)
	{
		master[i].respond = "";
		master[i].setFdPoll(POLLOUT);
	}
}



void Servers::isSocketsAreReady(vector<pollfd> &poll_fd)
{
	for (size_t i = 0; i < master.size(); i++)
		poll_fd.push_back(master[i].getFdPoll());
	int pint = poll(poll_fd.data(), (nfds_t)(poll_fd.size()), 8000);
	if(pint == 0)
		throw Servers::PollException("Server reloaded");
	if(pint < 0)
		throw runtime_error("poll : poll was failed");
}

void	Servers::setMasterSockets(void)
{
	for (size_t i = 0; i < fdSockets.size(); i++)
	{
		Socket tmp;
		tmp.setFdPoll(fdSockets[i], POLLIN);
		master.push_back(tmp);
	}
}