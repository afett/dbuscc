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

#ifndef DBUSCC_POINTER_H
#define DBUSCC_POINTER_H

#include <dbuscc/config.h>
#include <dbuscc/forward.h>

#if defined(DBUSCC_USE_BOOST_PTR)
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#define DBUSCC_SHARED_PTR(type) boost::shared_ptr<type>
#define DBUSCC_WEAK_PTR(type) boost::weak_ptr<type>
#define DBUSCC_SCOPED_PTR(type) boost::scoped_ptr<type>
#define DBUSCC_SHARED_FROM_THIS(type) boost::enable_shared_from_this<type>
#else
#include <tr1/memory>
#define DBUSCC_SHARED_PTR(type) std::tr1::shared_ptr<type>
#define DBUSCC_WEAK_PTR(type) std::tr1::weak_ptr<type>
#define DBUSCC_SCOPED_PTR(type) std::auto_ptr<type>
#define DBUSCC_SHARED_FROM_THIS(type) std::tr1::enable_shared_from_this<type>
#endif

namespace dbuscc {

typedef DBUSCC_SHARED_PTR(bus) bus_ptr;
typedef DBUSCC_SHARED_PTR(connection) connection_ptr;
typedef DBUSCC_SHARED_PTR(watch) watch_ptr;
typedef DBUSCC_WEAK_PTR(watch) watch_weak_ptr;
typedef DBUSCC_SHARED_PTR(timeout) timeout_ptr;
typedef DBUSCC_WEAK_PTR(timeout) timeout_weak_ptr;
typedef DBUSCC_SHARED_PTR(message) message_ptr;
typedef DBUSCC_SHARED_PTR(error_message) error_message_ptr;
typedef DBUSCC_SHARED_PTR(call_message) call_message_ptr;
typedef DBUSCC_SHARED_PTR(return_message) return_message_ptr;
typedef DBUSCC_SHARED_PTR(signal_message) signal_message_ptr;
typedef DBUSCC_SHARED_PTR(pending_call) pending_call_ptr;

namespace glue {

typedef DBUSCC_SHARED_PTR(watch) watch_ptr;
typedef DBUSCC_WEAK_PTR(watch) watch_weak_ptr;
typedef DBUSCC_SHARED_PTR(timeout) timeout_ptr;
typedef DBUSCC_WEAK_PTR(timeout) timeout_weak_ptr;

}

}

#endif
