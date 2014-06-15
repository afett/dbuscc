/*
	this file is part of the dbuscc library

	Copyright (c) 2014 Andreas Fett.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
	TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdlib>

#include <dbuscc/bus.h>
#include <dbuscc/glue/connection.h>
#include <dbuscc/glue/error.h>

namespace {

::DBusBusType bus_type(dbuscc::bus::Type type)
{
	switch (type) {
	case dbuscc::bus::TYPE_SESSION:
		return DBUS_BUS_SESSION;
	case dbuscc::bus::TYPE_SYSTEM:
		return DBUS_BUS_SYSTEM;
	case dbuscc::bus::TYPE_STARTER:
		return DBUS_BUS_STARTER;
	}

	return DBUS_BUS_SESSION;
}

}

namespace dbuscc {
namespace internal {

class bus : public dbuscc::bus {
public:
	connection_ptr open(std::string const&, error &);
	connection_ptr open_private(std::string const&, error &);
	connection_ptr open(Type, error &);
	connection_ptr open_private(Type, error &);
};

connection_ptr
bus::open(std::string const& address, error & e)
{
	DBusConnection *conn(
		dbus_connection_open(address.c_str(), glue::err(e)));
	return !conn ? connection_ptr() :
		glue::connection::create_shared(conn);
}

connection_ptr
bus::open_private(std::string const& address, error & e)
{
	DBusConnection *conn(
		dbus_connection_open_private(address.c_str(), glue::err(e)));
	return !conn ? connection_ptr() :
		glue::connection::create_private(conn);
}

connection_ptr
bus::open(Type bus, error & e)
{
	DBusConnection *conn(
		dbus_bus_get(::bus_type(bus), glue::err(e)));
	return !conn ? connection_ptr() :
		glue::connection::create_shared(conn);
}

connection_ptr
bus::open_private(Type bus, error & e)
{
	DBusConnection *conn(
		dbus_bus_get_private(::bus_type(bus), glue::err(e)));
	return !conn ? connection_ptr() :
		glue::connection::create_private(conn);
}

} // namespace internal

bus_ptr bus::create()
{
	return bus_ptr(new internal::bus());
}

std::string session_bus_address()
{
	char *addr(getenv("DBUS_SESSION_BUS_ADDRESS"));
	return addr ? addr : "";
}

}
