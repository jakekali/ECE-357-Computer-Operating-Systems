#ifndef MYSTDIOLIB_H
#define MYSTDIOLIB_H

struct MYSTREAM {
    int fd;
    int pos;
    int bufsiz;
    int mode;
    int readm;
    char *buf;
};

struct MYSTREAM *myfopen(const char *pathname, int mode, int buffsiz);
struct MYSTREAM *myfdopen(int fd, int mode, int buffsiz);
int myfgetc(struct MYSTREAM *stream);
int myfputc(int c, struct MYSTREAM *stream);
int myfclose(struct MYSTREAM *stream);

#endif
