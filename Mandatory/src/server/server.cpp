
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abberkac <abberkac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 14:49:52 by abberkac          #+#    #+#             */
/*   Updated: 2024/04/27 20:49:48 by abberkac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Inc/irc.hpp"

bool Server::_signal = false;

// parameterized constructor : initialize the server socket and set the port number
Server::Server(uint16_t port, char *password) : _countCli(0), _port(port), _password(password), _nfds(1), _pollFds(10)
{
	_listen_sd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket
	if (_listen_sd < 0)
		throw std::runtime_error("ERROR: Socket() failed");

	// Set socket option to allow address reuse
	int on = 1;
	if (setsockopt(_listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
	    close(_listen_sd);
		throw std::runtime_error("ERROR: setsockopt() failed");
	}

	// Set socket to non-blocking mode
	if (fcntl(_listen_sd, F_SETFL, O_NONBLOCK) < 0) {
	    close(_listen_sd);
		throw std::runtime_error("ERROR: Server fcntl() failed");
	}

	// Bind the socket to the specified address and port
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET; // Address family for IPv4
	_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any IP address
	_addr.sin_port = htons(_port); // Listen on the specified port

	if (bind(_listen_sd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
	    close(_listen_sd);
		throw std::runtime_error("ERROR: bind() failed");
	}

	// Start listening for incoming connections
	if (listen(_listen_sd, 10) < 0) {
        close(_listen_sd);
		throw std::runtime_error("ERROR: listen() failed");
    }

	// Initialize the fds array with the server socket
	_pollFds[0].fd = _listen_sd;
	_pollFds[0].events = POLLIN;
	_pollFds[0].revents = 0;

	// Initialize the commands map
	_commands["PASS"] = &Server::passCommand;
	_commands["USER"] = &Server::userCommand;
	_commands["PING"] = &Server::pingCommad;
	_commands["PONG"] = &Server::pongCommad;
	_commands["NAMES"] = &Server::namesCommad;
	_commands["TOPIC"] = &Server::topicCommand;
	_commands["QUIT"] = &Server::quitCommand;
	_commands["LIST"] = &Server::listCommand;
	_commands["PART"] = &Server::partCommand;
	_commands["MODE"] = &Server::modeCommand;
	_commands["PRIVMSG"] = &Server::privmsgCommand;
	_commands["INVITE"] = &Server::inviteCommand;
	_commands["NICK"] = &Server::nickCommand;
	_commands["KICK"] = &Server::kickCommand;
	_commands["JOIN"] = &Server::joinCommand;
}

// create the server and handle the incoming connections and data
void Server::createServer()
{
	int rc;

	// signal handling
    signal(SIGINT, Server::sigHandler);
    signal(SIGQUIT, Server::sigHandler);
	signal(SIGPIPE, SIG_IGN);
	// Start listening for incoming connections
	std::cout << YELLOW <<  "server is running ... " << RESET << std::endl;
	while (_signal == false) {
		// Check if the server is shutting down by signal
		rc = poll(&_pollFds[0], _nfds, -1);
	    if (rc < 0 && _signal == false)
			throw std::runtime_error("ERROR: poll() failed");

		for (size_t i = 0; i < _nfds; i++)
        {
			// Check if the server is shutting down by signal
			if (_signal == true)
				break;
			// Check if poll() has detected an event
			if (_pollFds[i].revents & POLLIN)
			{
	    		// Check for incoming connection on the server socket
				if (_pollFds[i].fd == _listen_sd)
					handlIncomeConnections();
				// Handle incoming data from clients
				else
					handleIncomeData(i);
			}
	    }
	}
	cleanUp();
}

// Handle incoming connections:
void Server::handlIncomeConnections()
{
    struct sockaddr_in client_adrs;
    socklen_t sock_len = sizeof(client_adrs);
    memset(&client_adrs, 0, sock_len);

    int newSck = accept(_listen_sd, (struct sockaddr *)&client_adrs, &sock_len);
    if (newSck == -1) {
		std::cerr << RED << "accept() failed" << RESET << std::endl;
		return;
	}
	if (_pollFds.size() >= _nfds)
		_pollFds.resize(_pollFds.size() + 10);

    _pollFds[_nfds].fd = newSck;
    _pollFds[_nfds].events = POLLIN;
    _pollFds[_nfds].revents = 0;
    // Add the new client to the clients map
    _clients.insert(std::pair<int, Client>(newSck, Client(newSck, client_adrs)));
    _nfds++;
    _countCli++;
    // std::cout << GREEN << "New connection" << RESET << std::endl;
}

// run the correct command
void Server::commandRunner(std::vector<std::string> &fields, Client &user)
{
	std::string command(fields[0]);
	fields.erase(fields.begin());

	if (_commands.find(command) != _commands.end())
		(this->*_commands[command])(fields, user);
	else
		replyTo(user.getSocket(), ERR_UNKNOWNCOMMAND(user.getNickname(), command));
}

// Handle incoming data from clients :
void
Server::handleIncomeData(int i)
{
	char buffer[4096] = {0};
	int rc;

	// Receive the incoming data
	rc = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);

	if (rc <= 0) {
		// std::cout << RED << "Connection closed." << RESET << std::endl;
		Server::disconnectClient(_pollFds[i].fd);
		_countCli--;
	}
	// parse the incoming data
	else {
        buffer[rc] = '\0';
        std::string rec(buffer);
		replaceChar(rec, '\r', '\n');

        std::map<int, Client>::iterator cIt = _clients.find(_pollFds[i].fd);
        if (cIt != _clients.end()) {
            if (rec.find_first_of('\n') == std::string::npos) {
                cIt->second._clientBuffer.append(rec);
                return;
            }
			else {
                rec = cIt->second._clientBuffer + rec;
                cIt->second._clientBuffer.clear();
            }

            std::vector<std::string> messages = getBuffers(rec);
            for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
                *it = trimTheSpaces(*it);
                if (!(*it).empty()) {
                    std::vector<std::string> fields = splitBySpace(*it);
                    if (!fields.empty()) {
                        fields[0] = stringUpper(fields[0]);
                        commandRunner(fields, cIt->second);
                    }
                }
            }
        }
    }
}

Server::~Server(void) {
	cleanUp();
}
