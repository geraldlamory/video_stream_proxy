#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef _WIN32
  #include <winsock2.h>
  typedef SOCKET socket_t;
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netdb.h>
  typedef int socket_t;
  #define INVALID_SOCKET -1
#endif

#include <string>

class Socket
{
public:

  // Must be called before creating any socket
  static void initialize();
  // Clean up when no socket is used anymore
  static void cleanUp();

  explicit Socket();
  Socket(const Socket& socket);
  virtual ~Socket();

  void close();

  // Start listening on the given port
  bool listen(unsigned int port);
  // Connect to the port on the given host
  bool connect(const std::string& host, int port);

  // Wait for a client to connect and return the new socket corresponding to the client
  Socket accept() const;

  int read(char* buffer, size_t size);
  int send(const char* buffer, size_t size) const;

  int getLastError() const;
  std::string getLastErrorMessage() const;

  bool isValid() const { return _sock != INVALID_SOCKET; }

  Socket& operator=(const Socket&) = delete;

private:

  // Create the socket and return the address info structure
  // freeaddrinfo must be called on addrinfo
  addrinfo* createSocket(const char* host, const char* port);

  explicit Socket(socket_t socket);

  socket_t    _sock;

};

#endif // __SOCKET_H__
