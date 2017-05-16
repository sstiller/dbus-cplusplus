/*
 *
 *  D-Bus++ - C++ bindings for D-Bus
 *
 *  Copyright (C) 2016  Sandro Stiller <sandro.stiller@elfin.de>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory>
#include <boost/bind.hpp>
#include <dbus-c++/asio-integration.h>

#include <boost/make_shared.hpp>

#include <dbus/dbus.h> // for DBUS_WATCH_*


using namespace DBus;

Asio::BusTimeout::BusTimeout(Timeout::Internal *ti, boost::asio::io_service& ioService)
  : Timeout(ti),
    ioService(ioService),
    timer(ioService)
{
  if (Timeout::enabled())
  {
    _enable();
  }
}

Asio::BusTimeout::~BusTimeout()
{
  _disable();
}

void Asio::BusTimeout::toggle()
{
  debug_log("asio: timeout %p toggled (%s)", this, Timeout::enabled() ? "on" : "off");

  if (Timeout::enabled())
  {
    _enable();
  }
  else
  {
    _disable();
  }
}

void Asio::BusTimeout::timeout_handler(const boost::system::error_code& ec)
{
  if (!ec)
  {
    // Timer expired
    handle();
    _enable();
  }
}

void Asio::BusTimeout::_enable()
{
  debug_log("Asio::BusTimeout::_enable(), %i millis", Timeout::interval());
  timer.expires_from_now(boost::posix_time::millisec(Timeout::interval()));
  timer.async_wait(boost::bind(&Asio::BusTimeout::timeout_handler, this, _1));
}

void Asio::BusTimeout::_disable()
{
  debug_log("Asio::BusTimeout::_disable()");
  timer.cancel();
}

// BusWatch

Asio::BusWatch::BusWatch(Watch::Internal *wi, boost::asio::io_service& ioService, BusDispatcher* bd)
: Watch(wi),
  ioService(ioService),
  dbusFdStream(ioService),
  dbusFdStreamEnabled(false),
  bd(bd)
{
  if (Watch::enabled())
  {
    _enable();
  }
}

Asio::BusWatch::~BusWatch()
{
  _disable();
}

void Asio::BusWatch::toggle()
{
  debug_log("asio: watch %p toggled (%s)", this, Watch::enabled() ? "on" : "off");

  if (Watch::enabled())
  {
    _enable();
  }else{
    _disable();
  }
}

void Asio::BusWatch::watchHandlerRead(const boost::system::error_code& error)
{
  debug_log("%s called, ptr=%p", __PRETTY_FUNCTION__, this);

  if(!error)
  {
    handle(DBUS_WATCH_READABLE);
  }else{
    if(error == boost::asio::error::operation_aborted)
    {
      // no restart because cancelled.
      return;
    }else if((error == boost::asio::error::network_down) ||
       (error == boost::asio::error::network_reset) ||
       (error == boost::asio::error::connection_reset) ||
       (error == boost::asio::error::broken_pipe) ||
       (error == boost::asio::error::connection_aborted))
    {
      handle(DBUS_WATCH_HANGUP);
    }else{
      handle(DBUS_WATCH_ERROR);
    }
  }

  bd->dispatch_pending();

  dbusFdStream.async_read_some(boost::asio::null_buffers(),
                               boost::bind(&Asio::BusWatch::watchHandlerRead,
                                           this,
                                           boost::asio::placeholders::error
                                          )
                              );    
  
}

void Asio::BusWatch::watchHandlerWrite(const boost::system::error_code& error)
{
  debug_log("%s called, ptr=%p", __PRETTY_FUNCTION__, this);

  if(!error)
  {
    handle(DBUS_WATCH_WRITABLE);
  }else{
    if(error == boost::asio::error::operation_aborted)
    {
      // no restart because cancelled.
      return;
    }else if((error == boost::asio::error::network_down) ||
       (error == boost::asio::error::network_reset) ||
       (error == boost::asio::error::connection_reset) ||
       (error == boost::asio::error::broken_pipe) ||
       (error == boost::asio::error::connection_aborted))
    {
      handle(DBUS_WATCH_HANGUP);
    }else{
      handle(DBUS_WATCH_ERROR);
    }
  }
  bd->dispatch_pending();

  dbusFdStream.async_write_some(boost::asio::null_buffers(),
                                boost::bind(&Asio::BusWatch::watchHandlerWrite,
                                            this,
                                            boost::asio::placeholders::error
                                           )
                               );
}

void Asio::BusWatch::_enable()
{
  debug_log("%s called, fd = %i", __PRETTY_FUNCTION__, descriptor());
  //TODO: Flag for read/write detection?
  int flags = Watch::flags();
  dbusFdStream.assign(boost::asio::local::stream_protocol(),
                      descriptor());
  dbusFdStreamEnabled = true;
  if (flags & DBUS_WATCH_READABLE)
  {
    dbusFdStream.async_read_some(boost::asio::null_buffers(),
                                 boost::bind(&Asio::BusWatch::watchHandlerRead,
                                             this,
                                             boost::asio::placeholders::error
                                            )
                                );
  }
  
  if (flags & DBUS_WATCH_WRITABLE)
  {
    dbusFdStream.async_write_some(boost::asio::null_buffers(),
                                  boost::bind(&Asio::BusWatch::watchHandlerWrite,
                                              this,
                                              boost::asio::placeholders::error
                                             )
                                 );
   }
}

void Asio::BusWatch::_disable()
{
  if(dbusFdStreamEnabled)
  {
    dbusFdStream.cancel();
    dbusFdStreamEnabled = false;
  }
}

Asio::BusDispatcher::BusDispatcher(boost::asio::io_service& ioService)
: ioService(ioService)
{
}

Timeout *Asio::BusDispatcher::add_timeout(Timeout::Internal *wi)
{
  Timeout *t = new Asio::BusTimeout(wi, ioService);

  debug_log("asio: added timeout %p (%sabled)", t, t->enabled() ? "en" : "dis");

  return t;
}

void Asio::BusDispatcher::rem_timeout(Timeout *t)
{
  debug_log("asio: removed timeout %p", t);

  delete t;
}

Watch *Asio::BusDispatcher::add_watch(Watch::Internal *wi)
{
  Watch* watchPtr = new Asio::BusWatch(wi, ioService, this);
  boost::shared_ptr<Watch> sharedWatchPtr(watchPtr);
  watchPtrMap[watchPtr] = sharedWatchPtr;

  debug_log("asio: added watch %p (%sabled) fd=%d flags=0x%02x",
            watchPtr,
            watchPtr->enabled() ? "en" : "dis",
            watchPtr->descriptor(),
            watchPtr->flags()
           );
  return watchPtr;
}

void Asio::BusDispatcher::rem_watch(Watch *w)
{
  debug_log("asio: removed watch %p", w);
  watchPtrMap.erase(w);
}
