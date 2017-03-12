#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h> 
#include<arpa/inet.h> //inet_addr
#include<unistd.h>
#include<ctype.h>
#include<errno.h>
#include<time.h>

//Copy from tinyhttpd-0.1.0
#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: UmboCV Assignment\r\n"

int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';
    
    return(i);
}

//Copy from tinyhttpd-0.1.0
void headers(int client, const char *filename, int size)
{
    char buf[1024];
    
    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    //printf("%s\n",buf);
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    //printf("%s\n",buf);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: image/jpeg\r\n");
    //printf("%s\n",buf);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: %d\r\n", size);
    //printf("%s\n",buf);
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    //printf("%s\n",buf);
    send(client, buf, strlen(buf), 0);
}

void sendPic(int client, FILE *resource, int fileLength)
{
    char *sendbuf;
    sendbuf = (char*) malloc (sizeof(char)*fileLength);
    size_t result = fread(sendbuf, 1, fileLength, resource);
    if (result > 0)
    {
      send(client, sendbuf, result, 0);      
    }   
    free(sendbuf);
}

//Copy from tinyhttpd-0.1.0
void serve_file(int client, const char *filename)
{
    FILE *resource = NULL;
    int size=0;
    //printf("Got path=%s\n",filename);
   
    resource = fopen(filename, "r");
    if (resource == NULL)
        printf("No such file(%s)", filename);
    else
    {
        //Get file size
        fseek(resource, 0, SEEK_END);
        size = ftell(resource);
        fseek(resource, 0, SEEK_SET);
        printf("Total Picture size: %i\n",size);

        //Write HTTP header
        headers(client, filename,size);

        //Send Picture
        sendPic(client, resource, size);
    }
    fclose(resource);
}

int replaceTestJPG()
{
    int pic_idx=0;
    char cmd[32];
    srand(time(NULL));
    pic_idx = rand()%5+1;
    printf("rand=%d\n",pic_idx);
    sprintf(cmd, "cp ./Image/%d.jpg test.jpg",pic_idx);
    system(cmd);
}

//Copy from tinyhttpd-0.1.0
void accept_request(int client)
{
    char buf[1024];
    char method[255];
    char url[255];
    char path[512];
    size_t i, j;
    char *query_string = NULL;
    
    get_line(client, buf, sizeof(buf));
    i = 0; j = 0;
    //printf("buf=%s\n",buf);

    // GET or PUT?
    //Skip space and end of char
    while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
    {
        method[i] = buf[j];
        i++; j++;
    }
    method[i] = '\0';
    //printf("method=%s\n",method);
    
    // Get url
    i = 0;
    while (ISspace(buf[j]) && (j < sizeof(buf)))
        j++;
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
    {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';
    
    //printf("Got url=%s\n",url);

    // Remove /
    if(url[0] == '/'){
       for(int idx=0; idx < sizeof(url)-1; idx++){
           path[idx] = url[idx+1];
       }
    }

    // skip GET favicon.ico
    if(strcmp(path, "test.jpg")){
       return;
    }

    replaceTestJPG();
    serve_file(client, path);
    close(client);
}

 
int main(int argc , char *argv[])
{
    // Create socket
    int srv_socket;
    srv_socket = socket(AF_INET , SOCK_STREAM , 0);
     
    if (srv_socket == -1)
    {
        printf("Could not create socket");
        close(srv_socket);
        return 1;
    }
     
    // Set bind info
    struct sockaddr_in socket_info;
    memset(&socket_info, 0, sizeof(socket_info));
    socket_info.sin_family = AF_INET; //IPV4
    socket_info.sin_addr.s_addr = INADDR_ANY; //IP
    socket_info.sin_port = htons(8000); //port
    if(bind(srv_socket, (struct sockaddr *) &socket_info, sizeof(socket_info)) == -1){
        printf("Could not bind(%s)\n", strerror(errno));
        close(srv_socket);
        return 1;
    }

    // Listen
    if(listen(srv_socket, 32) < 0)
    {
        printf("Could not listen");
        close(srv_socket);
        return 1;
    }

    //Accept
    struct sockaddr_in client_info;
    int addr_len = sizeof(client_info);

    int client_sock = -1;
    while(1){
        client_sock = accept(srv_socket, (struct sockaddr *) &client_info, (socklen_t *)&addr_len);
        if(client_sock == -1)
        {
            printf("Could not accept");
            close(srv_socket);
            return 1;
        }
        accept_request(client_sock);
    }

    //close
    close(client_sock);
    close(srv_socket);
    
    

    return 0;
}
