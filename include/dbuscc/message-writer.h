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

#ifndef DBUSCC_MESSAGE_WRITER_H
#define DBUSCC_MESSAGE_WRITER_H

#include <stdint.h>
#include <string>
#include <dbuscc/common.h>

namespace dbuscc {

class message_writer {
public:
	message_writer(message_ptr const&);
	message_writer(message_writer const&);
	message_writer & operator=(message_writer const&);

	bool push_uint8(uint8_t);
	bool push_bool(bool);
	bool push_int16(int16_t);
	bool push_uint16(uint16_t);
	bool push_int32(int32_t);
	bool push_uint32(uint32_t);
	bool push_int64(int64_t);
	bool push_uint64(uint64_t);
	bool push_double(double);
	bool push_string(std::string const&);
	bool push_object_path(object_path const&);

	glue::message_writer & glue() const;

private:
	message_ptr msg_;
	DBUSCC_SCOPED_PTR(glue::message_writer) impl_;
};

}

#endif
