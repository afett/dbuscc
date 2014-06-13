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

#ifndef DBUSCC_SIGNAL_SLOTS_H
#define DBUSCC_SIGNAL_SLOTS_H

#include <dbuscc/config.h>

#if defined(DBUSCC_USE_BOOST_FUNCTION)
#include <boost/function.hpp>
#define DBUSCC_FUNCTION(signature) boost::function<signature>
#else
#include <tr1/functional>
#define DBUSCC_FUNCTION(signature) std::tr1::function<signature>
#endif

#if defined(DBUSCC_USE_BOOST_SIGNALS)
#include <boost/signals2/signal.hpp>
#define DBUSCC_SIGNAL(signature) boost::signals2::signal<signature>
#elif defined(DBUSCC_USE_TSCB_SIGNALS)
#include <tscb/signal>
#define DBUSCC_SIGNAL(signature) tscb::signal<signature>
#else
#include <yash.hpp>
#define DBUSCC_SIGNAL(signature) yash::signal<signature>
#endif

#endif
