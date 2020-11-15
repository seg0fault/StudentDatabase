#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "../Command/command.h"
#include "../Database/database.h"

int main(int argc, char *argv[]) //file_in file_out
{
    FILE *fp_data, *fp_com, *fp_out, *fp_log;
    double worktime = 0;

    char buf[200];
    int res;

    int log = 0;

    database a;

    if(argc < 2){printf("[i]Usage %s file_data [file_commands] [file_output]\n", argv[0]); return 1;}
    fp_data = fopen(argv[1], "r");
    fp_com = fopen(argv[2], "r");
    if(argc >= 3) fp_com = fopen(argv[2], "r");
    else fp_com = stdin;
    if(argc >= 4) fp_out = fopen(argv[3], "w");
    else fp_out = stdout;
    fp_log = fopen("log.out", "w");

    if(!fp_data){printf("[!]Cannot open data file '%s'\n", argv[1]); return 2;}
    if(!fp_com){printf("[!]Cannot open command file '%s'\n", argv[2]); fclose(fp_data); return 2;}
    if(!fp_out){printf("[!]Cannot open output file '%s'\n", argv[3]); fclose(fp_data); fclose(fp_com); return 2;}
    else
    {
        if(fp_out != stdout) printf("[i]Output file %s set!\n", argv[3]);
        else printf("[i]Writing output to console\n");
    }
    if(!fp_log){printf("[!]Cannot create log file\n"); fclose(fp_data); fclose(fp_com); fclose(fp_out); return 2;}
    a.get_ostream(fp_out);
    a.get_logstream(fp_log);
    worktime = clock();
    if(a.database_init(fp_data))
    {
        printf("[!]Terminated due to init error =(\n");
        fclose(fp_data);
        fclose(fp_com);
        fclose(fp_out);
        fclose(fp_log);
        return 1;
    }
    else printf("[+]Data read OK\n[t]Startup time: %lf\n-----------------\n", (clock() - worktime)/CLOCKS_PER_SEC);
    printf("[i]Proccessing commands from file '%s'\n", argv[2]);
    worktime = clock();
    while(fgets(buf, 200, fp_com))
    {
        command com;
        res = com.parce_command(buf);
        if(res){fprintf(fp_log, "[s]Command: %sSyntax error: %d\n", buf, res); log++; continue;}
        res = a.get_command(com);
        if(res && res != 10){fprintf(fp_log, "[!]Command: %sCode: %d\n", buf, res); log++;}
        if(res == 10) break;
    }
    printf("[i]Command file '%s' proccesed\n[t]Work time: %lf\n-----------------\n", argv[2], (clock() - worktime)/CLOCKS_PER_SEC);
    if(log) printf("[!]Command exceptions: %d\n[!]View file log.out to see work log\n", log);
    fclose(fp_data);
    fclose(fp_com);
    fclose(fp_out);
    fclose(fp_log);
    return 0;
}





