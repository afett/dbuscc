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

#include <dbuscc/glue/watch.h>

#include "ptr-wrapper.h"
#include "xassert.h"

namespace dbuscc {
namespace internal {

class watch :
	public glue::watch,
	public DBUSCC_SHARED_FROM_THIS(watch) {
public:
	watch(DBusWatch *);

	DBUSCC_SIGNAL(void(void)) & on_change();
	DBUSCC_SIGNAL(void(void)) & on_remove();
	int fd() const;
	Flags flags() const;
	bool enabled() const;
	bool handle(Flags);

	void notify_toggled();
	void notify_removed();

	glue::watch & glue();
	DBusWatch *raw();

	void wrap();

	struct wrapper : public ptr_wrapper<glue::watch_ptr> {
		wrapper(glue::watch_ptr const& ptr)
		: ptr_wrapper<glue::watch_ptr>(ptr)
		{ }

		static glue::watch_ptr & unwrap(DBusWatch *raw)
		{
			void *data(dbus_watch_get_data(raw));
			return self(data)->ptr_;
		}
	};

private:

	DBusWatch *raw_;
	DBUSCC_SIGNAL(void(void)) on_change_;
	DBUSCC_SIGNAL(void(void)) on_remove_;
};

watch::watch(DBusWatch *raw)
:
	raw_(raw)
{
	DBUSCC_ASSERT(raw_);
}

void watch::wrap()
{
	DBUSCC_ASSERT(raw_);
	wrapper *data(new wrapper(shared_from_this()));
	dbus_watch_set_data(raw_, data, &wrapper::delete_wrapper);
}

glue::watch & watch::glue()
{
	return *this;
}

DBusWatch *watch::raw()
{
	return raw_;
}

int watch::fd() const
{
	DBUSCC_ASSERT(raw_);
	return dbus_watch_get_unix_fd(raw_);
}

watch::Flags watch::flags() const
{
	DBUSCC_ASSERT(raw_);
	Flags ret(FLAG_NONE);

	if (!enabled()) {
		return ret;
	}

	unsigned int dbus_flags(dbus_watch_get_flags(raw_));
	if (dbus_flags & DBUS_WATCH_READABLE) {
		ret |= FLAG_READ;
	}

	if (dbus_flags & DBUS_WATCH_WRITABLE) {
		ret |= FLAG_WRITE;
	}

	return ret;
}

bool watch::handle(Flags flags)
{
	DBUSCC_ASSERT(raw_);

	unsigned int dbus_flags(0);
	if (flags & FLAG_READ) {
		dbus_flags |= DBUS_WATCH_READABLE;
	}

	if (flags & FLAG_WRITE) {
		dbus_flags |= DBUS_WATCH_WRITABLE;
	}

	if (flags & FLAG_HANGUP) {
		dbus_flags |= DBUS_WATCH_HANGUP;
	}

	if (flags & FLAG_ERROR) {
		dbus_flags |= DBUS_WATCH_ERROR;
	}

	return dbus_watch_handle(raw_, dbus_flags);
}

bool watch::enabled() const
{
	DBUSCC_ASSERT(raw_);
	return dbus_watch_get_enabled(raw_);
}

DBUSCC_SIGNAL(void(void)) & watch::on_change()
{
	return on_change_;
}

DBUSCC_SIGNAL(void(void)) & watch::on_remove()
{
	return on_change_;
}

void watch::notify_toggled()
{
	on_change_();
}

void watch::notify_removed()
{
	on_remove_();
}

} // internal

namespace glue {

watch_weak_ptr watch::create(DBusWatch *raw)
{
	internal::watch *w(new internal::watch(raw));
	watch_ptr res(w);
	w->wrap();
	return res;
}

void watch::toggled(DBusWatch *raw, void *)
{
	DBUSCC_ASSERT(raw);
	internal::watch::wrapper::unwrap(raw)->notify_toggled();
}

void watch::removed(DBusWatch *raw, void *)
{
	DBUSCC_ASSERT(raw);
	internal::watch::wrapper::unwrap(raw)->notify_removed();
}

}

}
