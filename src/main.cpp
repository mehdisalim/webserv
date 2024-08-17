#include "Servers.hpp"
#include "webserver.h"
#include <signal.h>


#define READ_SIZE 1000

vector<string> split(const string& line, const string& sep) {
    vector<string> tokens;
    size_t start = 0;
	size_t end = 0;
	
    while ((end = line.find(sep, start)) != string::npos) {
		if (end != start)
        	tokens.push_back(line.substr(start, end - start));
        start = end + sep.length();
    }
    tokens.push_back(line.substr(start));
    return tokens;
}

string readF(const string &path)
{
	string content;
	char buffer[READ_SIZE + 1];

	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
		return (content);

	while (1)
	{
		ssize_t readBytes = read(fd, buffer, READ_SIZE);
		if (readBytes <= 0)
			break;
		buffer[readBytes] = 0;
		content.append(buffer, (size_t)readBytes);
	}
	close(fd);
	return (content);
}

void socketHaveEvent(Servers &servers, vector<pollfd> &poll_fd)
{
	for (size_t i = 0; i < poll_fd.size(); i++)
	{
		if (poll_fd[i].revents & POLLIN)
		{
			try
			{
				servers.readyToRead(i, poll_fd);
			}
			catch (runtime_error &e)
			{
				Logger::error(cerr, e.what(), "");
			}
		}
		else if (poll_fd[i].revents & POLLOUT)
			servers.readyToWrite(i, poll_fd);
	}
}

int main(int ac, char **av)
{
	string configFile((ac != 2) ? "configurations/default.conf" : av[1]);
	try
	{
		signal(SIGPIPE, SIG_IGN);
		Parser parser(configFile);
		Checker check(parser.getServers());
		check.fullCheck();
		ServerData serv(parser.getServers());
		Servers servers(serv);
		servers.runAllServers();
		servers.setMasterSockets();
		while (true)
		{
			try
			{
				vector<pollfd> poll_fd;
				servers.isSocketsAreReady(poll_fd);
				socketHaveEvent(servers, poll_fd);
			}
			catch (const Servers::PollException &e)
			{
				Logger::warn(cout, e.what(), "");
				continue;
			}
		}
	}
	catch(exception& e)
	{
		Logger::error(cerr, e.what(), "");
	}
}
