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
#include <dbuscc/object-path.h>
#include <dbuscc/interface.h>
#include <dbuscc/member.h>

#include "xassert.h"

namespace dbuscc {
namespace internal {

class signal_message :
	public dbuscc::signal_message,
	public glue::message
{
public:
	signal_message(DBusMessage *);
	~signal_message();
	message_writer create_writer();
	glue::message & glue();
	DBusMessage *raw();

private:
	DBusMessage *raw_;
};

signal_message::signal_message(DBusMessage *raw)
:
	raw_(raw)
{
	DBUSCC_ASSERT(raw_);
}

signal_message::~signal_message()
{
	dbus_message_unref(raw_);
}

message_writer signal_message::create_writer()
{
	return message_writer(message_ptr(this));
}

glue::message & signal_message::glue()
{
	return *this;
}

DBusMessage*
signal_message::raw()
{
	return raw_;
}

} // internal

namespace glue {

signal_message_ptr
message::create_signal(DBusMessage *raw)
{
	return new internal::signal_message(raw);
}

signal_message_ptr
message::create_signal(const char* path, const char *iface, const char *name)
{
	return create_signal(dbus_message_new_signal(path, iface, name));
}

} // glue

signal_message_ptr signal_message::create(
	object_path const& path, interface const& iface, member const& name)
{
	return glue::message::create_signal(path.c_str(), iface.c_str(), name.c_str());
}

}
