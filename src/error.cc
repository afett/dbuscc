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

#include <dbuscc/error.h>
#include <dbuscc/glue/error.h>

namespace dbuscc {

error::error()
:
	impl_(new glue::error())
{ }

error::~error()
{
	impl_.reset();
}

bool error::is_set() const
{
	return dbus_error_is_set(impl_->raw());
}

std::string error::name() const
{
	return impl_->raw()->name;
}

std::string error::message() const
{
	return impl_->raw()->message;
}

error::Type error::type() const
{
	// sigh ... libdbus error handling is a mess
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_FAILED))
		return TYPE_FAILED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_NO_MEMORY))
		return TYPE_NO_MEMORY;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SERVICE_UNKNOWN))
		return TYPE_SERVICE_UNKNOWN;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_NAME_HAS_NO_OWNER))
		return TYPE_NAME_HAS_NO_OWNER;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_NO_REPLY))
		return TYPE_NO_REPLY;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_IO_ERROR))
		return TYPE_IO_ERROR;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_BAD_ADDRESS))
		return TYPE_BAD_ADDRESS;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_NOT_SUPPORTED))
		return TYPE_NOT_SUPPORTED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_LIMITS_EXCEEDED))
		return TYPE_LIMITS_EXCEEDED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_ACCESS_DENIED))
		return TYPE_ACCESS_DENIED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_AUTH_FAILED))
		return TYPE_AUTH_FAILED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_NO_SERVER))
		return TYPE_NO_SERVER;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_TIMEOUT))
		return TYPE_TIMEOUT;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_NO_NETWORK))
		return TYPE_NO_NETWORK;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_ADDRESS_IN_USE))
		return TYPE_ADDRESS_IN_USE;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_DISCONNECTED))
		return TYPE_DISCONNECTED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_INVALID_ARGS))
		return TYPE_INVALID_ARGS;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_FILE_NOT_FOUND))
		return TYPE_FILE_NOT_FOUND;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_FILE_EXISTS))
		return TYPE_FILE_EXISTS;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_UNKNOWN_METHOD))
		return TYPE_UNKNOWN_METHOD;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_UNKNOWN_OBJECT))
		return TYPE_UNKNOWN_OBJECT;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_UNKNOWN_INTERFACE))
		return TYPE_UNKNOWN_INTERFACE;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_UNKNOWN_PROPERTY))
		return TYPE_UNKNOWN_PROPERTY;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_PROPERTY_READ_ONLY))
		return TYPE_PROPERTY_READ_ONLY;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_TIMED_OUT))
		return TYPE_TIMED_OUT;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_MATCH_RULE_NOT_FOUND))
		return TYPE_MATCH_RULE_NOT_FOUND;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_MATCH_RULE_INVALID))
		return TYPE_MATCH_RULE_INVALID;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_EXEC_FAILED))
		return TYPE_SPAWN_EXEC_FAILED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_FORK_FAILED))
		return TYPE_SPAWN_FORK_FAILED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_CHILD_EXITED))
		return TYPE_SPAWN_CHILD_EXITED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_CHILD_SIGNALED))
		return TYPE_SPAWN_CHILD_SIGNALED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_FAILED))
		return TYPE_SPAWN_FAILED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_SETUP_FAILED))
		return TYPE_SPAWN_SETUP_FAILED;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_CONFIG_INVALID))
		return TYPE_SPAWN_CONFIG_INVALID;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_SERVICE_INVALID))
		return TYPE_SPAWN_SERVICE_INVALID;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_SERVICE_NOT_FOUND))
		return TYPE_SPAWN_SERVICE_NOT_FOUND;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_PERMISSIONS_INVALID))
		return TYPE_SPAWN_PERMISSIONS_INVALID;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_FILE_INVALID))
		return TYPE_SPAWN_FILE_INVALID;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SPAWN_NO_MEMORY))
		return TYPE_SPAWN_NO_MEMORY;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_UNIX_PROCESS_ID_UNKNOWN))
		return TYPE_UNIX_PROCESS_ID_UNKNOWN;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_INVALID_SIGNATURE))
		return TYPE_INVALID_SIGNATURE;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_INVALID_FILE_CONTENT))
		return TYPE_INVALID_FILE_CONTENT;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN))
		return TYPE_SELINUX_SECURITY_CONTEXT_UNKNOWN;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_ADT_AUDIT_DATA_UNKNOWN))
		return TYPE_ADT_AUDIT_DATA_UNKNOWN;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_OBJECT_PATH_IN_USE))
		return TYPE_OBJECT_PATH_IN_USE;
	if (dbus_error_has_name(impl_->raw(), DBUS_ERROR_INCONSISTENT_MESSAGE))
		return TYPE_INCONSISTENT_MESSAGE;
	return TYPE_UNKNOWN;
}

glue::error & error::glue() const
{
	return *impl_;
}

namespace glue {

error::error()
{
	dbus_error_init(&raw_);
}

error::~error()
{
	if (dbus_error_is_set(&raw_)) {
		dbus_error_free(&raw_);
	}
}

}}
