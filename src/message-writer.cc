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

#include <dbuscc/object-path.h>
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

message_writer::~message_writer()
{
	impl_.reset();
}

bool message_writer::push_uint8(uint8_t v)
{
	return impl_->append_basic(DBUS_TYPE_BYTE, &v);
}

bool message_writer::push_bool(bool v)
{
	return impl_->append_basic(DBUS_TYPE_BOOLEAN, &v);
}

bool message_writer::push_int16(int16_t v)
{
	return impl_->append_basic(DBUS_TYPE_INT16, &v);
}

bool message_writer::push_uint16(uint16_t v)
{
	return impl_->append_basic(DBUS_TYPE_UINT16, &v);
}

bool message_writer::push_int32(int32_t v)
{
	return impl_->append_basic(DBUS_TYPE_INT32, &v);
}

bool message_writer::push_uint32(uint32_t v)
{
	return impl_->append_basic(DBUS_TYPE_UINT32, &v);
}

bool message_writer::push_int64(int64_t v)
{
	return impl_->append_basic(DBUS_TYPE_INT64, &v);
}

bool message_writer::push_uint64(uint64_t v)
{
	return impl_->append_basic(DBUS_TYPE_UINT64, &v);
}

bool message_writer::push_double(double v)
{
	return impl_->append_basic(DBUS_TYPE_DOUBLE, &v);
}

bool message_writer::push_string(std::string const& v)
{
	const char *s(v.c_str());
	return impl_->append_basic(DBUS_TYPE_STRING, &s);
}

bool message_writer::push_object_path(object_path const& v)
{
	const char *s(v.c_str());
	return impl_->append_basic(DBUS_TYPE_OBJECT_PATH, &s);
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

bool message_writer::append_basic(int type, const void *value)
{
	return dbus_message_iter_append_basic(&raw_, type, value);
}

}}
