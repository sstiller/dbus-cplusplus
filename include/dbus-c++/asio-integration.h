/*
 *
 *  D-Bus++ - C++ bindings for D-Bus
 *
 *  Copyright (C) 2016  Sandro Stiller <sstiller@elfin.de>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef __DBUSXX_ASIO_INTEGRATION_H
#define __DBUSXX_ASIO_INTEGRATION_H


#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "api.h"
#include "dispatcher.h"

namespace DBus
{

namespace Asio
{

class BusDispatcher;

class DXXAPI BusTimeout : public Timeout
{
private:

  BusTimeout(Timeout::Internal *, boost::asio::io_service& ioService);

  ~BusTimeout();

  void toggle();

  void timeout_handler(const boost::system::error_code& ec);

  void _enable();

  void _disable();

private:
  boost::asio::io_service& ioService;
  boost::asio::deadline_timer timer;
  friend class BusDispatcher;
};

class DXXAPI BusWatch : public Watch
{
private:

  BusWatch(Watch::Internal *, boost::asio::io_service& ioService, BusDispatcher* bd);

  ~BusWatch();

  void toggle();

  void _enable();

  void _disable();

private:
  boost::asio::io_service& ioService;
  boost::asio::posix::stream_descriptor dbusFdStream;
  bool dbusFdStreamEnabled;
  void watchHandlerRead(const boost::system::error_code& error);
  void watchHandlerWrite(const boost::system::error_code& error);
  BusDispatcher* bd;
  
  friend class BusDispatcher;
};

class DXXAPI BusDispatcher : public Dispatcher
{
public:
  BusDispatcher(boost::asio::io_service& ioService);

  void enter() {}

  void leave() {}

  Timeout *add_timeout(Timeout::Internal *);

  void rem_timeout(Timeout *);

  Watch *add_watch(Watch::Internal *);

  void rem_watch(Watch *);

private:
  boost::asio::io_service& ioService;
  std::map<Watch*, boost::shared_ptr<Watch> > watchPtrMap;
};

} /* namespace Asio */

} /* namespace DBus */

#endif//__DBUSXX_ASIO_INTEGRATION_H
