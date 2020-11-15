#ifndef COMMAND_H
#define COMMAND_H

#include "enum.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

#define PARCER_BUF_LEN 20

void print_command_usage();
char get_token(const char *source, int& i, const char *expected);
char skip_spaces(const char *source, int& i);
char get_statement(const char *source, char *dest, int& i, const char *expected);

#define STATEMENT_LEN 100
#define KEYWORD_LEN 20

class command
{
private:
    cmd_type cmd = CMD_NONE;

    op_type op = OP_NONE;

    cond_type c_name = COND_NONE;
    cond_type c_phone = COND_NONE;
    cond_type c_group = COND_NONE;

    char keyword_buf[KEYWORD_LEN];
    int first = 0;
    int second = 0;
    char statement1[STATEMENT_LEN]; //default - name
    char statement2[STATEMENT_LEN]; //default - phone
    char statement3[STATEMENT_LEN]; //default - group

    friend class database;
    friend class gtree;
public:
    ~command() = default;
    command(){statement1[0] = '\0'; statement2[0] = '\0'; statement3[0] = '\0'; keyword_buf[0] = '\0';}

    int parce_command(const char *source);
    int get_insert_arguments(const char *source, int& i);
    int get_select_arguments(const char *source, int& i, int type);
    void swap_select()
    {
        int saveint = first; first = second; second = saveint;
        char save[STATEMENT_LEN]; strcpy(save, statement1); strcpy(statement1, statement2); strcpy(statement2, save);
    }

    cmd_type get_cmd(const char *source, int &i);
    cond_type get_cond();
    op_type get_op();

    void print_command();


    void print_parced();
    void print_insert_table();
    void print_select_table(int type);
    void print_cond_to_table(cond_type type);

    void print_file(FILE *fp_out);
    void print_select_file(FILE *fp_out, int type);
    void print_cond_to_file(FILE *fp_out, cond_type type);
};








#endif
