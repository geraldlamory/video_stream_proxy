#include "Socket.h"

#ifdef _WIN32
  #include <Ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
#endif

#include <stdexcept>

void Socket::initialize()
{
#ifdef _WIN32
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(1, 1), &wsa_data) != 0)
    throw std::runtime_error("Failed to initialize WSA");
#endif
}

void Socket::cleanUp()
{
#ifdef _WIN32
  WSACleanup();
#endif
}

Socket::Socket() :
  _sock(INVALID_SOCKET)
{
}

Socket::Socket(const Socket& socket) :
  _sock(socket._sock)
{
}

Socket::Socket(socket_t socket) :
  _sock(socket)
{
}

Socket::~Socket()
{
}

addrinfo* Socket::createSocket(const char* host, const char* port)
{
  struct addrinfo   hints;
  struct addrinfo* addr;

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  if (host == nullptr)
  {
    hints.ai_family = AF_INET; // IPv4
    hints.ai_flags = AI_PASSIVE; // localhost
  }
  else
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6

  if (getaddrinfo(host, port, &hints, &addr) < 0)
    return nullptr;

  _sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (_sock != INVALID_SOCKET)
  {
    char yes = 1;
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(char));
  }

  return addr;
}

bool Socket::listen(unsigned int port)
{
  addrinfo* addr = this->createSocket(nullptr, std::to_string(port).c_str());
  if (_sock == INVALID_SOCKET)
    return false;

  if (bind(_sock, addr->ai_addr, addr->ai_addrlen) < 0)
    return false;

  freeaddrinfo(addr);

  // Start listening
  if (::listen(_sock, 0) < 0)
    return false;

  return true;
}

bool Socket::connect(const std::string& host, int port)
{
  addrinfo* addr = this->createSocket(host.c_str(), std::to_string(port).c_str());

  if (_sock == INVALID_SOCKET)
    return false;

  int result = ::connect(_sock, addr->ai_addr, addr->ai_addrlen);

  freeaddrinfo(addr);

  return result == 0;
}

Socket Socket::accept() const
{
  socket_t    client = INVALID_SOCKET;
  sockaddr_in client_addr;

  int addr_len = sizeof(client_addr);
  client = ::accept(_sock, (struct sockaddr*)&client_addr, &addr_len);

  return Socket(client);
}

void Socket::close()
{
  if (_sock != INVALID_SOCKET)
  {
#ifdef _WIN32
    if (shutdown(_sock, SD_BOTH) == 0)
      closesocket(_sock);
#else
    if (shutdown(_sock, SHUT_RDWR) == 0)
      close(_sock);
#endif
    _sock = INVALID_SOCKET;
  }
}

int Socket::read(char* buffer, size_t size)
{
  return recv(_sock, buffer, size, 0);
}

int Socket::send(const char* buffer, size_t size) const
{
  return ::send(_sock, buffer, size, 0);
}

int Socket::getLastError() const
{
#ifdef _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

std::string Socket::getLastErrorMessage() const
{
  int code = this->getLastError();
  std::string message;
#ifdef _WIN32
  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf,
    0, NULL);

  message = static_cast<char*>(lpMsgBuf);
  LocalFree(lpMsgBuf);

#else
  message = strerror(code);
#endif

  return message;
}
