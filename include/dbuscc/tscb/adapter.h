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
#ifndef DBUSCC_TSCB_ADAPTER_H
#define DBUSCC_TSCB_ADAPTER_H

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>

#include <tscb/ioready>
#include <tscb/reactor>

#include <dbuscc/pointer.h>

namespace dbuscc {

class tscb_adapter {
public:
	tscb_adapter(
		tscb::posix_reactor_service &,
		dbuscc::connection_ptr const&);

	~tscb_adapter();
private:
	struct io_ctx {
		~io_ctx();

		dbuscc::watch_ptr watch;
		tscb::ioready_connection conn;
		tscb::connection change_conn;
		tscb::connection remove_conn;
	};

	void on_watch_add(dbuscc::watch_ptr const&);
	void on_watch_change(boost::shared_ptr<io_ctx> const&);
	void on_watch_remove(boost::shared_ptr<io_ctx> const&);
	void on_io_ready(dbuscc::watch_ptr const&, tscb::ioready_events);

	struct timer_ctx {
		~timer_ctx();

		dbuscc::timeout_ptr timeout;
		tscb::timer_connection conn;
		tscb::connection change_conn;
		tscb::connection remove_conn;
	};

	void on_timeout_add(dbuscc::timeout_ptr const&);
	void on_timeout_change(boost::shared_ptr<timer_ctx> const&);
	void on_timeout_remove(boost::shared_ptr<timer_ctx> const&);
	bool on_timer(boost::posix_time::ptime &, dbuscc::timeout_ptr const&);

	void on_dispatch_state(dbuscc::connection::DispatchState);
	void do_dispatch();

	tscb::posix_reactor_service & reactor_;
	dbuscc::connection_ptr conn_;
	tscb::connection watch_conn_;
	tscb::connection timeout_conn_;
	tscb::connection dispatch_conn_;
};

}
#endif
