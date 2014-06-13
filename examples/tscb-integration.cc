#include <dbuscc/bus.h>
#include <dbuscc/error.h>
#include <dbuscc/connection.h>
#include <dbuscc/watch.h>
#include <dbuscc/timeout.h>
#include <tscb/dispatch>
#include <tscb/ioready>

#include <boost/bind.hpp>

#include <stdio.h>
#define TRACE() \
	fprintf(stderr, "%s:%d\n", __PRETTY_FUNCTION__, __LINE__)

namespace dbuscc {

class tscb_adapter {
public:
	tscb_adapter(
		tscb::posix_reactor_service & reactor,
		dbuscc::connection_ptr const& conn)
	:
		reactor_(reactor),
		conn_(conn)
	{
		/* see documentation in the dbuscc/connection header */
		conn_->on_watch_add().connect(
			boost::bind(&tscb_adapter::on_watch_add, this, _1));
		conn_->on_timeout_add().connect(
			boost::bind(&tscb_adapter::on_timeout_add, this, _1));
		conn_->on_dispatch_state().connect(
			boost::bind(&tscb_adapter::on_dispatch_state, this, _1));
		conn_->install_handlers();
	}

	struct io_ctx {
		dbuscc::watch_weak_ptr watch;
		tscb::ioready_connection conn;
	};

	struct timer_ctx {
		dbuscc::timeout_weak_ptr timeout;
		tscb::timer_connection conn;
	};

	void on_watch_add(dbuscc::watch_weak_ptr const& watch)
	{
		TRACE();
		boost::shared_ptr<io_ctx> ctx(new io_ctx);
		ctx->watch = watch;
		dbuscc::watch_ptr w(ctx->watch.lock());
		w->on_change().connect(boost::bind(
			&tscb_adapter::on_watch_change, this, ctx));
		reactor_.post(boost::bind(
			&tscb_adapter::on_watch_change, this, ctx));
	}

	void on_timeout_add(dbuscc::timeout_weak_ptr const& timeout)
	{
		TRACE();
		boost::shared_ptr<timer_ctx> ctx(new timer_ctx);
		ctx->timeout = timeout;
		dbuscc::timeout_ptr t(ctx->timeout.lock());
		t->on_change().connect(boost::bind(
			&tscb_adapter::on_timeout_change, this, ctx));
		reactor_.post(boost::bind(
			&tscb_adapter::on_timeout_change, this, ctx));
	}

	void on_watch_change(boost::shared_ptr<io_ctx> const& ctx)
	{
		TRACE();
		dbuscc::watch_ptr w(ctx->watch.lock());
		if (!w) {
			return;
		}

		tscb::ioready_events events(tscb::ioready_none);
		if (w->enabled()) {
			if (w->flags() & dbuscc::watch::FLAG_READ) {
				events |= tscb::ioready_input;
			}
			if (w->flags() & dbuscc::watch::FLAG_WRITE) {
				events |= tscb::ioready_output;
			}
		}

		if (ctx->conn.connected()) {
			ctx->conn.modify(events);
		} else {
			ctx->conn = reactor_.watch(
				boost::bind(&tscb_adapter::on_io_ready, this, ctx->watch, _1),
				w->fd(), events);
		}
	}

	void on_io_ready(dbuscc::watch_weak_ptr const& ww, tscb::ioready_events events)
	{
		TRACE();
		dbuscc::watch_ptr w(ww.lock());
		if (!w) {
			return;
		}

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
		w->handle(flags);
	}

	void on_timeout_change(boost::shared_ptr<timer_ctx> const& ctx)
	{
		TRACE();
		dbuscc::timeout_ptr timeout(ctx->timeout.lock());
		if (!timeout) {
			return;
		}

		ctx->conn.disconnect();
		if (timeout->enabled()) {
			boost::posix_time::ptime when(tscb::monotonic_time() +
				boost::posix_time::milliseconds(timeout->ms_interval()));

			reactor_.timer(boost::bind(
				&tscb_adapter::on_timer, this, _1, ctx->timeout), when);
		}
	}

	bool on_timer(boost::posix_time::ptime & now, dbuscc::timeout_weak_ptr const& wt)
	{
		TRACE();
		dbuscc::timeout_ptr t(wt.lock());
		if (!t) {
			return false;
		}

		t->handle();
		now += boost::posix_time::milliseconds(t->ms_interval());
		return true;
	}

	void on_dispatch_state(dbuscc::connection::DispatchState state)
	{
		TRACE();
		if (state != dbuscc::connection::DISPATCH_COMPLETE) {
			reactor_.post(boost::bind(&tscb_adapter::do_dispatch, this));
		}
	}

	void do_dispatch()
	{
		TRACE();
		on_dispatch_state(conn_->dispatch());
	}

	tscb::posix_reactor_service & reactor_;
	dbuscc::connection_ptr conn_;
};

}

int main()
{
	tscb::posix_reactor reactor;
	dbuscc::bus_ptr dbus(dbuscc::bus::create());
	dbuscc::error err;
	dbuscc::connection_ptr conn(
		dbus->open(dbuscc::bus::TYPE_SESSION, err));
	dbuscc::tscb_adapter adapter(reactor, conn);

	for (;;) {
		reactor.dispatch();
	}

	return 0;
}
