#pragma lib	"libwebfs.a"
#pragma src	"/sys/src/libwebfs"

typedef struct Webfs Webfs;
struct Webfs
{
	int		conn;
	int		ctlfd;
	int		bodyfd;
	char	url[1024+1];
	char	useragent[512];
	char	method[32];
	char	*postbody;
	int		npostbody;
};

extern char *mntwebfs;

Webfs *webfs(char *url, ...);
int webfshdr(Webfs *w, char *key, char *value);
char *webfsget(Webfs *w);
char *webfspost(Webfs *w, char *body);
void webfsfree(Webfs *w);
