#include "Response.hpp"
#include <cmath>
#include <signal.h>
#include <sys/wait.h>

int Response::isDirectory(const string& path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == -1)
		return (-1);
	return (S_ISDIR(statbuf.st_mode));
}

int Response::isFile(const string& path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == -1)
		return (-1);
	return (S_ISREG(statbuf.st_mode));
}



string Response::runScript(vector<String> args, string fileName)
{
	string		response;
	pid_t		pid;
	int			input[2];
	int			output[2];
	int			status;
	string		extention;
	size_t		pos;



	if ((pos = fileName.rfind(".")) != string::npos)
		extention = fileName.substr(pos, fileName.size() - pos);
	if (!extention.empty() && args.size() == 2) {
		if (args[1] == extention) {
			if (pipe(input) == -1 || pipe(output) == -1)
				throw 500;
			if ((pid = fork()) == -1)
				throw 500;
			if (pid == 0) {
				char	*argv[] = { &args[0][0], &fileName[0], NULL};
				string str[11] = {
					("HTTP_CONNECTION=" + request.header("Connection") + "\0"),
					("HTTP_HOST=" + request.header("Host")),
					("CONTENT_LENGTH=" + request.header("Content-Length") + "\0"),
					("CONTENT_TYPE=" + request.header("Content-Type") + "\0"),
					("HTTP_ACCEPT=" + request.header("Accept") + "\0"),
					("HTTP_USER_AGENT=" + request.header("User-Agent") + "\0"),
					("PATH_INFO=" + request.getPath() + "\0"),
					("SCRIPT_NAME=" + fileName + "\0"),
					("QUERY_STRING=" + request.getQuery() + "\0"),
					("REQUEST_METHOD=" + request.getMethod() + "\0")
				};
				char	*envp[12];
				size_t i = 0;
				for ( i = 0 ; i < 11; i++)
					envp[i] = &str[i][0];
				envp[i] = NULL;

				close(output[0]);
				dup2(output[1], STDOUT_FILENO);
				close(output[1]);

				close(input[1]);
				dup2(input[0], STDIN_FILENO);
				close(input[0]);
				if (execve(argv[0], argv, envp) < 0)
					exit(1);
			}
			else {
				time_t start_time = time(NULL);
				if (start_time == (long)-1)
					throw 500;
				vector<Data> cgi_time = location.getData("cgi_time");
				double timeout_seconds = (cgi_time.empty()) ? 3 : strtod(cgi_time.front().getValue().c_str(), NULL);
				const string &resBody = request.getBody();
				close(input[0]);
				if (write(input[1], resBody.c_str(), resBody.size()) == -1)
					exit(1);
				close(input[1]);

				while (true) {
            		if (waitpid(pid, &status, WNOHANG) == pid) {
                		break;
            		}
            		double current_time = double(time(NULL) - start_time);
            		if (current_time > timeout_seconds) {
                		kill(pid, SIGKILL);
                		waitpid(pid, &status, 0);
						throw 504;
					}
					usleep(100000); // 100 milliseconds == 100000 microseconde
            	}
				if (WIFEXITED(status) && WEXITSTATUS(status) && WEXITSTATUS(status))
					throw 500;
				char res[200];
				bzero(res, 200);
				ssize_t bytes = 0;
				close(output[1]);
				while ((bytes = read(output[0], res, 199)) != 0) {
					response.append(res);
					bzero(res, 200);
					if (bytes < 199)
						break;
				}
				close(output[0]);
			}
		}
	}
	return response;
}


Response::Response( const Request& req, const ServerPattern& _server ) : request(req), server(_server)
{
	setStatusCode(200);
	setMessage("OK");
	setHeader("Server", "Nginx-v2");
	setStatusMessage();
}

Response::~Response(void) {
}

void Response::setStatusMessage()
{
	statusMessage[200] = "OK";
	statusMessage[201] = "Created";
	statusMessage[204] = "No Content";

	statusMessage[301] = "Move Permanently";
	statusMessage[302] = "Found";
	statusMessage[304] = "Not Modified";

	statusMessage[400] = "Bad Request";
	statusMessage[403] = "Forbidden";
	statusMessage[404] = "Not Found";

	statusMessage[405] = "Method Not Allowed";
	statusMessage[413] = "Request Entity Too Longe";
	statusMessage[414] = "Request-Uri Too Longe";

	statusMessage[500] = "Internal Server Error";
	statusMessage[501] = "Not Implamented";
	statusMessage[504] = "Gateway Timeout";
}
string Response::getStatusMessage( int status ) {
	return (statusMessage[status]);
}

void Response::setFileToServe(const string &fileName)
{
	fileToServe = fileName;
}

void Response::setStatusCode( int statusCode )
{
    this->statusCode = statusCode;
}
void Response::setMessage( const string &message )
{
    this->message = message;
}
void Response::setHeader(const string &key, const string &value)
{
    header[key] = value;
}
void Response::setBody(const string &body)
{
    this->body = body;
}
void Response::setResponse(const string& response) {
	this->response = response;
}



// geters
const string &Response::getMessage( void ) const
{
    return (this->message);
}
const string &Response::getResponse( void ) const
{
    return (this->response);
}
const string& Response::getBody() const {
	return (body);
}
const string&	Response::getFileToServe() const {
	return (fileToServe);
}


void Response::makeResponse( void ) {
	response += VERSION + String::toString(statusCode) + " " + getMessage() + ENDLINE;
	if (!header.empty()) {
		for ( maps::iterator it = header.begin(); it != header.end(); it++) {
			response += it->first + ": " + it->second + ENDLINE; 
		}
	}
	response += ENDLINE;
	if (!body.empty()) {
		response += body + "\n";
	}
}


string Response::getMimeType( const string &key) const
{
	try {
		string ret = mimeType.at(key);
		return (ret);
	}
	catch(...){
		return ("text/plain");
	}
}

void Response::setMimeType(const map<string, string>& mimeType)
{	
	if (mimeType.size())
		this->mimeType = mimeType;
}




bool isAllowdChar(const string& uri) {
	string alloweChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	for (size_t i = 0; i < uri.size(); i++)
	{
		if (find(alloweChar.begin(), alloweChar.end(), uri[i]) == alloweChar.end() || (uri[i] == '%' && isdigit(uri[i + 1])) )
			return (false);
	}
	return (true);
}
void Response::isFormed()
{
	string value = server.getData("client_max_body_size")[0].getValue();
	long long size = convertor(value);

	if (!request.header("Transfer-Encoding").empty() && request.header("Transfer-Encoding") != "chunked")
		throw 500;
	else if ((request.header("Transfer-Encoding").empty() && request.header("Content-Length").empty() && request.getMethod() == "POST") || !isAllowdChar(request.getPath()))
		throw 400;
	else if (request.getPath().length() > 2048)
		throw 414;
	else if (isinf(size) || (long long)request.getBody().size() > size)
		throw 413;
}
void Response::isMatched() {

	string path = request.getPath();


	vector<String> pathss;
	ServerPattern::getAllLocationPath(server.getLocations(), pathss);
	vector<String> newPaths;
	String test(path);
	test.rightTrim("/");

	for (size_t i = 0; i < pathss.size(); i++)
	{
		if (!strncmp(pathss[i].c_str(), test.c_str(), pathss[i].size()) && pathss[i].size() == test.size())
		{
			newPaths.clear();
			newPaths.push_back(pathss[i]);
			break;
		}
		if (!strncmp(pathss[i].c_str(), test.c_str(), pathss[i].size()))
			newPaths.push_back(pathss[i]);
	}
	vector<String>::iterator it = max_element(newPaths.begin(), newPaths.end());
	if (it == newPaths.end())
		location = server;
	else
		location = server.getLocationByPath(server.getLocations(), *it);
	
}
void Response::isRedirected() {
	if (!location.empty()) {
		vector<Data> redirection = location.getData("return");
		if (redirection.size()) {
			vector<string> sp = split(redirection[0].getValue(), " ");
			if (sp.size() == 2) {
				this->_redirection = sp[1];
				int red = atoi(sp[0].c_str());
				if (red >= 300 && red < 400)
					throw red;
			}
		}
	}
}
void Response::isMethodAllowed() {
	vector<Data> method = location.getData("method");
	if (!method.empty()) {
		vector<string> methods = split(method.front().getValue(), " ");
		if (find(methods.begin(), methods.end(), request.getMethod()) == methods.end())
			throw 405;
	}
	else
		throw 405;
}
void Response::whichMethod() {
	string path = request.getPath();
	string root = getRoot();
	string alias = getAlias();

	if (!alias.empty()) {
		string locationPath = location.getPath();
		string newPath;
		size_t pos = 0;
		if ((pos = path.find(locationPath)) != string::npos)
			newPath = path.substr(pos + locationPath.length(), path.length() - pos + locationPath.length());
		pathToServe = alias + newPath;  
	}
	else
		pathToServe = root + request.getPath();
	// cout << "method ======> [" << request.getMethod() << "]\n";
	if (request.getMethod() == "GET") {
		GetMethod(pathToServe);
		// handle Get method
	}
	else if (request.getMethod() == "POST") {
		// handle Post method
		PostMethod(pathToServe);
	}
	else if (request.getMethod() == "DELETE") {
		// handle Delete method
		DeleteMethod(pathToServe);
	}
}

string Response::getErrorFile(int statusCode) const
{
	vector<Data>  errorPage = location.getData("error_page");

	for (size_t i = 0; i < errorPage.size(); i++) {
		vector<string> value = split(errorPage[i].getValue(), " ");
		for (size_t j = 0; j < value.size() - 1; j++) {
			if (statusCode == atoi(value[j].c_str()))
				return (value[value.size() - 1]);
		}
	}
	return ("");
}
string Response::getRoot() const {
	if (!location.empty()) {
		vector<Data> root = location.getData("root");
		if (root.empty())
			return ("");
		else
			return (root[0].getValue());
	}
	return ("");
}
string Response::getAlias() const {
	if (!location.empty()) {
		vector<Data> data = location.getData("alias");
		if (data.size())
			return (data[0].getValue());
	}
	return ("");
}
string Response::isFound( const string& path ) const
{
	vector<Data> vIndex = location.getData("index");
	if (!vIndex.empty())
	{
		for (size_t i = 0; i < vIndex.size(); i++)
		{
			vector<string> filename = split(vIndex[i].getValue(), " ");
			for (size_t j = 0; j < filename.size(); j++)
			{
				if (access((path + filename[j]).c_str(), O_RDONLY) != -1)
					return (filename[j]);
			}
		}
	}
	return ("");
}
bool Response::isCgi() {
	vector<Data> cgiFiles = location.getData("cgi");
	if (cgiFiles.empty())
		return (false);
	return (true);
}
string Response::getCgiFile()
{
	vector<Data> cgiFiles = location.getData("cgi");
	if ( !cgiFiles.empty() )
		return cgiFiles[0].getValue();
	return "";
}

string Response::getLocationPath() {
	location.getPath();
	return (location.getPath());
}

string Response::isUpload() {
	vector<Data> uploadFile = location.getData("upload_dir");
	if ( !uploadFile.empty())
		return (uploadFile[0].getValue());
	else
		return ("");
}


const string& Response::getRedirection() const {
	return (_redirection);
}
void Response::setRedirection(const string& redirection) {
	this->_redirection = redirection;
}
const map<int, string>& Response::getStatusMessage() const {
	return (statusMessage);
}



void Response::GetMethod(const string& pathToServe) {
	string index;
	if (isDirectory(pathToServe) == 1) {
		// is Directory
		if (request.getPath() != "/" && pathToServe[pathToServe.size() - 1] != '/') {
			_redirection = request.getPath() + "/";
			throw 301;
		}

		index = isFound(pathToServe);
		if (!index.empty()) {
			// success have a index
			if (access((pathToServe + index).c_str(),  O_RDONLY) != -1)
				throw 200;
			else if (isCgi()) // if location have cgi
				setResponse(runScript(location.getData("cgi")[0].getValue().split(' ') , pathToServe + index));
				// run cgi here


				
		}
		else
		{
			if (!this->location.getData("autoindex").empty() && this->location.getData("autoindex")[0].getValue() == "on")
			{
				if (pathToServe[pathToServe.length() - 1] == '/')
					body = getDirectoryContent(pathToServe, request.getPath());
				else
					throw 404;
			}
			else
				throw 403;
			// auto index
			// check if autoindex in configfile is on or off if is off 403 otherwise return autoindex
		}
	}
	else if (isFile(pathToServe) == 1) {
		// is File
		// if (pathToServe == alias)
		// 	throw 500;
		size_t pos;
		string extention;
		if ((pos = pathToServe.rfind(".")) != string::npos)
			extention = pathToServe.substr(pos, pathToServe.size() - pos);
		if (isCgi() && !extention.empty() && extention == location.getData("cgi")[0].getValue().split(' ')[1])
			// run cgi here
			setResponse(runScript(location.getData("cgi")[0].getValue().split(' ') , pathToServe));

		else {
			if (access((request.getPath() + index).c_str(), O_RDONLY) != -1)
				throw 404;
			else
				throw 200;
		}
	}
	else
		throw 404;
	throw 200;
}
void Response::PostMethod(const string& pathToServe) {
	string index;

	if (isDirectory(pathToServe) == 1) {
		// is Directory
		if (request.getPath() != "/" && pathToServe[pathToServe.size() - 1] != '/') {
			_redirection = request.getPath() + "/";
			throw 301;
		}
		index = isFound(pathToServe);
		if (!index.empty()) {
			// success have a index
			if (access((pathToServe + index).c_str(),  O_RDONLY) != -1)
				throw 200;
			else if (isCgi()) // if location have cgi
				// run cgi here
				setResponse(runScript(location.getData("cgi")[0].getValue().split(' ') , pathToServe + index));
			else if (!isUpload().empty()) {
				string uploadDir = isUpload();
				if (uploadDir.back() != '/')
					uploadDir.append("/");
				
				vector<pair<string, string> > upload = request.getUploads();
				for (size_t i = 0; i < upload.size(); i++)
				{
					if (!upload[i].first.empty()) {
						ofstream up(uploadDir + upload[i].first);
						up << upload[i].second;
						up.close();
					}
				}
			}

		}
		else
			throw 403;
	}
	else if (isFile(pathToServe) == 1) {
		size_t pos;
		string extention;
		if ((pos = pathToServe.rfind(".")) != string::npos)
			extention = pathToServe.substr(pos, pathToServe.size() - pos);
		if (isCgi() && !extention.empty() && extention == location.getData("cgi")[0].getValue().split(' ')[1])
			// run cgi here
			setResponse(runScript(location.getData("cgi")[0].getValue().split(' ') , pathToServe));
		else if (!isUpload().empty()) {
			string uploadDir = isUpload();
			if (uploadDir.back() != '/')
				uploadDir.append("/");
			if (!opendir(uploadDir.c_str()))
				throw 500;
			vector<pair<string, string> > upload = request.getUploads();
			for (size_t i = 0; i < upload.size(); i++)
			{
				if (!upload[i].first.empty()) {
					ofstream up(uploadDir + upload[i].first);
					up << upload[i].second;
					up.close();
				}
			}
		}
		else {
			if (access((request.getPath() + index).c_str(), O_RDONLY) != -1)
				throw 404;
			else
				throw 200;
		}
	}
	else
		throw 404;
	throw 200;
}
void Response::DeleteMethod(const string& pathToServe) {
	string index;
	if (isDirectory(pathToServe) == 1) {
		if (request.getPath() != "/" && pathToServe[pathToServe.size() - 1] != '/')
			throw 409;
		deleteAll(pathToServe);
		throw 204;
	}
	else if (isFile(pathToServe) == 1) {
		if (!remove(pathToServe.c_str()))
			throw 204;
		else
			throw 500;
	}
	else
		throw 404;
}
void Response::deleteAll (const string& path) 
{
	DIR	*dir = opendir(path.c_str());
	if (!dir)
		throw 500;
	struct dirent *dirp;

	while ((dirp = readdir(dir)) != NULL)
	{
		if (string(dirp->d_name) == "." || string(dirp->d_name) == "..")
			continue ;
		else if (isDirectory(path + dirp->d_name) == 1)
			deleteAll(path + dirp->d_name + "/");
		else if (remove((path + dirp->d_name).c_str()))
				throw 500;
	}
	closedir(dir);
}
void Response::redirection(int code, const string& path) {
	setStatusCode(code);
	setMessage(getStatusMessage(code));
	setHeader("Location", path);
}