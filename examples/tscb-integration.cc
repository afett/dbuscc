#include <dbuscc/bus.h>
#include <dbuscc/error.h>
#include <dbuscc/connection.h>
#include <dbuscc/watch.h>
#include <dbuscc/timeout.h>
#include <dbuscc/object-path.h>
#include <dbuscc/interface.h>
#include <dbuscc/member.h>
#include <dbuscc/bus-name.h>
#include <dbuscc/signal-message.h>
#include <dbuscc/call-message.h>
#include <dbuscc/pending-call.h>

#include <tscb/dispatch>
#include <tscb/ioready>

#include <boost/bind.hpp>

#include <stdio.h>
#define TRACE() \
	fprintf(stderr, "%s:%d\n", __PRETTY_FUNCTION__, __LINE__)

#define TRACE_MSG(msg) \
	fprintf(stderr, msg " in %s:%d\n", __PRETTY_FUNCTION__, __LINE__)

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

		do_dispatch();
	}

	struct io_ctx {
		dbuscc::watch_ptr watch;
		tscb::ioready_connection conn;
	};

	struct timer_ctx {
		dbuscc::timeout_ptr timeout;
		tscb::timer_connection conn;
	};

	void on_watch_add(dbuscc::watch_ptr const& watch)
	{
		TRACE();
		boost::shared_ptr<io_ctx> ctx(new io_ctx);
		ctx->watch = watch;
		watch->on_change().connect(boost::bind(
			&tscb_adapter::on_watch_change, this, ctx));
		watch->on_remove().connect(boost::bind(
			&tscb_adapter::on_watch_remove, this, ctx));
		reactor_.post(boost::bind(
			&tscb_adapter::on_watch_change, this, ctx));
	}

	void on_timeout_add(dbuscc::timeout_ptr const& timeout)
	{
		TRACE();
		boost::shared_ptr<timer_ctx> ctx(new timer_ctx);
		ctx->timeout = timeout;
		timeout->on_change().connect(boost::bind(
			&tscb_adapter::on_timeout_change, this, ctx));
		timeout->on_remove().connect(boost::bind(
			&tscb_adapter::on_timeout_remove, this, ctx));
		reactor_.post(boost::bind(
			&tscb_adapter::on_timeout_change, this, ctx));
	}

	void on_watch_change(boost::shared_ptr<io_ctx> const& ctx)
	{
		TRACE();
		tscb::ioready_events events(tscb::ioready_none);
		if (ctx->watch->enabled()) {
			if (ctx->watch->flags() & dbuscc::watch::FLAG_READ) {
				events |= tscb::ioready_input;
				TRACE_MSG("read");
			}
			if (ctx->watch->flags() & dbuscc::watch::FLAG_WRITE) {
				events |= tscb::ioready_output;
				TRACE_MSG("write");
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

	void on_watch_remove(boost::shared_ptr<io_ctx> const& ctx)
	{
		TRACE();
		ctx->conn.disconnect();
		ctx->watch.reset();
	}

	void on_io_ready(dbuscc::watch_ptr const& watch, tscb::ioready_events events)
	{
		TRACE();
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

	void on_timeout_change(boost::shared_ptr<timer_ctx> const& ctx)
	{
		TRACE();
		ctx->conn.disconnect();
		if (ctx->timeout->enabled()) {
			boost::posix_time::ptime when(tscb::monotonic_time() +
				boost::posix_time::milliseconds(ctx->timeout->ms_interval()));

			ctx->conn = reactor_.timer(boost::bind(
				&tscb_adapter::on_timer, this, _1, ctx->timeout), when);
		}
	}

	void on_timeout_remove(boost::shared_ptr<timer_ctx> const& ctx)
	{
		TRACE();
		ctx->conn.disconnect();
		ctx->timeout.reset();
	}

	bool on_timer(boost::posix_time::ptime & now, dbuscc::timeout_ptr const& timeout)
	{
		TRACE();
		timeout->handle();
		now += boost::posix_time::milliseconds(timeout->ms_interval());
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

void on_hello_reply(dbuscc::pending_call_ptr const& reply)
{
	std::cerr << __PRETTY_FUNCTION__ << "\n";
	if (reply->reply_error()) {
		std::cerr << "got error:" << reply->reply_error().name() << "\n";
	} else {
		std::cerr << "Success\n";
	}
}

int main()
{
	tscb::posix_reactor reactor;
	dbuscc::bus_ptr dbus(dbuscc::bus::create());
	dbuscc::error err;
	dbuscc::connection_ptr conn(
		dbus->open_private(dbuscc::session_bus_address(), err));
	dbuscc::tscb_adapter adapter(reactor, conn);

	dbuscc::call_message_ptr hello(
		dbuscc::call_message::create(
			dbuscc::bus_name("org.freedesktop.DBus"),
			dbuscc::object_path("/org/freedesktop/DBus"),
			dbuscc::interface("org.freedesktop.DBus"),
			dbuscc::member("Hello")));
	dbuscc::pending_call_ptr hello_reply(conn->call(hello));
	hello_reply->on_completion().connect(boost::bind(
		on_hello_reply, hello_reply));

	/*

	dbuscc::message_ptr msg(
		dbuscc::signal_message::create(
			dbuscc::object_path("/foo"),
			dbuscc::interface("foo.bar"),
			dbuscc::member("my_signal")));

	if (!conn->send(msg)) {
		return 1;
	}

	*/

	for (;;) {
		reactor.dispatch();
	}

	return 0;
}
