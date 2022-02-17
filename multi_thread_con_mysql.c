/********
* it used to test MySQL
* gcc -o multi_thread_con_mysql $(mysql_config --cflags) multi_thread_con_mysql.c $(mysql_config --libs)
*********/
#include <stdio.h>
#include "mysql.h"
#include <pthread.h>
#include <netinet/in.h>  
#include <netdb.h>
#include <sys/syscall.h>

char    *ip[20] = { 0 };  
char    *user[20];
int     *pass[20];
char    *db[20];
int     port;  
int     con_num;
unsigned long sleeptime;

void *myThreadCon(int a) {
    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_OPT_COMPRESS,0);
    mysql_options(&mysql,MYSQL_INIT_COMMAND,"SET autocommit=0");
    int tid = syscall(SYS_gettid);
    printf("Thread_id is: %d  ",tid);

    if(!mysql_real_connect(&mysql,&ip,&user,&pass,&db,port,NULL,0)) {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",mysql_error(&mysql));
    } else {
        printf("Success to connect to database: %d\n",a);
    }
}

int main(int argc,char *argv[]) {

    int rc, i, j,detachstate;

    if( argc != 8 ) {  
        fprintf(stderr,"%s <target ip> <target port> <username> <password> <connect_num> <sleep_time> \n", argv[0]);  
        fprintf(stderr, "For example 192.168.1.1 root password dbname port 1000(connection) 600(seconds) \n");  
        return -1;  
    } 
    strcpy( ip, argv[1] );
    if( inet_addr(ip) == INADDR_NONE ) {
        printf( "target ip error.\n" );
        return -1;
    }
    strcpy(user,argv[2]);
    if(user == NULL){
        printf("Name is not empty! \n");
        return -1;
    }
   
    strcpy(pass,argv[3]);

    strcpy(db,argv[4]);

    port = atoi( argv[5] );
    if( port < 0 || port > 65535 ) {
        printf( "Port between 1000 - 65535 !\n" );
        return -1;
    }
    con_num =atoi( argv[6] );
    if( con_num <= 0 || con_num > 10000 ) {
        printf( "The number of connections between 1 and 10000 !\n" );
        return -1;
    }

    sleeptime = atoi(argv[7]);

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i=1;i<=con_num;i++) {
        rc = pthread_create(&tid, &attr, myThreadCon, i);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        /* Wait for the thread */
        rc = pthread_join(tid, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
    sleep(sleeptime);
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}
