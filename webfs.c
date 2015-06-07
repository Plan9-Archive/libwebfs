#include <u.h>
#include <libc.h>
#include <webfs.h>

char *mntwebfs = "/mnt/web";

Webfs*
webfs(char *url, ...)
{
	int ctlfd, n;
	char buf[1024+1];
	va_list arg;
	Webfs *w;

	snprint(buf, sizeof(buf), "%s/clone", mntwebfs);
	if((ctlfd = open(buf, ORDWR)) < 0)
		return nil;

	if((n = read(ctlfd, buf, sizeof(buf-1))) <= 0){
		close(ctlfd);
		return nil;
	}
	buf[n] = 0;

	w = mallocz(sizeof(*w), 1);
	if(w == nil){
		close(ctlfd);
		return nil;
	}

	w->conn = atoi(buf);
	w->ctlfd = ctlfd;
	w->bodyfd = -1;

	va_start(arg, url);
	vsnprint(w->url, sizeof(w->url), url, arg);
	va_end(arg);

	return w;
}

int
webfshdr(Webfs *w, char *key, char *value)
{
	int n;
	char hdr[256];

	n = snprint(hdr, sizeof(hdr), "headers %s: %s", key, value);

	if(write(w->ctlfd, hdr, n) != n)
		return -1;
	return 0;
}

static int
doreq(Webfs *w)
{
	int fd, n;
	char buf[1024];

	n = snprint(buf, sizeof(buf), "url %s", w->url);
	if(write(w->ctlfd, buf, n) != n)
		return -1;

	if(w->useragent[0]){
		n = snprint(buf, sizeof(buf), "useragent %s", w->useragent);
		if(write(w->ctlfd, buf, n) != n)
			return -1;
	}

	if(w->method[0]){
		n = snprint(buf, sizeof(buf), "request %s", w->method);
		if(write(w->ctlfd, buf, n) != n)
			return -1;
	}

	if(w->postbody != nil && w->npostbody > 0){
		snprint(buf, sizeof(buf), "%s/%d/postbody", mntwebfs, w->conn);
		fd = open(buf, ORDWR);
		if(fd < 0)
			return -1;
		n = write(fd, w->postbody, w->npostbody);
		close(fd);
		if(n != w->npostbody){
			return -1;
		}
	}

	snprint(buf, sizeof(buf), "%s/%d/body", mntwebfs, w->conn);
	fd = open(buf, OREAD);
	if(fd < 0)
		return -1;

	w->bodyfd = fd;

	return 0;
}

char *webfsget(Webfs *w)
{
	int n;
	char *buf;

	if(doreq(w) < 0)
		return nil;

	buf = mallocz(8192+1, 1);
	n = read(w->bodyfd, buf, 8192);
	if(n < 0){
		free(buf);
		return nil;
	}

	buf[n] = 0;
	return buf;
}

char *webfspost(Webfs *w, char *body)
{
	w->postbody = strdup(body);
	w->npostbody = strlen(body);

	return webfsget(w);
}

void
webfsfree(Webfs *w)
{
	if(w->ctlfd >= 0)
		close(w->ctlfd);
	if(w->bodyfd >= 0)
		close(w->bodyfd);
	if(w->postbody != nil)
		free(w->postbody);
	free(w);
}
