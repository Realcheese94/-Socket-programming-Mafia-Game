#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAXLINE 511
#define MAX_SOCK 1024
char *EXIT_STRING = "exit";
char *START_STRING = "방에 입장하였습니다. 방장이 시작하기 전까지 대기중입니다..... \n";
char *start_command = "start";
int nnbyte;
char bbuf[MAXLINE+1];
char buf[MAXLINE+1];
char cuf[MAXLINE+1];
int maxfdp1;
int job1,job2;
int num_chat = 0;
int k;
char sbuf[MAXLINE+1];
char *job1message="\n당신은 도둑입니다 보석이있는 경찰의 위치를 파악하여 보석을 훔쳐가세요 매번 밤에 기회가 부여됩니다.\n\n";
char *job2message="\n당신은 경찰입니다. 매번 밤에 기회가 부여되고 도둑을 올바르게 지목하여 시민들의 추리를 도와주세요.\n\n";
char *otherjobmessage = "\n당신은 시민입니다. 아침에 투표를 하여 한명의 직업을 알수 있습니다. 함께 추리하여 도둑을 검거하세요\n\n";
char *morningmessage ="\n낮이 밝았습니다. 밤이되기전까지 추리를하여 도둑을 검거하세요\n\n";
char *nightmessage ="밤이 되었습니다.\n\n 도둑은 보석을 가지고있을 것 같은 경찰을 지목하세요\n\n";
char *nmafiamessage ="경찰을 한명지목하여 보석이있는지 확인하세요\n\n";
char *npolicemessage="도둑을 한명 지목하여 도둑인지 확인하세요\n\n";
char *vote="\n투표를 진행합니다... 도둑으로 의심되는 사람을 순서대로  한명씩 지목하세요\n\n";
char *voteresult="가장 많은 투표를 받은 사람은";
char *victory="그는 도둑입니다.\n\n";
char userid[MAXLINE+1];
char username[5];
char *nvictory="그는 도둑이 아닙니다.\n\n";
int morningvote[5];
char fbuf[MAXLINE+1];
clock_t g_startTime,g_endTime;
int clisock_list[MAX_SOCK];
int listen_sock;
int n=0;
char ccuf[MAXLINE+1];
void addClient(int s,struct sockaddr_in *newcliaddr);
int getmax();
int sbyte;
char *mafiavictory="도둑이 경찰의 보석을 가지고 달아났습니다.\n\n*************************** 도둑의 승리입니다*******************\n\n";
char *policevictory="경찰이도둑을 잡았습니다\n\n********************************* 시민 팀 승리*************************\n\n";
char howmanyvote[MAXLINE+1];
char vvote[MAXLINE+1];
char *correct_command="그는 도둑이 맞습니다.\n\n********************시민 승리 *********************\n\n";
char *incorrect_command="그는 도둑이 아닙니다.  밤이 되었습니다.\n\n";
char *pcorrect_command="그는 보석을 가지고 있었습니다. 게임을 종료 합니다.\n\n***************도둑의 승리********************\n\n";
char *pincorrect_command="그는 보석을 가지고 있지 않습니다.\n\n";
char *police_command="경찰은 도둑으로 의심되는 사람을 지목하세요\n\n";
void removeClient(int s);
int tcp_listen(int host,int port,int backlog);
void errquit(char *mesg)
{
perror(mesg);
exit(1);
}
int main(int argc,char *argv[]){
struct sockaddr_in cliaddr;
int i,j,nbyte,accp_sock,addrlen = sizeof(struct sockaddr_in);
int mvote[5];
int max=0;
int turn =0;
fd_set read_fds;
if(argc != 2){
printf("사용법 : %s port\n",argv[0]);
exit(0);
}
listen_sock =tcp_listen(INADDR_ANY,atoi(argv[1]),5);
maxfdp1 = getmax()+1;
g_startTime = clock();
while(1){
FD_ZERO(&read_fds);
FD_SET(listen_sock,&read_fds);
for(i=0;i<num_chat;i++)
FD_SET(clisock_list[i],&read_fds);
puts("wait forclient");
if(select(maxfdp1,&read_fds,NULL,NULL,NULL)<0)
errquit("select fail");
if(FD_ISSET(listen_sock,&read_fds)){
accp_sock=accept(listen_sock,(struct sockaddr*)&cliaddr,&addrlen);
if(accp_sock==-1)
errquit("accept fail");
addClient(accp_sock,&cliaddr);
send(accp_sock,START_STRING,strlen(START_STRING),0);
printf("%d번째 사용자 추가 \n",num_chat);
}

if(num_chat==5){
printf("게임을 시작하려면 start 키를 누르세요\n");
while(fgets(bbuf,sizeof(bbuf),stdin)!=NULL){
nnbyte=strlen(bbuf);
if(strstr(bbuf,start_command)!=NULL){
printf("게임을 시작합니다\n");
srand(time(NULL));
job1 = rand()%5;
job2=(job1+1)%5;
printf("사용자들의 낮이 시작되었습니다.\n");
for(k=0;k<num_chat;k++)
{
send(clisock_list[k],morningmessage,strlen(morningmessage),0);
if(k==job1){
send(clisock_list[k],job1message,strlen(job1message),0);
}
else if(k==job2){
send(clisock_list[k],job2message,strlen(job2message),0);
}
else
{
send(clisock_list[k],otherjobmessage,strlen(otherjobmessage),0);
}
}
//
//while 있던 자리
//메인있던 자리
while(n<5){
FD_ZERO(&read_fds);
FD_SET(listen_sock,&read_fds);
for(i=0;i<num_chat;i++)
FD_SET(clisock_list[i],&read_fds);
maxfdp1 = getmax()+1;
if(select(maxfdp1,&read_fds,NULL,NULL,NULL)<0)
errquit("select fail");
if(FD_ISSET(listen_sock,&read_fds)){
accp_sock=accept(listen_sock,(struct sockaddr*)&cliaddr,&addrlen);
if(accp_sock==-1)
errquit("accept fail");
} 
for(i=0;i<num_chat;i++){
if(FD_ISSET(clisock_list[i],&read_fds)){
nbyte = recv(clisock_list[i],cuf,MAXLINE,0);
switch(i){
case 0:
cuf[nbyte-1]='-';
cuf[nbyte]='0';
break;
case 1:
cuf[nbyte-1]='-';
cuf[nbyte]='1';
break;
case 2:
cuf[nbyte-1]='-';
cuf[nbyte]='2';
break;
case 3:
cuf[nbyte-1]='-';
cuf[nbyte]='3';
break;
case 4:
cuf[nbyte-1]='-';
cuf[nbyte]='4';
break;
}
for(j=0;j<num_chat;j++)
if(i!=j)
send(clisock_list[j],cuf,nbyte+1,0);
}
}
printf("%s\n",cuf);
n=n+1;
}
}    
printf("투표 진행을 시작합니다.");
for(k=0;k<num_chat;k++){
send(clisock_list[k],vote,strlen(vote),0);
mvote[k]=0;
}
for(i=0;i<num_chat;i++){
if(FD_ISSET(clisock_list[i],&read_fds));
nbyte = recv(clisock_list[i],vvote,MAXLINE,0);

if(!strncmp(vvote,"0",1))
++mvote[0];
if(!strncmp(vvote,"1",1))
++mvote[1];
if(!strncmp(vvote,"2",1))
++mvote[2];
if(!strncmp(vvote,"3",1))
++mvote[3];
if(!strncmp(vvote,"4",1)) 
++mvote[4];
if(mvote[i]>max)
max=i;
}
//for(i=0;i<num_chat;i++)
//{
//if(mvote[i]>max)
//max=i;
//}
//for(i=0;i<num_chat;i++){
//if(mvote[i]>=max)
//max=i;

//}
printf("진짜 도둑은 %d",job1);
printf("가장 많은 투표를 받은 사람은  : %d",max);
sprintf(howmanyvote,"%s %d 입니다",voteresult,max);
for(i=0;i<num_chat;i++){
send(clisock_list[i],howmanyvote,strlen(howmanyvote),0);
} //가장많은득표
if(max==job1){
for(i=0;i<num_chat;i++)
send(clisock_list[i],correct_command,strlen(correct_command),0);
return;//종료
}
else{          //아닌경우 
for(i=0;i<num_chat;i++)
send(clisock_list[i],incorrect_command,strlen(incorrect_command),0);//아니라는 말과 밤으로 넘어간다.
}
for(i=0;i<num_chat;i++){
if(i==job1){
send(clisock_list[i],nmafiamessage,strlen(nmafiamessage),0);//도둑에게만

if(FD_ISSET(clisock_list[job1],&read_fds));
nbyte = recv(clisock_list[job1],ccuf,MAXLINE,0);//메세지 청취
if(!strncmp(ccuf,"0",1))//1일경우
{
if(job2==0){//도둑이 경찰을 맞출경우
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);

}
return;
}

else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
//못맞출경우
}


else if(!strncmp(ccuf,"1",1))
{
if(job2==1){
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);

}
else if(!strncmp(ccuf,"2",1))
{
if(job2==2){
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
}
else if(!strncmp(ccuf,"3",1))
{
if(job2==3){
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
}
else
{
if(job2==4){
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
}
}
//도둑 턴 종료
}
//경찰 턴 시작
for(i=0;i<num_chat;i++){
if(i==job2){
send(clisock_list[i],police_command,strlen(police_command),0);//경찰에게만

if(FD_ISSET(clisock_list[job2],&read_fds));
nbyte = recv(clisock_list[job2],ccuf,MAXLINE,0);//메세지 청취
if(!strncmp(ccuf,"0",1))//1일경우
{
if(job1==0){//경찰 도둑을을 맞출경우
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
//못맞출경우
}
else if(!strncmp(ccuf,"1",1))
{
if(job1==1){
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);

}
else if(!strncmp(ccuf,"2",1))
{
if(job1==2){
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);

}
else if(!strncmp(ccuf,"3",1))
{
if(job1==3){
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
}
else
{
if(job1==4){
send(clisock_list[job1],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
}
}

}
//낮 시작
while(turn<10){
for(i=0;i<num_chat;i++)
send(clisock_list[i],morningmessage,strlen(morningmessage),0);
n=0;
while(n<5){
FD_ZERO(&read_fds);
FD_SET(listen_sock,&read_fds);
for(i=0;i<num_chat;i++)
FD_SET(clisock_list[i],&read_fds);
maxfdp1 = getmax()+1;
if(select(maxfdp1,&read_fds,NULL,NULL,NULL)<0)
errquit("select fail");
if(FD_ISSET(listen_sock,&read_fds))
{
accp_sock=accept(listen_sock,(struct sockaddr*)&cliaddr,&addrlen);
if(accp_sock==-1)
errquit("accept fail");
}
for(i=0;i<num_chat;i++){
if(FD_ISSET(clisock_list[i],&read_fds)){
nbyte = recv(clisock_list[i],cuf,MAXLINE,0);
switch(i)
{
case 0:
cuf[nbyte-1]='-';
cuf[nbyte]='0';
break;
case 1:
cuf[nbyte-1]='-';
cuf[nbyte]='1';
break;
case 2:
cuf[nbyte-1]='-';
cuf[nbyte]='2';
break;
case 3:
cuf[nbyte-1]='-';
cuf[nbyte]='3';
break;
case 4:
cuf[nbyte-1]='-';
cuf[nbyte]='4';
break;
}
for(j=0;j<num_chat;j++)
if(i!=j)
send(clisock_list[j],cuf,nbyte+1,0);
}
}
printf("%s\n",cuf);
n=n+1;
}
//의심 필요시 } 추가
printf("투표 진행을 시작합니다.");
for(k=0;k<num_chat;k++)
{
send(clisock_list[k],vote,strlen(vote),0);
mvote[k]=0;
}
for(i=0;i<num_chat;i++){
if(FD_ISSET(clisock_list[i],&read_fds));
nbyte = recv(clisock_list[i],vvote,MAXLINE,0);

if(!strncmp(vvote,"0",1))
mvote[0]+=1;
else if(!strncmp(vvote,"1",1))
mvote[1]+=1;
else if(!strncmp(vvote,"2",1))
mvote[2]+=1;
else if(!strncmp(vvote,"3",1))
mvote[3]+=1;
else
mvote[4]+=1;


if(mvote[i]>=max)
max=i;
}
printf("진짜 도둑은 %d",job1);
printf("가장 많은 투표를 받은 사람은  : %d",max);
sprintf(howmanyvote,"%s %d 입니다",voteresult,max);
for(i=0;i<num_chat;i++)
{
send(clisock_list[i],howmanyvote,strlen(howmanyvote),0);
} //가장많은득표
if(max==job1){
for(i=0;i<num_chat;i++)
send(clisock_list[i],correct_command,strlen(correct_command),0);
return;
}
else
{          //아닌경우
for(i=0;i<num_chat;i++)
send(clisock_list[i],incorrect_command,strlen(incorrect_command),0);//아니라는 말과 밤으로 넘어간다.
}
for(i=0;i<num_chat;i++){
if(i==job1)
{
send(clisock_list[i],nmafiamessage,strlen(nmafiamessage),0);//도둑에게만

if(FD_ISSET(clisock_list[job1],&read_fds));
nbyte = recv(clisock_list[job1],ccuf,MAXLINE,0);//메세지 청취
if(!strncmp(ccuf,"0",1))//1일경우
{
if(job2==0){//도둑이 경찰을 맞출경우
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);

}
return;
}

else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
//못맞출경우
}


else if(!strncmp(ccuf,"1",1))
{
if(job2==1){
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);

}
else if(!strncmp(ccuf,"2",1))
{
if(job2==2)
{
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
}
else if(!strncmp(ccuf,"3",1))
{
if(job2==3)
{
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
}
else
{
if(job2==4){
send(clisock_list[job1],pcorrect_command,strlen(pcorrect_command),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],mafiavictory,strlen(mafiavictory),0);
}
return;
}
else
send(clisock_list[job1],pincorrect_command,strlen(pincorrect_command),0);
}
}
//도둑 턴 종료
}
//경찰 턴 시작
for(i=0;i<num_chat;i++)
{
if(i==job2)
{
send(clisock_list[i],police_command,strlen(police_command),0);//경찰에게만

if(FD_ISSET(clisock_list[job2],&read_fds));
nbyte = recv(clisock_list[job2],ccuf,MAXLINE,0);//메세지 청취
if(!strncmp(ccuf,"0",1))//1일경우
{
if(job1==0)
{//경찰 도둑을을 맞출경우
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++)
{
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
//못맞출경우
}
else if(!strncmp(ccuf,"1",1))
{
if(job1==1){
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
}
else if(!strncmp(ccuf,"2",1))
{
if(job1==2){
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);

}
else if(!strncmp(ccuf,"3",1))
{
if(job1==3){
send(clisock_list[job2],victory,strlen(victory),0);
for(i=0;i<num_chat;i++){
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
}
else
{
if(job1==4)
{
send(clisock_list[job1],victory,strlen(victory),0);
for(i=0;i<num_chat;i++)
{
send(clisock_list[i],policevictory,strlen(policevictory),0);
}
return;
}
else
send(clisock_list[job2],nvictory,strlen(nvictory),0);
}
}
//추가 while(1)
}
}//추가 trun
}
return 0;
}
}
}
void addClient(int s,struct sockaddr_in *newcliaddr){
char buf[20];
int i=0;
maxfdp1=getmax()+1;
inet_ntop(AF_INET,&newcliaddr->sin_addr,buf,sizeof(buf));
printf("new client:%s\n",buf);
clisock_list[num_chat]=s;
num_chat++;
maxfdp1 = getmax() +1;
}
void removeClient(int s){
close(clisock_list[s]);
maxfdp1=getmax()+1;
if(s!=num_chat-1)
clisock_list[s] = clisock_list[num_chat-1];
num_chat--;
printf("채팅 참가자 1명 탈퇴 현재 참가자수 =%d\n",num_chat);
}
int getmax()
{
int max=listen_sock;
int i;
for(i=0;i<num_chat;i++)
if(clisock_list[i] >max)
max = clisock_list[i];
return max;
}
int tcp_listen(int host,int port,int backlog){
int sd;
struct sockaddr_in servaddr;
sd= socket(AF_INET,SOCK_STREAM,0);
if(sd==-1)
{
perror("socket fail");
exit(1);
}
bzero((char*)&servaddr,sizeof(servaddr));
servaddr.sin_family=AF_INET;
servaddr.sin_addr.s_addr = htonl(host);
servaddr.sin_port = htons(port);
if(bind(sd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
{
perror("bind fail");
exit(1);
}
listen(sd,backlog);
return sd;
}
