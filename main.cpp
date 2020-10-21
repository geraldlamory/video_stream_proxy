#include <iostream>
#include <sstream>
#include "Url.h"
#include "HLSProxy.h"

#define PORT 8080

int main(int ac, char** av)
{
  if (ac != 2) {
    std::cout << "usage: " << av[0] << " <stream_url>" << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    Socket::initialize();

    Url       url(av[1]);
    HLSProxy  proxy(url.getHost(), url.getPort());

    proxy.run(PORT);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    Socket::cleanUp();
    return EXIT_FAILURE;
  }

  Socket::cleanUp();
  return EXIT_SUCCESS;
}

