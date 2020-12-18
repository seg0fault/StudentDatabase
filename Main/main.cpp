#include <ctime>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "command.h"
#include "database.h"

int readFromClient (int fd, char *buf);

int main(int argc, char *argv[])
{
    FILE *fp_data;
    double worktime = 0;
    char buf[200];
    int port, res, i, err, opt = 1;
    int sock, new_sock;
    fd_set active_set, read_set;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t size;
    database a;

    if(argc < 3){printf("[i]Usage %s file_data port\n", argv[0]); return 1;}

    if(sscanf(argv[2], "%d", &port) != 1){printf("Incorrect port format\n"); return 1;}

    if(!(fp_data = fopen(argv[1], "r"))){printf("[!]Cannot open data file '%s'\n", argv[1]); return 2;}

    a.get_logstream(stdout);
    worktime = clock();
    if(a.database_init(fp_data))
    {
        printf("[!]Terminated due to init error =(\n");
        fclose(fp_data);
        return 1;
    }
    else printf("[+]Data read OK\n[t]Startup time: %lf\n-----------------\n", (clock() - worktime)/CLOCKS_PER_SEC);


    // Создаем TCP сокет для приема запросов на соединение
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
            perror ("Server: cannot create socket");
            exit (EXIT_FAILURE);
    }

    // Заполняем адресную структуру
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //  и связываем сокет с любым адресом
    err = bind (sock, (struct sockaddr *) &addr, sizeof (addr));
    if (err < 0){perror ("Server: cannot bind socket"); exit (EXIT_FAILURE);}

    err = listen (sock, 10); // Создаем очередь на 3 входящих запроса соединения
    if (err < 0) {perror ("Server: listen queue failure"); exit (EXIT_FAILURE);}
    // Подготавливаем множества дескрипторов каналов ввода-вывода.
    // Для простоты не вычисляем максимальное значение дескриптора,
    // а далее будем проверять все дескрипторы вплоть до максимально
    // возможного значения FD_SETSIZE.
    FD_ZERO (&active_set);
    FD_SET (sock, &active_set);

    while (1) // Основной бесконечный цикл проверки состояния сокетов
    {         // Проверим, не появились ли данные в каком-либо сокете.
              // В нашем варианте ждем до фактического появления данных.
        read_set = active_set;
        if (select (FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0)
        {
            perror ("Server: select failure");
            exit (EXIT_FAILURE);
        }
        for (i = 0; i < FD_SETSIZE; i++) // Данные появились. Проверим в каком сокете.
        {
            if (FD_ISSET (i, &read_set))
            {
                if (i == sock) // пришел запрос на новое соединение
                {
                    size = sizeof (client);
                    new_sock = accept (sock, (struct sockaddr *) &client, &size);
                    if (new_sock < 0) {perror ("Accept failure"); exit (EXIT_FAILURE);}
                    fprintf (stdout, "Server: connect from host %s, port %d.\n",
                         inet_ntoa (client.sin_addr),(unsigned int) ntohs (client.sin_port));
                    FD_SET (new_sock, &active_set);
                }
                else // пришли данные в уже существующем соединени
                {
                    err = readFromClient (i, buf);
                    if (err < 0) {close(i); FD_CLR(i, &active_set);} // ошибка или конец данных
                    else // данные прочитаны нормально
                    {
                        command com;
                        res = com.parce_command(buf);
                        if(res)
                        {
                            send_signal(i, SIG_SYNTAX);
                            continue;
                        }
                        res = a.get_command(com, i);
                        if(res == 10){fclose(fp_data); close(sock); return 0;}
                        else if(!res) send_signal(i, SIG_DONE);
                    }
                }
            }
        }
    }

    fclose(fp_data);
    close(sock);
    return 0;
}

int readFromClient (int fd, char *buf)
{
        int nbytes, len, i;
        // Получаем длину сообщения
        if (read (fd, &len, sizeof (int)) != (int) sizeof (int))
        {
                // ошибка чтения
                perror ("read length");
                return -1;
        }
        // Здесь неплохо бы проверить размер буфера
        // Получаем len байт
        for (i = 0; len > 0; i += nbytes, len -= nbytes)
        {
                nbytes = read (fd, buf + i, len);
                if (nbytes < 0)
                {
                        perror ("read");
                        return -1;
                }
                else if (nbytes == 0)
                {
                        perror ("read truncated");
                        return -1;
                }
        }
        // Длина находится в i
        if (i == 0)
        {
                // нет данных
                fprintf (stderr, "Server: no message\n");
                return -1;
        }
        else
        {
                // есть данные
//                fprintf (stdout, "Server got %d bytes of message: %s\n", i, buf);
                return 0;
        }
}

