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
#ifndef DBUSCC_WATCH_H
#define DBUSCC_WATCH_H

#include <dbus/dbus.h>
#include <dbuscc/common.h>

namespace dbuscc {

class watch {
public:
	enum Flags {
		FLAG_NONE   = 0,
		FLAG_READ   = (1 << 0),
		FLAG_WRITE  = (1 << 1),
		FLAG_HANGUP = (1 << 2),
		FLAG_ERROR  = (1 << 3),
	};

	virtual ~watch() { }
	virtual DBUSCC_SIGNAL(void(void)) & on_change() = 0;
	virtual DBUSCC_SIGNAL(void(void)) & on_remove() = 0;
	virtual int fd() const = 0;
	virtual Flags flags() const = 0;
	virtual bool enabled() const = 0;
	virtual bool handle(Flags) = 0;

	virtual glue::watch & glue() = 0;
};

static inline watch::Flags operator&(watch::Flags l, watch::Flags r)
{
	return (watch::Flags)((int)l & (int)r);
}

static inline watch::Flags operator|(watch::Flags l, watch::Flags r)
{
	return (watch::Flags)((int)l | (int)r);
}

static inline watch::Flags operator~(watch::Flags l)
{
	return (watch::Flags)(~(int)l);
}

static inline watch::Flags & operator&=(watch::Flags & l, watch::Flags r)
{
	l = l & r; return l;
}

static inline watch::Flags & operator|=(watch::Flags & l, watch::Flags r)
{
	l = l | r; return l;
}

}

#endif
