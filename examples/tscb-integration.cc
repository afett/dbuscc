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

#include <dbuscc/bus.h>
#include <dbuscc/error.h>
#include <dbuscc/connection.h>
#include <dbuscc/object-path.h>
#include <dbuscc/interface.h>
#include <dbuscc/member.h>
#include <dbuscc/bus-name.h>
#include <dbuscc/signal-message.h>
#include <dbuscc/call-message.h>
#include <dbuscc/pending-call.h>
#include <dbuscc/message-reader.h>
#include <dbuscc/return-message.h>

#include <dbuscc/tscb/adapter.h>
#include <tscb/dispatch>

#include <iostream>
#include <boost/bind.hpp>

void on_hello_reply(dbuscc::pending_call_ptr const& reply, bool & stop)
{
	std::cerr << __PRETTY_FUNCTION__ << "\n";
	if (reply->reply_error()) {
		std::cerr << "got error:" << reply->reply_error().name() << "\n";
	} else {
		std::cerr << "Success:\n";
		dbuscc::return_message_ptr reply_msg(reply->reply());
		dbuscc::message_reader r(reply_msg->create_reader());
		std::string unique_name;
		if (r.pop_string(unique_name)) {
			std::cerr << "out name is:" << unique_name << "\n";
		} else {
			std::cerr << "failed to get return value\n";
		}
	}

	stop = true;
}

int main()
{
	tscb::posix_reactor reactor;
	dbuscc::bus_ptr dbus(dbuscc::bus::create());
	dbuscc::error err;
	dbuscc::connection_ptr conn(
		dbus->open_private(dbuscc::session_bus_address(), err));
	dbuscc::tscb_adapter adapter(reactor, conn);

	dbuscc::call_message_ptr hello(
		dbuscc::call_message::create(
			dbuscc::bus_name("org.freedesktop.DBus"),
			dbuscc::object_path("/org/freedesktop/DBus"),
			dbuscc::interface("org.freedesktop.DBus"),
			dbuscc::member("Hello")));
	dbuscc::pending_call_ptr hello_reply(conn->call(hello));

	bool stop(false);
	tscb::scoped_connection c(hello_reply->on_completion().connect(boost::bind(
		on_hello_reply, hello_reply, boost::ref(stop))));

	/*

	dbuscc::message_ptr msg(
		dbuscc::signal_message::create(
			dbuscc::object_path("/foo"),
			dbuscc::interface("foo.bar"),
			dbuscc::member("my_signal")));

	if (!conn->send(msg)) {
		return 1;
	}

	*/

	while(!stop) {
		reactor.dispatch();
	}

	return 0;
}
