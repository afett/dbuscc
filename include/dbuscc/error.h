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

#ifndef DBUSCC_ERROR_H
#define DBUSCC_ERROR_H

#include <string>
#include <dbuscc/pointer.h>

namespace dbuscc {

class error {
public:
	enum Type {
		TYPE_FAILED,
		TYPE_NO_MEMORY,
		TYPE_SERVICE_UNKNOWN,
		TYPE_NAME_HAS_NO_OWNER,
		TYPE_NO_REPLY,
		TYPE_IO_ERROR,
		TYPE_BAD_ADDRESS,
		TYPE_NOT_SUPPORTED,
		TYPE_LIMITS_EXCEEDED,
		TYPE_ACCESS_DENIED,
		TYPE_AUTH_FAILED,
		TYPE_NO_SERVER,
		TYPE_TIMEOUT,
		TYPE_NO_NETWORK,
		TYPE_ADDRESS_IN_USE,
		TYPE_DISCONNECTED,
		TYPE_INVALID_ARGS,
		TYPE_FILE_NOT_FOUND,
		TYPE_FILE_EXISTS,
		TYPE_UNKNOWN_METHOD,
		TYPE_UNKNOWN_OBJECT,
		TYPE_UNKNOWN_INTERFACE,
		TYPE_UNKNOWN_PROPERTY,
		TYPE_PROPERTY_READ_ONLY,
		TYPE_TIMED_OUT,
		TYPE_MATCH_RULE_NOT_FOUND,
		TYPE_MATCH_RULE_INVALID,
		TYPE_SPAWN_EXEC_FAILED,
		TYPE_SPAWN_FORK_FAILED,
		TYPE_SPAWN_CHILD_EXITED,
		TYPE_SPAWN_CHILD_SIGNALED,
		TYPE_SPAWN_FAILED,
		TYPE_SPAWN_SETUP_FAILED,
		TYPE_SPAWN_CONFIG_INVALID,
		TYPE_SPAWN_SERVICE_INVALID,
		TYPE_SPAWN_SERVICE_NOT_FOUND,
		TYPE_SPAWN_PERMISSIONS_INVALID,
		TYPE_SPAWN_FILE_INVALID,
		TYPE_SPAWN_NO_MEMORY,
		TYPE_UNIX_PROCESS_ID_UNKNOWN,
		TYPE_INVALID_SIGNATURE,
		TYPE_INVALID_FILE_CONTENT,
		TYPE_SELINUX_SECURITY_CONTEXT_UNKNOWN,
		TYPE_ADT_AUDIT_DATA_UNKNOWN,
		TYPE_OBJECT_PATH_IN_USE,
		TYPE_INCONSISTENT_MESSAGE,
		TYPE_UNKNOWN,
	};

	error();
	~error();

	typedef bool (error::*unspecified_boolean_type)() const;
	operator unspecified_boolean_type() const
	{ return is_set() ? &error::is_set : 0; }

	bool is_set() const;
	std::string name() const;
	std::string message() const;
	Type type() const;

	glue::error & glue() const;
private:
	error(error const&); // = deleted
	error & operator=(error const&); // = deleted

	DBUSCC_SCOPED_PTR(glue::error) impl_;
};

}

#endif
