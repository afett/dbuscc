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

#ifndef DBUSCC_CONNECTION_H
#define DBUSCC_CONNECTION_H

#include <dbuscc/pointer.h>
#include <dbuscc/signal-slots.h>

namespace dbuscc {

class connection {
public:
	enum DispatchState {
		DISPATCH_DATA_REMAINS,
		DISPATCH_COMPLETE,
		DISPATCH_NEED_MEMORY,
	};

	virtual ~connection() {}
	virtual void close() = 0;
	virtual bool send(message_ptr const&) = 0;
	virtual pending_call_ptr call(call_message_ptr const&) = 0;
	virtual DispatchState dispatch_state() const = 0;
	virtual DispatchState dispatch() = 0;

	virtual void install_handlers() = 0;
	virtual DBUSCC_SIGNAL(void(watch_weak_ptr)) & on_watch_add() = 0;
	virtual DBUSCC_SIGNAL(void(timeout_weak_ptr)) & on_timeout_add() = 0;
	virtual DBUSCC_SIGNAL(void(DispatchState)) & on_dispatch_state() = 0;

	virtual glue::connection & glue() = 0;
};

}

#endif
