#ifndef DBUSCC_MOCKS_H
#define DBUSCC_MOCKS_H

#include <dbuscc/connection.h>
#include <dbuscc/call-message.h>
#include <dbuscc/return-message.h>
#include <dbuscc/signal-message.h>
#include <dbuscc/message-writer.h>
#include <dbuscc/pending-call.h>
#include <dbuscc/timeout.h>
#include <dbuscc/watch.h>
#include <dbuscc/error.h>

namespace dbuscc {
namespace mock {

template <typename Interface, typename Glue>
class base : public Interface {
public:
	Glue & glue()
	{
		return *(static_cast<Glue*>(0));
	}
};

class connection : public base<dbuscc::connection, glue::connection> {
public:
	virtual void close()
	{ }

	virtual bool send(message_ptr const&)
	{ return true; }

	virtual pending_call_ptr call(call_message_ptr const&)
	{ return pending_call_ptr(); }

	virtual DispatchState dispatch_state() const
	{ return dbuscc::connection::DISPATCH_COMPLETE; }

	virtual DispatchState dispatch()
	{ return dbuscc::connection::DISPATCH_COMPLETE; }

	virtual void install_handlers()
	{ }

	DBUSCC_SIGNAL(void(watch_ptr)) & on_watch_add()
	{ return on_watch_add_; }

	DBUSCC_SIGNAL(void(timeout_ptr)) & on_timeout_add()
	{ return on_timeout_add_; }

	DBUSCC_SIGNAL(void(DispatchState)) & on_dispatch_state()
	{ return on_dispatch_state_; }

	DBUSCC_SIGNAL(void(watch_ptr)) on_watch_add_;
	DBUSCC_SIGNAL(void(timeout_ptr)) on_timeout_add_;
	DBUSCC_SIGNAL(void(DispatchState)) on_dispatch_state_;
};

class message_writer : public base<dbuscc::message_writer, glue::message_writer> {
public:
	message_writer()
	{ }

	virtual bool push_uint8(uint8_t)
	{ return true; }

	virtual bool push_bool(bool)
	{ return true; }

	virtual bool push_int16(int16_t)
	{ return true; }

	virtual bool push_uint16(uint16_t)
	{ return true; }

	virtual bool push_int32(int32_t)
	{ return true; }

	virtual bool push_uint32(uint32_t)
	{ return true; }

	virtual bool push_int64(int64_t)
	{ return true; }

	virtual bool push_uint64(uint64_t)
	{ return true; }

	virtual bool push_double(double)
	{ return true; }

	virtual bool push_string(std::string const&)
	{ return true; }

	virtual bool push_object_path(object_path const&)
	{ return true; }
};

class call_message : public base<dbuscc::call_message, glue::message> {
public:
	virtual dbuscc::message_writer create_writer()
	{ return mock::message_writer(); }
};

class return_message : public base<dbuscc::return_message, glue::message> {
public:
	virtual dbuscc::message_writer create_writer()
	{ return mock::message_writer(); }
};

class signal_message : public base<dbuscc::signal_message, glue::message> {
public:
	virtual dbuscc::message_writer create_writer()
	{ return mock::message_writer(); }
};

class pending_call : public base<dbuscc::pending_call, glue::pending_call> {
public:
	virtual DBUSCC_SIGNAL(void(void)) & on_completion()
	{ return on_completion_; }

	virtual void cancel()
	{ }

	virtual State state() const
	{
		return STATE_PENDING;
	}

	virtual return_message_ptr reply() const
	{
		return return_message_ptr();
	}

	virtual error & reply_error()
	{
		return error_;
	}

	DBUSCC_SIGNAL(void(void)) on_completion_;
	error error_;
};

class timeout : public base<dbuscc::timeout, glue::timeout> {
public:
	virtual DBUSCC_SIGNAL(void(void)) & on_change()
	{ return on_change_; }

	virtual DBUSCC_SIGNAL(void(void)) & on_remove()
	{ return on_remove_; }

	virtual bool enabled() const
	{ return true; }

	virtual bool handle()
	{ return true; }

	virtual int ms_interval() const
	{ return -1; }

	DBUSCC_SIGNAL(void(void)) on_change_;
	DBUSCC_SIGNAL(void(void)) on_remove_;
};

class watch : public base<dbuscc::watch, glue::watch> {
public:
	virtual DBUSCC_SIGNAL(void(void)) & on_change()
	{ return on_change_; }

	virtual DBUSCC_SIGNAL(void(void)) & on_remove()
	{ return on_remove_; }

	virtual int fd() const
	{ return -1; }

	virtual Flags flags() const
	{ return FLAG_NONE; }

	virtual bool enabled() const
	{ return true; }

	virtual bool handle(Flags)
	{ return true; }

	DBUSCC_SIGNAL(void(void)) on_change_;
	DBUSCC_SIGNAL(void(void)) on_remove_;
};

}}

#endif
