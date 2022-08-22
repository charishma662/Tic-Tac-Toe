#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080


void send_message(int socknum, char *msg){
	int size = strlen(msg);
	int val = send(socknum,&msg[0],size,0);
	if(val!=size)
		printf("Send Error\n");
		
}

char *find_mat(char out[],int val){
	char *ret = (char *)calloc(100,sizeof(char));
	ret[0] = 0;
	int i=0;
	int c =0;
	while(1){
		char x = out[i+val];
		strncat(ret,&x,1);
		if(x=='\n')
			c++;
			
		if(c==3)
			break;
		
		i++;
	}
	return ret;
		
}
	
	

int main(){

	char buffer[10000];
	buffer[0] = 0;
	int socknum = socket(AF_INET,SOCK_STREAM,0);
	if(socknum<0){
		printf("Socket error \n");
		return 0;
	}
	
	struct sockaddr_in adds;
	adds.sin_family = AF_INET;
	adds.sin_port = htons(PORT);
	int v2 = inet_pton(AF_INET,"127.0.0.1",&adds.sin_addr);
	if(v2<=0){
		printf("address invalid\n");
		return -1;
	}
	
	int conn = connect(socknum,(struct sockaddr*)&adds, sizeof(adds));
	if(conn<0){
		printf("connection failed\n");
		return -1;
	}
	else{
		printf("Connected to the game server \n");
	}
	
	//buffer[0] = 0;
	
	int valread = read(socknum,buffer,4096);
	buffer[valread] = 0;

	printf("buffer in %s \n",buffer);
	
	char out[10000];
	
	strcpy(out,buffer);
	printf("out in %s \n",out);
	
	buffer[0] = 0;
	printf(" the c is here %c \n",out[0]);
	if(out[1]=='0'){
	
		int plyid = out[3] -'0';
		printf("Your player ID is: %d \n",plyid);
		printf("Waiting for a partner to join\n");
		
		valread = read(socknum,buffer,10000);
		printf("buffer is inside %s\n",buffer);
		buffer[valread] = 0;
		strcpy(out,buffer);
		buffer[0] = 0;  
	
	}
	else{
	
		printf("Wrong message \n");
		//printf("HI");
		return 0;
	
	
	}
	
	if(out[0]=='1'){
		 int partid = out[2]-'0';
		 char let = out[4];
		 
		char *x = find_mat(out,8);
		/*char board[30] = "";
		int count= 0;
		for(int i=0;;i++){
			char x = out[i+8];
			strncat(board,&x,1);
			if(x == '\n')
				count++;	
			if(count==3)
				break;
		}  */
		
		
		printf("Your Partner's ID is %d . Your symbol is %c \n",partid,let);
		printf("Starting the game... \n");
		printf("%s",x);
	}
	else{
		printf("Wrong msg\n");
		return 0;
	}
				
	// cout<<out<<"\n";
	
	/*printf("Your Partner's ID is %d . Your symbol is %c \n",partid,let);
	printf("Starting the game... \n");
	printf("%s",board);  */
	
	while(1){
		buffer[0] = 0;
		valread = read(socknum,buffer,10000);
		if(valread==0){
			printf("server Disconnected \n");
			printf("closing game\n");
			return 0;
		}
		
		buffer[valread] = 0;
		strcpy(out,buffer);
		
		//printf("out = %s", out);
		switch(out[0]){
		
			case '1' :
				printf("Rematch acccepted. starting the match again\n");
				int part;
				part = out[2]-'0';
				char let = out[4];
				char *x22 = find_mat(out,6);
				/*char brd1[30] = "";
				int count2 = 0;
				for(int i=0;;i++){
					char x = out[i+6];
					strncat(brd1,&x,1);
					if(out[i+6]=='\n');
						count2++;
					if(count2 == 3)
						break;
				}  */
				
				printf("Your partner's ID is %d . Your symbol is %d \n",part,let);
				printf("Starting the game...\n");
				printf("%s\n",x22);
				break;
				
				
				
				
			
			case '3' :
				
				printf("Invalid move\n");
				printf("Please try again\n");
				char *x33 = find_mat(out,2);
				/*char brd2[30] = "" ;
				int count2 = 0;
				for(int i=0;;i++){
					char x = out[i+2];
					strncat(brd2,&x,1);
					if(out[i+2] == '\n')
						count2++;
					if(count2 ==3)
						break;
				}  */
				
				printf("%s\n",x33);  
				
				
				break;
				
			
			
			case '4' :
				printf("\n");
				char *x44 = find_mat(out,2);
				printf("%s \n",x44);
				break;
				
				
				
			case '5' :
				printf("THE END\n");
				if(out[2] == 'L')
					printf("You lost, Better luck next time!\n");
					
				else if(out[2]=='W')
					printf("You won! Good game\n");
					
					
				else if(out[2] =='D')
					printf("Oops! It's a draw\n");
					
				else
					printf("argh!time out\n");

				
				printf("Rematch (1 for Y) (0 for no\n");
			
				int ans;
				scanf("%d",&ans);
			
				if(ans == 1){
					send_message(socknum, "2 1");
				}
				else{
					send_message(socknum, "2 0");
				}
				
				break;
				
			case '2' :
				
				printf("Enter (ROW,COL) for placing your mark: ");
				time_t st;
				time_t end;
				time(&st);
				int row, col;
				scanf("%d %d",&row,&col);
				
				time(&end);
				//printf("%d %d\n",row,col);
				// printf("time taken to given input is %d  \n",end-st);
				if(end-st>15) { //time to fill the row
					printf("Time out!\n");
					printf("Sorry exiting the game for slow response \n");
					return 0;
				}
				
				// printf("row  : %d and col : %d \n ",row,col);
				//fflush(stdout);
				char msg4[10] = "1 ";
				//strcat(msg4,"1 ");
				char x1 = (char)(row+(int)('0'));
				strncat(msg4,&x1,1);
				char x5 = ' ';
				strncat(msg4,&x5,1);
				char x2 = (char)(col+ (int)('0'));
				strncat(msg4,&x2,1);
				//strcat(msg4," ");
			//	strncat(msg4,&x2,1);
				printf("msg sent is %s \n",msg4);
				send_message(socknum, msg4);
				break;
				
			case '6' :
				
				printf("opponent didnt make it in time \n");
				return 0;
				break;
			
			case '7' :
				printf("No rematch.\n");
				return 0;
			
			
				
			/*default :
				printf("E game lo ninnu evadra ranichindhi\n");
				return 0; */
		}
	}
	
	return 0;
				
}
