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

#include <dbuscc/glue/call-message.h>
#include <dbuscc/glue/pending-call.h>
#include <dbuscc/glue/watch.h>
#include <dbuscc/glue/connection.h>

#include "ptr-wrapper.h"
#include "xassert.h"

namespace dbuscc {
namespace internal {

class connection :
	public glue::connection,
	public DBUSCC_SHARED_FROM_THIS(connection)
{
public:
	connection(DBusConnection *);
	~connection();
	bool send(message_ptr const&);
	pending_call_ptr call(call_message_ptr const&);

	DBUSCC_SIGNAL(void(watch_weak_ptr)) & on_watch_add();

	glue::connection & glue();
	DBusConnection *raw();
	bool send(DBusMessage *);
	DBusPendingCall *call(DBusMessage *);

protected:
	DBUSCC_SIGNAL(void(watch_weak_ptr)) on_watch_add_;
	DBusConnection *raw_;

private:
	typedef DBUSCC_WEAK_PTR(connection) connection_weak_ptr;
	typedef DBUSCC_SHARED_PTR(connection) connection_ptr;

	struct wrapper : public ptr_wrapper<connection_weak_ptr> {
		wrapper(connection_weak_ptr const& ptr)
		: ptr_wrapper<connection_weak_ptr>(ptr)
		{ }
	};

	static dbus_bool_t add_watch(DBusWatch *, void *);
	void install_watch_handler();

	bool watch_handler_installed_;
};

class shared_connection : public connection {
public:
	shared_connection(DBusConnection *);
	void close();
};

class private_connection : public connection {
public:
	~private_connection();
	private_connection(DBusConnection *);
	void close();
};

connection::connection(DBusConnection *raw)
:
	raw_(raw),
	watch_handler_installed_(false)
{
	DBUSCC_ASSERT(raw_);
}

shared_connection::shared_connection(DBusConnection *raw)
:
	connection(raw)
{ }

void shared_connection::close()
{
	DBUSCC_ASSERT(false && "attempt to close a shared connection");
}

private_connection::private_connection(DBusConnection *raw)
:
	connection(raw)
{ }

private_connection::~private_connection()
{
	close();
}

void private_connection::close()
{
	dbus_connection_close(raw_);
}

bool connection::send(message_ptr const& msg)
{
	DBUSCC_ASSERT(msg);
	return send(msg->glue().raw());
}

bool connection::send(DBusMessage *msg)
{
	DBUSCC_ASSERT(msg);
	// we don't pass in the serial as sending will set the serial
	// on the message and we can get it from there
	return dbus_connection_send(raw_, msg, NULL);
}

pending_call_ptr connection::call(call_message_ptr const& msg)
{
	DBUSCC_ASSERT(msg);
	return glue::pending_call::create(call(msg->glue().raw()));
}

DBusPendingCall *connection::call(DBusMessage *msg)
{
	DBUSCC_ASSERT(msg);
	DBusPendingCall *pending_return(0);
	bool success(dbus_connection_send_with_reply(
		raw_, msg, &pending_return, -1));
	if (!success) {
		return NULL;
	}

	DBUSCC_ASSERT(pending_return);
	return pending_return;
}

void connection::install_watch_handler()
{
	if (watch_handler_installed_) {
		return;
	}

	dbus_connection_set_watch_functions(
		raw_,
		&connection::add_watch,
		&glue::watch::removed,
		&glue::watch::toggled,
		new wrapper(shared_from_this()),
		&wrapper::delete_wrapper);

	watch_handler_installed_ = true;
}

dbus_bool_t connection::add_watch(DBusWatch *raw_watch, void *data)
{
	DBUSCC_ASSERT(data);
	DBUSCC_ASSERT(raw_watch);

	connection_ptr self(wrapper::self(data)->ptr_.lock());
	if (self) {
		self->on_watch_add_(glue::watch::create(raw_watch));
	}

	return true;
}

DBUSCC_SIGNAL(void(watch_weak_ptr)) & connection::on_watch_add()
{
	install_watch_handler();
	return on_watch_add_;
}

connection::~connection()
{
	dbus_connection_unref(raw_);
}

glue::connection & connection::glue()
{
	return *this;
}

DBusConnection*
connection::raw()
{
	return raw_;
}

} // internal

namespace glue {

connection_ptr
connection::create_private(DBusConnection *raw)
{
	return connection_ptr(new internal::private_connection(raw));
}

connection_ptr
connection::create_shared(DBusConnection *raw)
{
	return connection_ptr(new internal::shared_connection(raw));
}

}}
