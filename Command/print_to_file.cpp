#include "command.h"

void command::print_file(FILE *fp_out)
{
    switch(cmd)
    {
    case CMD_NONE: break;
    case CMD_QUIT: fprintf(fp_out, "quit;\n"); break;
    case CMD_STOP: fprintf(fp_out, "stop;\n"); break;
    case CMD_INSERT: fprintf(fp_out, "insert (%s, %s, %s);\n", statement1, statement2, statement3); break;
    case CMD_SELECT: print_select_file(fp_out, 1); break;
    case CMD_DELETE: print_select_file(fp_out, 0); break;
    }
}

void command::print_select_file(FILE *fp_out, int type)
{
    if(type) fprintf(fp_out, "select *");
    else fprintf(fp_out, "delete");
    if(!first){fprintf(fp_out, ";\n"); return;}
    fprintf(fp_out, " where");
    switch(first)
    {
    case 1: fprintf(fp_out, " name "); print_cond_to_file(fp_out, c_name); break;
    case 2: fprintf(fp_out, " phone "); print_cond_to_file(fp_out, c_phone); break;
    case 3: fprintf(fp_out, " group "); print_cond_to_file(fp_out, c_group); break;
    default: fprintf(fp_out, " field_error ");
    }
    fprintf(fp_out, "%s", statement1);
    if(!second) {fprintf(fp_out, ";\n"); return;}
    switch (op)
    {
    case OP_NONE: fprintf(fp_out, " op_error "); break;
    case OP_AND: fprintf(fp_out, " and "); break;
    case OP_OR: fprintf(fp_out, " or "); break;
    }
    switch(second)
    {
    case 1: fprintf(fp_out, "name "); print_cond_to_file(fp_out, c_name); break;
    case 2: fprintf(fp_out, "phone "); print_cond_to_file(fp_out, c_phone); break;
    case 3: fprintf(fp_out, "group "); print_cond_to_file(fp_out, c_group); break;
    default: fprintf(fp_out, "field_error ");
    }
    fprintf(fp_out, "%s;\n", statement2);
}

void command::print_cond_to_file(FILE *fp_out, cond_type type)
{
    switch(type)
    {
    case COND_NONE: fprintf(fp_out, " cond_error"); break;
    case COND_EQ:   fprintf(fp_out, "= "); break;
    case COND_NE:   fprintf(fp_out, "<> "); break;
    case COND_LE:   fprintf(fp_out, "<= "); break;
    case COND_GE:   fprintf(fp_out, ">= "); break;
    case COND_LT:   fprintf(fp_out, "< "); break;
    case COND_GT:   fprintf(fp_out, "> "); break;
    case COND_LIKE: fprintf(fp_out, "like "); break;
    }
}
