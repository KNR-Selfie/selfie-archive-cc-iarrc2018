#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include  <arpa/inet.h>
#include <sys/select.h>

#define MAXLINE 1024
#define SERV_PORT 6000

//!> 注意输入是由stdin，接受是由server发送过来
//!> 所以在client端也是需要select进行处理的
void send_and_recv( int connfd )
{
    FILE * fp = stdin;
    int   lens;
    char send[MAXLINE];
    char recv[MAXLINE];
    fd_set rset;
    FD_ZERO( &rset );
    int maxfd = ( fileno( fp ) > connfd ? fileno( fp ) : connfd  + 1 );   
                                                //!> 输入和输出的最大值
    int n;
   
    while( 1 )
    {
        FD_SET( fileno( fp ), &rset );
        FD_SET( connfd, &rset );            //!> 注意不要把rset看作是简单的一个变量
                                                //!> 注意它其实是可以包含一组套接字的哦，
                                                //!> 相当于是封装的数组！每次都要是新的哦！
       
        if( select( maxfd, &rset, NULL, NULL, NULL ) == -1 )
        {
            printf("Client Select Error..\n");
            exit(EXIT_FAILURE  );
        }
       
        //!> if 连接口有信息
        if( FD_ISSET( connfd, &rset ) )    //!> if 连接端口有信息
        {
            printf( "client get from server ...\n" );
            memset( recv, 0, sizeof( recv ) );
            n = read( connfd, recv, MAXLINE );
            if( n == 0 )
            {
                printf("Recv ok...\n");
                break;
            }
            else if( n == -1 )
            {
                printf("Recv error...\n");
                break;
            }
            else
            {
                lens = strlen( recv );
                recv[lens] = '\0';
                //!> 写到stdout
                write( STDOUT_FILENO, recv, MAXLINE );
                printf("\n");
            }

        }
       
        //!> if 有stdin输入
        if( FD_ISSET( fileno( fp ), &rset ) )    //!> if 有输入
        {
            //!> printf("client stdin ...\n");
           
            memset( send, 0, sizeof( send ) );
            if( fgets( send, MAXLINE, fp ) == NULL )
            {
                printf("End...\n");
                exit( EXIT_FAILURE );
            }
            else
            {
                //!>if( str )
                lens = strlen( send );
                send[lens-1] = '\0';        //!> 减一的原因是不要回车字符
                                            //!> 经验值：这一步非常重要的哦！！！！！！！！
                if( strcmp( send, "q" ) == 0 )
                {
                    printf( "Bye..\n" );
                    return;
                }
               
                printf("Client send : %s\n", send);
                write( connfd, send, strlen( send ) );
            }
        }
       
    }
   
}

int main( int argc, char ** argv )
{
    //!> char * SERV_IP = "10.30.97.188";
    char     buf[MAXLINE];
    int       connfd;
    struct sockaddr_in servaddr;
   
    if( argc != 2 )
    {
        printf("Input server ip !\n");
        exit( EXIT_FAILURE );
    }
   
    //!> 建立套接字
    if( ( connfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
        printf("Socket Error %d.\n",errno);
        exit( EXIT_FAILURE );
    }

    //!> 套接字信息
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
   
    //!> 链接server
    if( connect( connfd, ( struct sockaddr *  )&servaddr, sizeof( servaddr ) ) < 0 )
    {
        printf("Connect error..\n");
        exit(EXIT_FAILURE);
    }   
   

    //!>
    //!> send and recv
    send_and_recv( connfd );
   
    //!>

    close( connfd );
    printf("Exit\n");
   
    return 0;
}
