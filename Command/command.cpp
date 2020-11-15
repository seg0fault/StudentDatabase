#include "command.h"


int command::parce_command(const char *source)
{
    for(int i = 0; source[i]; i++)
    {
        if(source[i] != ' ')
        {
            if(!cmd && !(cmd = get_cmd(source, i))) return ERR_CMD;
            else
            {
                switch(cmd)
                {
                case CMD_NONE: return ERR_CMD;
                case CMD_QUIT: return 0;
                case CMD_STOP: return 0;
                case CMD_INSERT: return get_insert_arguments(source, i);
                case CMD_SELECT: return get_select_arguments(source, i, 1);
                case CMD_DELETE: return get_select_arguments(source, i, 0);
                }
            }
        }
    }
    return 0;
}

void print_command_usage()
{
    FILE *fp;
    char buf[200];
    fp = fopen("usage.txt", "r");
    if(!fp){printf("Error opening usage file!\n"); return;}
    while(fgets(buf, 200, fp)) printf("%s", buf);
    fclose(fp);
}















