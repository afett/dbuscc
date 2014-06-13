#ifndef DBUSCC_TRACE_H
#define DBUSCC_TRACE_H

#include <stdlib.h>
#include <stdio.h>

#define DBUSCC_TRACE()                                                     \
	if (getenv("DBUSCC_TRACE")) {                                      \
		fprintf(stderr, "%s:%d\n", __PRETTY_FUNCTION__, __LINE__); \
	}

#endif
