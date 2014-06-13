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

#include <dbuscc/glue/timeout.h>

#include "ptr-wrapper.h"
#include "xassert.h"

namespace dbuscc {
namespace internal {

class timeout :
	public glue::timeout,
	public DBUSCC_SHARED_FROM_THIS(timeout) {
public:
	timeout(DBusTimeout *);

	DBUSCC_SIGNAL(void(void)) & on_change();
	DBUSCC_SIGNAL(void(void)) & on_remove();
	int ms_interval() const;
	bool enabled() const;
	bool handle();

	void notify_toggled();
	void notify_removed();

	glue::timeout & glue();
	DBusTimeout *raw();

	void wrap();

	struct wrapper : public ptr_wrapper<glue::timeout_ptr> {
		wrapper(glue::timeout_ptr const& ptr)
		: ptr_wrapper<glue::timeout_ptr>(ptr)
		{ }

		static glue::timeout_ptr & unwrap(DBusTimeout *raw)
		{
			void *data(dbus_timeout_get_data(raw));
			return self(data)->ptr_;
		}
	};

private:

	DBusTimeout *raw_;
	DBUSCC_SIGNAL(void(void)) on_change_;
	DBUSCC_SIGNAL(void(void)) on_remove_;
};

timeout::timeout(DBusTimeout *raw)
:
	raw_(raw)
{
	DBUSCC_ASSERT(raw_);
	wrapper *data(new wrapper(shared_from_this()));
	dbus_timeout_set_data(raw_, data, &wrapper::delete_wrapper);
}

void timeout::wrap()
{
	DBUSCC_ASSERT(raw_);
	wrapper *data(new wrapper(shared_from_this()));
	dbus_timeout_set_data(raw_, data, &wrapper::delete_wrapper);
}

glue::timeout & timeout::glue()
{
	return *this;
}

DBusTimeout *timeout::raw()
{
	return raw_;
}

int timeout::ms_interval() const
{
	DBUSCC_ASSERT(raw_);
	return dbus_timeout_get_interval(raw_);
}

bool timeout::handle()
{
	DBUSCC_ASSERT(raw_);
	return dbus_timeout_handle(raw_);
}

bool timeout::enabled() const
{
	DBUSCC_ASSERT(raw_);
	return dbus_timeout_get_enabled(raw_);
}

DBUSCC_SIGNAL(void(void)) & timeout::on_change()
{
	return on_change_;
}

DBUSCC_SIGNAL(void(void)) & timeout::on_remove()
{
	return on_change_;
}

void timeout::notify_toggled()
{
	on_change_();
}

void timeout::notify_removed()
{
	on_remove_();
}

} // internal

namespace glue {

timeout_weak_ptr timeout::create(DBusTimeout *raw)
{
	internal::timeout *t(new internal::timeout(raw));
	timeout_ptr res(t);
	t->wrap();
	return res;
}

void timeout::toggled(DBusTimeout *raw, void *)
{
	DBUSCC_ASSERT(raw);
	internal::timeout::wrapper::unwrap(raw)->notify_toggled();
}

void timeout::removed(DBusTimeout *raw, void *)
{
	DBUSCC_ASSERT(raw);
	internal::timeout::wrapper::unwrap(raw)->notify_removed();
}

}

}
