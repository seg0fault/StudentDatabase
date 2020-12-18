#ifndef DATABASE_H
#define DATABASE_H

int strcmp_eq(char* a, char* b);
int strcmp_ne(char* a, char* b);
int strcmp_gt(char* a, char* b);
int strcmp_ge(char* a, char* b);
int strcmp_lt(char* a, char* b);
int strcmp_le(char* a, char* b);

int cmp_eq(int a, int b);
int cmp_ne(int a, int b);
int cmp_gt(int a, int b);
int cmp_ge(int a, int b);
int cmp_lt(int a, int b);
int cmp_le(int a, int b);

#include <ctime>
#include "command.h"
#include "2list.h"
#include "btree.h"
#include "avl.h"
#include "gtree.h"
class database
{
private:
    list data;
    avl_tree name_tree;
    btree phone_tree;
    gtree group_tree;
    list_node *sel_start = nullptr;
    list_node *sel_last = nullptr;

    FILE *ostream;
    FILE *logstream;

    int sel_all = 0;

    //технические переменные для функции добавления в выделение
    char *other_name_key  = nullptr;
    int   other_phone_key = 0;
    int   other_group_key = 0;
    int   other_statement_type = 0;
    op_type oper = OP_NONE;
    int (*cmp_name)(char*, char*) = nullptr;
    int (*cmp_phone)(int, int) = nullptr;
    int (*cmp_group)(int, int) = nullptr;
public:
    database() = default;
    ~database() = default;
    void delete_name_tree(){name_tree.avl_delete_all();}
    void delete_phone_tree(){phone_tree.btree_delete_tree(phone_tree.root);}
    int database_init(FILE *fp);

    int get_command(command& com, int fd);
    int        com_insert(command& com, int fd);
    int        com_select(command& com);
    int        com_delete_selected(int fd);
    int        com_print_selected(int fd);


    void get_sel_parameters(int position, int type, command& com);
    void clear_sel_parameters()
    {
        other_name_key = nullptr; other_phone_key = 0; other_group_key = 0; other_statement_type = 0;
        oper = OP_NONE; cmp_name = nullptr; cmp_phone = nullptr; cmp_group = nullptr;
    }
    void add_to_selection(list_node *sel);

    void print_all(int fd, int type); //type 0 - as list, type 1 - phone tree
    void get_ostream(FILE *stream){ostream = stream;}
    void get_logstream(FILE *stream){logstream = stream;}
};





#endif
