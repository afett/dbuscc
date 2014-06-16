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

#ifndef DBUSCC_MESSAGE_READER_H
#define DBUSCC_MESSAGE_READER_H

#include <stdint.h>
#include <string>
#include <dbuscc/pointer.h>

namespace dbuscc {

class message_reader {
public:
	message_reader(message_ptr const&);
	message_reader(message_reader const&);
	message_reader & operator=(message_reader const&);
	~message_reader();

	bool pop_uint8(uint8_t &);
	bool pop_bool(bool &);
	bool pop_int16(int16_t &);
	bool pop_uint16(uint16_t &);
	bool pop_int32(int32_t &);
	bool pop_uint32(uint32_t &);
	bool pop_int64(int64_t &);
	bool pop_uint64(uint64_t &);
	bool pop_double(double &);
	bool pop_string(std::string &);
	bool pop_object_path(object_path &);

	glue::message_reader & glue() const;

private:
	message_ptr msg_;
	DBUSCC_SCOPED_PTR(glue::message_reader) impl_;
};

}

#endif
