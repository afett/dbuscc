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

#include <dbuscc/glue/message.h>
#include <dbuscc/glue/message-writer.h>

#include "xassert.h"

namespace dbuscc {

message_writer::message_writer(message_ptr const& msg)
:
	msg_(msg),
	impl_(0)
{
	DBUSCC_ASSERT(msg_);
	impl_.reset(new glue::message_writer(msg->glue().raw()));
}

message_writer::message_writer(message_writer const& o)
:
	impl_(new glue::message_writer(*o.impl_))
{ }

message_writer & message_writer::operator=(message_writer const& o)
{
	if (&o != this) {
		impl_.reset(new glue::message_writer(*o.impl_));
	}

	return *this;
}

glue::message_writer & message_writer::glue() const
{
	return *impl_;
}

namespace glue {

message_writer::message_writer(DBusMessage *msg)
:
	msg_(msg)
{
	DBUSCC_ASSERT(msg_);
	dbus_message_iter_init_append(msg, &raw_);
}

message_writer::~message_writer()
{ }

}}
