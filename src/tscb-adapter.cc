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

#include <boost/bind.hpp>

#include <dbuscc/connection.h>
#include <dbuscc/timeout.h>
#include <dbuscc/watch.h>

#include <dbuscc/tscb/adapter.h>

#include "trace.h"

namespace dbuscc {

tscb_adapter::tscb_adapter(
	tscb::posix_reactor_service & reactor,
	dbuscc::connection_ptr const& conn)
:
	reactor_(reactor),
	conn_(conn),
	watch_conn_(),
	timeout_conn_(),
	dispatch_conn_()
{
	/* see documentation in the dbuscc/connection header */
	watch_conn_ = conn_->on_watch_add().connect(
		boost::bind(&tscb_adapter::on_watch_add, this, _1));
	timeout_conn_ = conn_->on_timeout_add().connect(
		boost::bind(&tscb_adapter::on_timeout_add, this, _1));
	dispatch_conn_ = conn_->on_dispatch_state().connect(
		boost::bind(&tscb_adapter::on_dispatch_state, this, _1));
	conn_->install_handlers();

	do_dispatch();
}

tscb_adapter::~tscb_adapter()
{
	watch_conn_.disconnect();
	timeout_conn_.disconnect();
	dispatch_conn_.disconnect();
}

tscb_adapter::io_ctx::~io_ctx()
{
	change_conn.disconnect();
	remove_conn.disconnect();
}

void tscb_adapter::on_watch_add(dbuscc::watch_ptr const& watch)
{
	DBUSCC_TRACE();
	boost::shared_ptr<io_ctx> ctx(new io_ctx);
	ctx->watch = watch;
	ctx->change_conn = watch->on_change().connect(boost::bind(
		&tscb_adapter::on_watch_change, this, ctx));
	ctx->remove_conn = watch->on_remove().connect(boost::bind(
		&tscb_adapter::on_watch_remove, this, ctx));
	reactor_.post(boost::bind(
		&tscb_adapter::on_watch_change, this, ctx));
}

void tscb_adapter::on_watch_change(boost::shared_ptr<io_ctx> const& ctx)
{
	DBUSCC_TRACE();
	tscb::ioready_events events(tscb::ioready_none);
	if (ctx->watch->enabled()) {
		if (ctx->watch->flags() & dbuscc::watch::FLAG_READ) {
			events |= tscb::ioready_input;
			DBUSCC_TRACE();
		}
		if (ctx->watch->flags() & dbuscc::watch::FLAG_WRITE) {
			events |= tscb::ioready_output;
			DBUSCC_TRACE();
		}
	}

	if (ctx->conn.connected()) {
		ctx->conn.modify(events);
	} else {
		ctx->conn = reactor_.watch(
			boost::bind(&tscb_adapter::on_io_ready, this, ctx->watch, _1),
			ctx->watch->fd(), events);
	}
}

void tscb_adapter::on_watch_remove(boost::shared_ptr<io_ctx> const& ctx)
{
	DBUSCC_TRACE();
	ctx->conn.disconnect();
	ctx->watch.reset();
}

void tscb_adapter::on_io_ready(dbuscc::watch_ptr const& watch, tscb::ioready_events events)
{
	DBUSCC_TRACE();
	dbuscc::watch::Flags flags(dbuscc::watch::FLAG_NONE);
	if (events & tscb::ioready_input) {
		flags |= dbuscc::watch::FLAG_READ;
	}
	if (events & tscb::ioready_output) {
		flags |= dbuscc::watch::FLAG_WRITE;
	}
	if (events & tscb::ioready_error) {
		flags |= dbuscc::watch::FLAG_ERROR;
	}
	if (events & tscb::ioready_hangup) {
		flags |= dbuscc::watch::FLAG_HANGUP;
	}
	watch->handle(flags);
}

tscb_adapter::timer_ctx::~timer_ctx()
{
	change_conn.disconnect();
	remove_conn.disconnect();
}

void tscb_adapter::on_timeout_add(dbuscc::timeout_ptr const& timeout)
{
	DBUSCC_TRACE();
	boost::shared_ptr<timer_ctx> ctx(new timer_ctx);
	ctx->timeout = timeout;
	ctx->change_conn = timeout->on_change().connect(boost::bind(
		&tscb_adapter::on_timeout_change, this, ctx));
	ctx->remove_conn = timeout->on_remove().connect(boost::bind(
		&tscb_adapter::on_timeout_remove, this, ctx));
	reactor_.post(boost::bind(
		&tscb_adapter::on_timeout_change, this, ctx));
}

void tscb_adapter::on_timeout_change(boost::shared_ptr<timer_ctx> const& ctx)
{
	DBUSCC_TRACE();
	ctx->conn.disconnect();
	if (ctx->timeout->enabled()) {
		boost::posix_time::ptime when(tscb::monotonic_time() +
			boost::posix_time::milliseconds(ctx->timeout->ms_interval()));

		ctx->conn = reactor_.timer(boost::bind(
			&tscb_adapter::on_timer, this, _1, ctx->timeout), when);
	}
}

void tscb_adapter::on_timeout_remove(boost::shared_ptr<timer_ctx> const& ctx)
{
	DBUSCC_TRACE();
	ctx->conn.disconnect();
	ctx->timeout.reset();
}

bool tscb_adapter::on_timer(boost::posix_time::ptime & now, dbuscc::timeout_ptr const& timeout)
{
	DBUSCC_TRACE();
	timeout->handle();
	now += boost::posix_time::milliseconds(timeout->ms_interval());
	return true;
}

void tscb_adapter::on_dispatch_state(dbuscc::connection::DispatchState state)
{
	DBUSCC_TRACE();
	if (state != dbuscc::connection::DISPATCH_COMPLETE) {
		reactor_.post(boost::bind(&tscb_adapter::do_dispatch, this));
	}
}

void tscb_adapter::do_dispatch()
{
	DBUSCC_TRACE();
	on_dispatch_state(conn_->dispatch());
}

}
