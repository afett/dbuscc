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
#include <dbuscc/glue/message-reader.h>
#include <dbuscc/glue/message-writer.h>

#include "xassert.h"

namespace dbuscc {
namespace internal {

class error_message :
	public dbuscc::error_message,
	public glue::message
{
public:
	error_message(DBusMessage *);
	~error_message();
	std::string error_name() const;
	message_reader create_reader();
	message_writer create_writer();
	glue::message & glue();
	DBusMessage *raw();

private:
	DBusMessage *raw_;
};

error_message::error_message(DBusMessage *raw)
:
	raw_(raw)
{
	DBUSCC_ASSERT(raw_);
}

error_message::~error_message()
{
	dbus_message_unref(raw_);
}

message_reader error_message::create_reader()
{
	return message_reader(this);
}

message_writer error_message::create_writer()
{
	return message_writer(this);
}

std::string error_message::error_name() const
{
	DBUSCC_ASSERT(raw_);
	const char *name(dbus_message_get_error_name(raw_));
	return name ? name : "";
}

glue::message & error_message::glue()
{
	return *this;
}

DBusMessage*
error_message::raw()
{
	return raw_;
}

} // internal

namespace glue {

error_message_ptr
message::create_error(DBusMessage *raw)
{
	return new internal::error_message(raw);
}

}}
