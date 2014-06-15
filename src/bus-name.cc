#include <dbus/dbus.h>
#include <dbuscc/bus-name.h>

namespace dbuscc {

bus_name::bus_name()
:
	name_()
{ }

bus_name::bus_name(bus_name const& o)
:
	name_(o.name_)
{ }

bus_name & bus_name::operator=(bus_name const& o)
{
	if (&o != this) {
		name_ = o.name_;
	}

	return *this;
}

bus_name::bus_name(std::string const& path)
:
	name_(path)
{ }

std::string bus_name::str() const
{
	return name_;
}

const char *bus_name::c_str() const
{
	return name_.c_str();
}

bool bus_name::is_valid() const
{
	return dbus_validate_bus_name(name_.c_str(), NULL);
}

bool operator==(bus_name const& l, bus_name const& r)
{
	return l.str() == r.str();
}

bool operator!=(bus_name const& l, bus_name const& r)
{
	return l.str() != r.str();
}

bool operator<(bus_name const& l, bus_name const& r)
{
	return l.str() < r.str();
}

}
