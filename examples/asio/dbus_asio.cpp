#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dbus_asio.h"

#include <iostream>
#include <vector>
// boost
#include <boost/asio.hpp>

using namespace std;

static const char *DBUS_SERVER_NAME = "org.freedesktop.DBus";
static const char *DBUS_SERVER_PATH = "/org/freedesktop/DBus";

typedef vector <string> Names;

boost::asio::io_service ioService;
DBus::Asio::BusDispatcher dispatcher(ioService);

DBusBrowser::DBusBrowser(::DBus::Connection &conn)
  : ::DBus::ObjectProxy(conn, DBUS_SERVER_PATH, DBUS_SERVER_NAME)
{
  typedef std::vector< std::string > Names;

  Names names = ListNames();

  for (Names::iterator it = names.begin(); it != names.end(); ++it)
  {
    cout << *it << endl;
  }
}

void DBusBrowser::NameOwnerChanged(
  const std::string &name, const std::string &old_owner, const std::string &new_owner)
{
  cout << name << ": " << old_owner << " -> " << new_owner << endl;
}

void DBusBrowser::NameLost(const std::string &name)
{
  cout << name << " lost" << endl;
}

void DBusBrowser::NameAcquired(const std::string &name)
{
  cout << name << " acquired" << endl;
}


void signalHandler(const boost::system::error_code& error, int /*signal_number*/)
{
  if (!error)
  {
    // A signal occurred.
    std::cout << "Exiting..." << std::endl;
    ioService.stop();
  }
}

int main(int argc, char *argv[])
{

  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  DBusBrowser browser(conn);
  // connect signals (ctl+c, term)
  boost::asio::signal_set signals(ioService, SIGINT, SIGTERM);
  signals.async_wait(signalHandler);

  ioService.run();

  return 0;
}
