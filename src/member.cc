#include <dbus/dbus.h>
#include <dbuscc/member.h>

namespace dbuscc {

member::member()
:
	name_()
{ }

member::member(member const& o)
:
	name_(o.name_)
{ }

member & member::operator=(member const& o)
{
	if (&o != this) {
		name_ = o.name_;
	}

	return *this;
}

member::member(std::string const& path)
:
	name_(path)
{ }

std::string member::str() const
{
	return name_;
}

const char *member::c_str() const
{
	return name_.c_str();
}

bool member::is_valid() const
{
	return dbus_validate_member(name_.c_str(), NULL);
}

bool operator==(member const& l, member const& r)
{
	return l.str() == r.str();
}

bool operator!=(member const& l, member const& r)
{
	return l.str() != r.str();
}

bool operator<(member const& l, member const& r)
{
	return l.str() < r.str();
}

}
