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

#include <iostream>

#include <dbuscc/glue/connection.h>
#include <dbuscc/glue/message.h>
#include <dbuscc/glue/pending-call.h>
#include <dbuscc/glue/timeout.h>
#include <dbuscc/glue/watch.h>

#include "trace.h"
#include "xassert.h"

namespace {

dbuscc::connection::DispatchState dispatch_state(DBusDispatchStatus status)
{
	switch (status) {
	case DBUS_DISPATCH_DATA_REMAINS:
		return dbuscc::connection::DISPATCH_DATA_REMAINS;
	case DBUS_DISPATCH_COMPLETE:
		return dbuscc::connection::DISPATCH_COMPLETE;
	case DBUS_DISPATCH_NEED_MEMORY:
		return dbuscc::connection::DISPATCH_NEED_MEMORY;
	}

	DBUSCC_ASSERT(false);
	return dbuscc::connection::DISPATCH_NEED_MEMORY;
}

}

namespace dbuscc {
namespace internal {

class connection :
	public dbuscc::connection,
	public glue::connection
{
public:
	connection(DBusConnection *);
	~connection();
	bool send(message_ptr const&);
	pending_call_ptr call(call_message_ptr const&);
	DispatchState dispatch_state() const;
	DispatchState dispatch();

	void install_handlers();

	DBUSCC_SIGNAL(void(watch_ptr)) & on_watch_add();
	DBUSCC_SIGNAL(void(timeout_ptr)) & on_timeout_add();
	DBUSCC_SIGNAL(void(DispatchState)) & on_dispatch_state();

	glue::connection & glue();
	DBusConnection *raw();
	bool send(DBusMessage *);
	DBusPendingCall *call(DBusMessage *);

protected:
	DBUSCC_SIGNAL(void(watch_ptr)) on_watch_add_;
	DBUSCC_SIGNAL(void(timeout_ptr)) on_timeout_add_;
	DBUSCC_SIGNAL(void(DispatchState)) on_dispatch_state_;
	DBusConnection *raw_;

private:
	static dbus_bool_t add_watch(DBusWatch *, void *);
	void install_watch_handler();

	static dbus_bool_t add_timeout(DBusTimeout *, void *);
	void install_timeout_handler();

	static void dispatch_state_changed(DBusConnection *, DBusDispatchStatus, void *);
	void install_dispatch_handler();

	bool watch_handler_installed_;
	bool timeout_handler_installed_;
	bool dispatch_handler_installed_;
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
	watch_handler_installed_(false),
	timeout_handler_installed_(false),
	dispatch_handler_installed_(false)
{
	DBUSCC_ASSERT(raw_);
}

shared_connection::shared_connection(DBusConnection *raw)
:
	connection(raw)
{ }

void shared_connection::close()
{
	DBUSCC_TRACE();
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
	DBUSCC_TRACE();
	dbus_connection_close(raw_);
}

bool connection::send(message_ptr const& msg)
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(msg);
	return send(msg->glue().raw());
}

bool connection::send(DBusMessage *msg)
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(msg);
	// we don't pass in the serial as sending will set the serial
	// on the message and we can get it from there
	return dbus_connection_send(raw_, msg, NULL);
}

pending_call_ptr connection::call(call_message_ptr const& msg)
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(msg);
	return glue::pending_call::create(call(msg->glue().raw()));
}

DBusPendingCall *connection::call(DBusMessage *msg)
{
	DBUSCC_TRACE();
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

void connection::install_handlers()
{
	DBUSCC_TRACE();
	install_watch_handler();
	install_timeout_handler();
	install_dispatch_handler();
}

void connection::install_watch_handler()
{
	DBUSCC_TRACE();
	if (watch_handler_installed_) {
		return;
	}

	dbus_connection_set_watch_functions(
		raw_,
		&connection::add_watch,
		&glue::watch::removed,
		&glue::watch::toggled,
		this,
		NULL // no deleter as we destroy the DBusConnection ourselfs
		);

	watch_handler_installed_ = true;
}

dbus_bool_t connection::add_watch(DBusWatch *raw_watch, void *data)
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(data);
	DBUSCC_ASSERT(raw_watch);

	connection *self(static_cast<connection *>(data));
	self->on_watch_add_(glue::watch::create(raw_watch));
	return true;
}

DBUSCC_SIGNAL(void(watch_ptr)) & connection::on_watch_add()
{
	DBUSCC_TRACE();
	return on_watch_add_;
}

void connection::install_timeout_handler()
{
	DBUSCC_TRACE();
	if (timeout_handler_installed_) {
		return;
	}

	dbus_connection_set_timeout_functions(
		raw_,
		&connection::add_timeout,
		&glue::timeout::removed,
		&glue::timeout::toggled,
		this,
		NULL // no deleter as we destroy the DBusConnection ourselfs
	);

	timeout_handler_installed_ = true;
}

dbus_bool_t connection::add_timeout(DBusTimeout *raw_timeout, void *data)
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(data);
	DBUSCC_ASSERT(raw_timeout);

	connection *self(static_cast<connection *>(data));
	self->on_timeout_add_(glue::timeout::create(raw_timeout));
	return true;
}

DBUSCC_SIGNAL(void(timeout_ptr)) & connection::on_timeout_add()
{
	DBUSCC_TRACE();
	return on_timeout_add_;
}

connection::DispatchState connection::dispatch_state() const
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(raw_);
	return ::dispatch_state(
		dbus_connection_get_dispatch_status(raw_));
}

connection::DispatchState connection::dispatch()
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(raw_);
	return ::dispatch_state(
		dbus_connection_dispatch(raw_));
}

void connection::install_dispatch_handler()
{
	DBUSCC_TRACE();
	if (dispatch_handler_installed_) {
		return;
	}

	dbus_connection_set_dispatch_status_function(
		raw_,
		&connection::dispatch_state_changed,
		this,
		NULL // no deleter as we destroy the DBusConnection ourselfs
	);

	dispatch_handler_installed_ = true;
}

void connection::dispatch_state_changed(
	DBusConnection *, DBusDispatchStatus new_state, void *data)
{
	DBUSCC_TRACE();
	DBUSCC_ASSERT(data);

	connection *self(static_cast<connection *>(data));
	self->on_dispatch_state_(::dispatch_state(new_state));
}

DBUSCC_SIGNAL(void(connection::DispatchState)) & connection::on_dispatch_state()
{
	DBUSCC_TRACE();
	return on_dispatch_state_;
}

connection::~connection()
{
	dbus_connection_unref(raw_);
	raw_ = 0;
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

}

std::ostream & connection::print(std::ostream & os, DispatchState state)
{
	switch (state) {
	case DISPATCH_DATA_REMAINS: os << "DISPATCH_DATA_REMAINS"; break;
	case DISPATCH_COMPLETE:     os << "DISPATCH_COMPLETE"; break;
	case DISPATCH_NEED_MEMORY:  os << "DISPATCH_NEED_MEMORY"; break;
	}
	return os;
}

}
