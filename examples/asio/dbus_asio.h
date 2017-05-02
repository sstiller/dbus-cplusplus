#ifndef __DEMO_DBUS_BROWSER_H
#define __DEMO_DBUS_BROWSER_H

// boost
#include <boost/asio.hpp>

#include <dbus-c++/dbus.h>
#include <dbus-c++/asio-integration.h>

#include "dbus_asio-glue.h"

class DBusBrowser
  : public org::freedesktop::DBus_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
public:

  DBusBrowser(::DBus::Connection &conn);

private:

  void NameOwnerChanged(const std::string &, const std::string &, const std::string &);

  void NameLost(const std::string &);

  void NameAcquired(const std::string &);
  

};

#endif//__DEMO_DBUS_BROWSER_H
