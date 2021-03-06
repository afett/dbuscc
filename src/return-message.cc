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

class return_message :
	public glue::message,
	public dbuscc::return_message
{
public:
	return_message(DBusMessage *);
	~return_message();
	message_reader create_reader();
	message_writer create_writer();
	glue::message & glue();
	DBusMessage *raw();

private:
	DBusMessage *raw_;
};

return_message::return_message(DBusMessage *raw)
:
	raw_(raw)
{
	DBUSCC_ASSERT(raw_);
}

return_message::~return_message()
{
	dbus_message_unref(raw_);
	raw_ = 0;
}

message_reader return_message::create_reader()
{
	return message_reader(this);
}

message_writer return_message::create_writer()
{
	return message_writer(this);
}

glue::message & return_message::glue()
{
	return *this;
}

DBusMessage*
return_message::raw()
{
	return raw_;
}

} // internal

namespace glue {

return_message_ptr
message::create_return(DBusMessage *raw)
{
	return new internal::return_message(raw);
}

}}
