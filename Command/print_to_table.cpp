#include "command.h"

void command::print_parced()
{
    switch(cmd)
    {
    case CMD_NONE: printf("COMMAND_EMPTY!\n"); break;
    case CMD_QUIT: printf("QUIT\n"); break;
    case CMD_STOP: printf("STOP\n"); break;
    case CMD_INSERT: print_insert_table(); break;
    case CMD_SELECT: print_select_table(1); break;
    case CMD_DELETE: print_select_table(0); break;
    }
}

void command::print_insert_table()
{
    printf("| TYPE | STATEMENT1 | STATEMENT2 | STATEMENT3 |\n");
    printf("|INSERT|%12s|%12s|%12s|", statement1, statement2, statement3);
}

void command::print_select_table(int type)
{
    printf("| TYPE |FIELD1|CND1| STATEMENT1 |\n");
    if(type) printf("|SELECT|");
    else     printf("|DELETE|");
    switch(first)
    {
    case 1:  printf(" NAME |"); print_cond_to_table(c_name); break;
    case 2:  printf(" PHONE|"); print_cond_to_table(c_phone); break;
    case 3:  printf(" GROUP|"); print_cond_to_table(c_group); break;
    default: printf("   N/A|");
    }
    printf("%12s|\n", statement1);
    if(!second) return;
    printf("|||||||||||||||||||||||||||||||||\n");
    printf("| OPER |FIELD2|CND2| STATEMENT2 |\n");
    switch(op)
    {
    case OP_NONE: printf("|  N/A |"); break;
    case OP_AND:  printf("|  AND |"); break;
    case OP_OR:   printf("|  OR  |"); break;
    }
    switch(second)
    {
    case 1:  printf(" NAME |"); print_cond_to_table(c_name); break;
    case 2:  printf(" PHONE|"); print_cond_to_table(c_phone); break;
    case 3:  printf(" GROUP|"); print_cond_to_table(c_group); break;
    default: printf("   N/A|");
    }
    printf("%12s|\n\n", statement2);
}

void command::print_cond_to_table(cond_type type)
{
    switch(type)
    {
    case COND_NONE: printf(" N/A|"); break;
    case COND_EQ:   printf(" == |"); break;
    case COND_NE:   printf(" != |"); break;
    case COND_LE:   printf(" <= |"); break;
    case COND_GE:   printf(" >= |"); break;
    case COND_LT:   printf(" <  |"); break;
    case COND_GT:   printf(" >  |"); break;
    case COND_LIKE:   printf("LIKE|"); break;
    }
}
