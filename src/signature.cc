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

#include <dbus/dbus.h>

#include <dbuscc/glue/error.h>
#include <dbuscc/signature.h>

namespace dbuscc {

signature::signature()
:
	str_(),
	valid_(false)
{ }

signature::signature(std::string const& sig)
:
	str_(sig),
	valid_(dbus_signature_validate(str_.c_str(), 0))
{ }

signature::signature(std::string const& sig, error & err)
:
	str_(sig),
	valid_(dbus_signature_validate(str_.c_str(), err.glue().raw()))
{ }

signature::signature(signature const& o)
:
	str_(o.str_),
	valid_(o.valid_)
{ }

signature & signature::operator=(signature const& o)
{
	if (&o != this) {
		str_ = o.str_;
		valid_ = o.valid_;
	}

	return *this;
}

bool signature::valid() const
{
	return valid_;
}

std::string signature::str() const
{
	return str_;
}

}
