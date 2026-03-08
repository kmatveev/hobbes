#include "netserver.H"
#include "network.H"

#include <hobbes/ipc/net.H>

#if defined(__MINGW64__) || defined(_MSC_VER)
#include <winsock2.h> // accept
#include <ws2tcpip.h>
#include <io.h> // close
#else
#include <sys/socket.h> // accept
#include <unistd.h> // close
#endif

namespace hog {

// TODO allocateServer does not support host, only port
// - that's an issue if the host has multiple net device
DefaultNetServer::DefaultNetServer(const std::string& hostport)
  :_socket(hobbes::allocateServer(hostport)) // throws on error
{}

DefaultNetServer::~DefaultNetServer()
{
  close(_socket);
}

std::unique_ptr<NetConnection> DefaultNetServer::accept() {
  const int client = ::accept(_socket, nullptr, nullptr);
  if (client < 0) {
    return {};
  }

  return createNetConnection(client);
}

} // namespace hog
