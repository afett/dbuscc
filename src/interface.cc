#include <dbus/dbus.h>
#include <dbuscc/interface.h>

namespace dbuscc {

interface::interface()
:
	name_()
{ }

interface::interface(interface const& o)
:
	name_(o.name_)
{ }

interface & interface::operator=(interface const& o)
{
	if (&o != this) {
		name_ = o.name_;
	}

	return *this;
}

interface::interface(std::string const& path)
:
	name_(path)
{ }

std::string interface::str() const
{
	return name_;
}

const char *interface::c_str() const
{
	return name_.c_str();
}

bool interface::is_valid() const
{
	return dbus_validate_interface(name_.c_str(), NULL);
}

bool operator==(interface const& l, interface const& r)
{
	return l.str() == r.str();
}

bool operator!=(interface const& l, interface const& r)
{
	return l.str() != r.str();
}

bool operator<(interface const& l, interface const& r)
{
	return l.str() < r.str();
}

}
