#include "command.h"

char skip_spaces(const char *source, int& i)
{
    for(; source[i] == ' '; i++);
    return source[i];
}

char get_token(const char *source, int& i, const char *expected)
{
    if(!skip_spaces(source, i)) return 0;
    if(!strchr(expected, source[i])) return 0;
    i++;
    return source[i];
}

char get_statement(const char *source, char *dest, int& i, const char *expected)
{
    if(!skip_spaces(source, i)) return 0;
    int word_start = i, word_len = 0;
    for (; source[i]; i++)
    {
        if(strchr(expected, source[i]))
        {
            strncpy(dest, source + word_start, word_len);
            dest[word_len] = '\0';
            i++;
            return source[i-1];
        }
        else if(source[i] == ' ')
        {
            if(!get_token(source, i, expected)) return 0;
            strncpy(dest, source + word_start, word_len);
            dest[word_len] = '\0';
            return source[i-1];
        }
        word_len++;
    }
    return 0;
}

int command::get_insert_arguments(const char *source, int &i)
{
    if(!get_token(source, i, "(")) return 1;
    if(!get_statement(source, statement1, i, ",")) return 1;
    if(!get_statement(source, statement2, i, ",")) return 1;
    if(!get_statement(source, statement3, i, ")")) return 1;
    if(!get_token(source, i, ";")) return 1;
    return 0;
}


int command::get_select_arguments(const char *source, int &i, int type)
{
    cond_type cond;    char res;

    if(type){if(!get_token(source, i, "*")) return 1;}
    if(get_token(source, i, ";")) return 0;
    if(!get_statement(source, keyword_buf, i, " ")) return 1;
    if(strcmp(keyword_buf, "where") != 0) return 1;

    if(!get_statement(source, keyword_buf, i, " ")) return 1;
    if(!strcmp(keyword_buf, "name")) first = 1;
    else if(!strcmp(keyword_buf, "phone")) first = 2;
    else if(!strcmp(keyword_buf, "group")) first = 3;
    else return 0;

    if(!get_statement(source, keyword_buf, i, " ")) return 1;
    if(!(cond = get_cond())) return 1;
    switch(first)
    {
    case 1: c_name = cond; break;
    case 2: c_phone = cond; break;
    case 3: c_group = cond; break;
    default: break;
    }

    if(!(res = get_statement(source, statement1, i, " ;"))) return 1;
    if(res == ';') return 0;
    if(get_token(source, i, ";")) return 0;

    i--;
    if(!get_statement(source, keyword_buf, i, " ")) return 1;
    op = get_op();

    if(!get_statement(source, keyword_buf, i, " ")) return 1;
    if(!strcmp(keyword_buf, "name")) second = 1;
    else if(!strcmp(keyword_buf, "phone")) second = 2;
    else if(!strcmp(keyword_buf, "group")) second = 3;
    else return 1;

    if(!get_statement(source, keyword_buf, i, " ")) return 1;
    if(!(cond = get_cond())) return 1;
    switch(second)
    {
    case 1: c_name = cond; break;
    case 2: c_phone = cond; break;
    case 3: c_group = cond; break;
    default: break;
    }

    if(!(res = get_statement(source, statement2, i, " ;"))) return 1;
    if(res == ';') return 0;
    if(get_token(source, i, ";")) return 0;

    return 1;
}


cmd_type command::get_cmd(const char *source, int& i)
{
    if(        (source[i]   == 'q')
            && (source[i+1] == 'u')
            && (source[i+2] == 'i')
            && (source[i+3] == 't')){i += 4; if(!get_token(source, i, ";")) return CMD_NONE; return CMD_QUIT;}

    else if(   (source[i]   == 'i')
            && (source[i+1] == 'n')
            && (source[i+2] == 's')
            && (source[i+3] == 'e')
            && (source[i+4] == 'r')
            && (source[i+5] == 't')){i += 6; return CMD_INSERT;}

    else if(   (source[i]   == 's')
            && (source[i+1] == 'e')
            && (source[i+2] == 'l')
            && (source[i+3] == 'e')
            && (source[i+4] == 'c')
            && (source[i+5] == 't')){i += 6; return CMD_SELECT;}

    else if(   (source[i]   == 'd')
            && (source[i+1] == 'e')
            && (source[i+2] == 'l')
            && (source[i+3] == 'e')
            && (source[i+4] == 't')
            && (source[i+5] == 'e')){i += 6; return CMD_DELETE;}

    else if(   (source[i]   == 's')
            && (source[i+1] == 't')
            && (source[i+2] == 'o')
            && (source[i+3] == 'p')){i += 4; if(!get_token(source, i, ";")) return CMD_NONE; return CMD_STOP;}

    else return CMD_NONE;
}

cond_type command::get_cond()
{
    if(!strcmp(keyword_buf, "=")) return COND_EQ;
    if(!strcmp(keyword_buf, "<>")) return COND_NE;
    if(!strcmp(keyword_buf, "<=")) return COND_LE;
    if(!strcmp(keyword_buf, ">=")) return COND_GE;
    if(!strcmp(keyword_buf, "<")) return COND_LT;
    if(!strcmp(keyword_buf, ">")) return COND_GT;
    if(!strcmp(keyword_buf, "like")) return COND_LIKE;
    return COND_NONE;
}

op_type command::get_op()
{
    if(!strcmp(keyword_buf, "and")) return OP_AND;
    if(!strcmp(keyword_buf, "or")) return OP_OR;
    return OP_NONE;
}
