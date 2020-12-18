#ifndef RECORD_H
#define RECORD_H

#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>

#define LEN 1234

int clprintf(int fd, const char *format, ...);
int send_signal(int fd, int sig);

class record
{
private:
    char *name = nullptr;
    int   phone = 0;
    int   group = 0;
    friend class btree_node;
    friend class btree;
    friend class database;
    friend class avl_tree;
    friend class avl_node;
    friend class gtree;
    friend class gtree_node;
    friend struct node;
public:
    record() = default;
    ~record(){destroy();}
    void destroy();

    int record_init_base(const char *new_name, int new_phone, int new_group);
    int record_init_string(const char *new_name, const char *new_phone, const char *new_group);
    int record_init_file(FILE *fp);

    void print(FILE *stream);
    void print_client(int fd);
    void print_enh(FILE *stream);
    void print_enh_client(int fd);

    record& operator=(const record& x);
    record& operator=(record&& x);
    int operator<(const record& x) const;
    int operator>(const record& x) const;
    int operator==(const record& x) const;
};

#endif


