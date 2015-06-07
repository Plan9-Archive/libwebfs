#include <u.h>
#include <libc.h>
#include "webfs.h"

void
get(void)
{
	Webfs *w = webfs("http://httpbin.org/get");
	if(w == nil)
		sysfatal("webfs: %r");

	char *data = webfsget(w);
	if(data == nil)
		sysfatal("webfsget: %r");

	print(data);

	free(data);

	webfsfree(w);
}

void
post(void)
{
	Webfs *w = webfs("http://httpbin.org/post");
	if(w == nil)
		sysfatal("webfs: %r");

	if(webfshdr(w, "Content-Type", "application/octet-stream") < 0)
		sysfatal("webfshdr: %r");

	char *data = webfspost(w, "Hello!");
	if(data == nil)
		sysfatal("webfsget: %r");

	print(data);

	free(data);

	webfsfree(w);
}

void
main(int argc, char *argv[])
{
	ARGBEGIN{
	}ARGEND

	get();
	post();

	exits(nil);
}
