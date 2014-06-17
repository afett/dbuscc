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

#include <dbuscc/glue/pending-call.h>
#include <dbuscc/glue/message.h>
#include <dbuscc/glue/error.h>

#include "xassert.h"

namespace dbuscc {
namespace internal {

class pending_call :
	public dbuscc::pending_call,
	public glue::pending_call
{
public:
	pending_call(DBusPendingCall *);
	~pending_call();

	State state() const;
	void cancel();
	return_message_ptr reply() const;
	error & reply_error();
	DBUSCC_SIGNAL(void(void)) & on_completion();

	glue::pending_call & glue();
	DBusPendingCall *raw();

	void notify();

private:
	DBusPendingCall *raw_;
	State state_;
	return_message_ptr reply_message_;
	error error_;
	DBUSCC_SIGNAL(void(void)) on_completion_;
};

pending_call::pending_call(DBusPendingCall *raw)
:
	raw_(raw),
	state_(STATE_PENDING),
	reply_message_(),
	error_()
{
	DBUSCC_ASSERT(raw_);

	dbus_pending_call_set_notify(
		raw_,
		&glue::pending_call::notify,
		this,
		NULL);
}

pending_call::~pending_call()
{
	DBUSCC_ASSERT(raw_);
	// cancel the call so that libdbus yields its reference.
	cancel();
	dbus_pending_call_unref(raw_);
	raw_ = 0;
}

void pending_call::cancel()
{
	DBUSCC_ASSERT(raw_);
	dbus_pending_call_cancel(raw_);
	state_ = STATE_CANCELED;
}

void pending_call::notify()
{
	DBUSCC_ASSERT(raw_);
	DBUSCC_ASSERT(dbus_pending_call_get_completed(raw_));
	DBusMessage *raw_reply(dbus_pending_call_steal_reply(raw_));
	// for a timeout an artifical error message will
	// be created, so we should allways get a message on completion.
	DBUSCC_ASSERT(raw_reply);

	// we may get a message in calceled state if it was
	// received before cancel was called....
	if (state_ == STATE_CANCELED) {
		dbus_message_unref(raw_reply);
		// no notification here
		return;
	}

	DBUSCC_ASSERT(state_ == STATE_PENDING);
	if (dbus_set_error_from_message(error_.glue().raw(), raw_reply)) {
		state_ = (error_.type() == error::TYPE_NO_REPLY) ?
			STATE_TIMEOUT : STATE_ERROR;
	} else if (dbus_message_get_type(raw_reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN) {
		reply_message_ = glue::message::create_return(raw_reply);
	}
	// the dbus api docs say we must handle all
	// message types gracefully. so we'll just call
	// notify an do not deliver a reply if it's not
	// an error or a method return.
	state_ = STATE_COMPLETED;
	dbus_message_unref(raw_reply);
	on_completion_();
}

DBUSCC_SIGNAL(void(void)) & pending_call::on_completion()
{
	return on_completion_;
}

pending_call::State pending_call::state() const
{
	return state_;
}

return_message_ptr pending_call::reply() const
{
	return reply_message_;
}

error & pending_call::reply_error()
{
	return error_;
}

glue::pending_call & pending_call::glue()
{
	return *this;
}

DBusPendingCall*
pending_call::raw()
{
	return raw_;
}

} // internal

namespace glue {

pending_call_ptr
pending_call::create(DBusPendingCall *raw)
{
	return pending_call_ptr(new internal::pending_call(raw));
}

void pending_call::notify(DBusPendingCall *, void *data)
{
	DBUSCC_ASSERT(data);
	internal::pending_call *self(
		static_cast<internal::pending_call*>(data));
	self->notify();
}

}}
