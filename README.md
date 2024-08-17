# Webserv

This project involves creating a simple HTTP server in C++98 that can handle various HTTP requests, just like NGINX or Apache. It is designed to give you a deep understanding of the HTTP protocol, which is essential for web development and related systems.

## Table of Contents
- [Introduction](#introduction)
- [General Rules](#general-rules)
- [Mandatory Features](#mandatory-features)
- [Bonus Features](#bonus-features)
- [Configuration](#configuration)
- [Building and Running the Project](#building-and-running-the-project)
- [Testing](#testing)
- [Submission and Evaluation](#submission-and-evaluation)

## Introduction
The goal of this project is to implement a fully-functional HTTP server. The server should be able to handle HTTP requests, manage connections, and serve both static and dynamic content. The project is designed to provide a strong foundation in networking and the HTTP protocol.

## General Rules
- The server must be written in C++98.
- The server should never crash or quit unexpectedly.
- You must provide a Makefile that supports the following targets: `all`, `clean`, `fclean`, `re`.
- Compile with `-Wall`, `-Wextra`, and `-Werror` flags.
- The project must be compatible with both Linux and MacOS environments.
- You are not allowed to use any external libraries, including Boost.
- The code must comply with the C++98 standard.

## Mandatory Features
The core requirements for the server include:
- **Non-blocking I/O:** The server must be fully non-blocking using `poll()`, `select()`, or other equivalents.
- **HTTP Methods:** The server must support `GET`, `POST`, and `DELETE` requests.
- **Static Content:** Serve static files such as HTML, CSS, and images.
- **Multiple Ports:** The server should be able to listen on multiple ports simultaneously.
- **CGI Support:** The server should be able to execute CGI scripts (e.g., PHP, Python).
- **File Uploads:** Clients should be able to upload files via the server.
- **Configuration File:** The server should accept a configuration file that defines the ports, server names, routes, error pages, etc.
- **Resilience:** The server should handle high traffic loads and remain stable under stress.

### Configuration File
The server's configuration file should allow the following settings:
- Port and host configurations.
- Server names.
- Error pages.
- Client body size limits.
- Route definitions, including accepted methods, redirections, directory listing, and CGI execution.
- File upload paths.

## Bonus Features
If all mandatory features are implemented perfectly, the following extras can be added:
- **Multiple CGI Support:** Handle different CGI scripts for various routes and file extensions.

## Building and Running the Project
To build the project, run the following command:

```bash
make
```
To start the server:

```bash
./webserv [configuration file]
```
If no configuration file is provided, a default configuration will be used.

### Testing
You should test the server using:
- Web browsers like Chrome, Firefox, or Safari.
- Command-line tools like telnet and curl.
- NGINX for comparison with standard HTTP responses and behavior.
Stress testing is recommended to ensure the server remains stable under heavy load.

### Submission and Evaluation
Submit your project in a Git repository. Ensure that all required files are included:
- Source files (*.cpp, *.h, *.hpp)
- Configuration files
- Makefile

The evaluation will focus on:
- Compliance with the C++98 standard.
- Correct implementation of mandatory features.
- Stability and resilience under load.
- Correctness of HTTP responses and behavior.

## Contributing
Contributions to this project are welcome! If you have suggestions or improvements, please open an issue or submit a pull request. When contributing, please ensure that your code follows the existing style and conventions.

## License
This project is licensed under the MIT License. For more details, please refer to the LICENSE file.
