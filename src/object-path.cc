#include <dbus/dbus.h>
#include <dbuscc/object-path.h>

namespace dbuscc {

object_path::object_path()
:
	path_()
{ }

object_path::object_path(object_path const& o)
:
	path_(o.path_)
{ }

object_path & object_path::operator=(object_path const& o)
{
	if (&o != this) {
		path_ = o.path_;
	}

	return *this;
}

object_path::object_path(std::string const& path)
:
	path_(path)
{ }

std::string object_path::str() const
{
	return path_;
}

const char *object_path::c_str() const
{
	return path_.c_str();
}

bool object_path::is_valid() const
{
	return dbus_validate_path(path_.c_str(), NULL);
}

bool operator==(object_path const& l, object_path const& r)
{
	return l.str() == r.str();
}

bool operator!=(object_path const& l, object_path const& r)
{
	return l.str() != r.str();
}

bool operator<(object_path const& l, object_path const& r)
{
	return l.str() < r.str();
}

}
