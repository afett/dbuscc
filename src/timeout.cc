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

#include "xassert.h"

namespace dbuscc {
namespace internal {

class timeout :
	public dbuscc::timeout,
	public glue::timeout
{
public:
	timeout(DBusTimeout *);
	static timeout_ptr from_raw(DBusTimeout *);

	DBUSCC_SIGNAL(void(void)) & on_change();
	DBUSCC_SIGNAL(void(void)) & on_remove();
	int ms_interval() const;
	bool enabled() const;
	bool handle();

	void notify_toggled();
	void notify_removed();

	glue::timeout & glue();
	DBusTimeout *raw();

private:
	static void unref(void *);

	DBusTimeout *raw_;
	bool removed_;
	DBUSCC_SIGNAL(void(void)) on_change_;
	DBUSCC_SIGNAL(void(void)) on_remove_;
};

timeout_ptr timeout::from_raw(DBusTimeout *raw)
{
	void *data(dbus_timeout_get_data(raw));
	DBUSCC_ASSERT(data);
	timeout *self(static_cast<timeout*>(data));
	return timeout_ptr(self);
}

void timeout::unref(void *data)
{
	DBUSCC_ASSERT(data);
	timeout *self(static_cast<timeout*>(data));
	intrusive_ptr_release(self);
}

timeout::timeout(DBusTimeout *raw)
:
	raw_(raw),
	removed_(false)
{
	DBUSCC_ASSERT(raw_);
	intrusive_ptr_add_ref(this);
	dbus_timeout_set_data(raw_, this, &timeout::unref);
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
	if (removed_) {
		return -1;
	}

	DBUSCC_ASSERT(raw_);
	return dbus_timeout_get_interval(raw_);
}

bool timeout::handle()
{
	if (removed_) {
		return false;
	}

	DBUSCC_ASSERT(raw_);
	return dbus_timeout_handle(raw_);
}

bool timeout::enabled() const
{
	if (removed_) {
		return false;
	}

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
	removed_ = true;
	raw_ = 0;
	on_remove_();
}

} // internal

namespace glue {

timeout_ptr timeout::create(DBusTimeout *raw)
{
	return new internal::timeout(raw);
}

void timeout::toggled(DBusTimeout *raw, void *)
{
	internal::timeout::from_raw(raw)->glue().notify_toggled();
}

void timeout::removed(DBusTimeout *raw, void *)
{
	internal::timeout::from_raw(raw)->glue().notify_removed();
}

}

}
