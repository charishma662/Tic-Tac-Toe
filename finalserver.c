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

#define MAX 80
#define PORT 8080


char mat[3][3];

int check[3][3];
void game_started();
int check_winner(int,int);
char* find_mat();
void print_start(int,int,FILE*);
void send_message(int, char*);
int check_draw();
int play_game(int,int,char);


void game_started(){
	//fprintf(fp, "Game server started. Waiting for players\n");
	//fprintf(fp,"Server start time : %d ms \n",clock());
	printf("Game server started. Waiting for players\n");
	printf("Server start time : %d ms \n",clock());
}

int check_winner(int row,int col){
	
	int rowb = 0;  //implies that row is not matched
	if((mat[row][0]=='O' && mat[row][1] == 'O' && mat[row][2] == 'O') ||  (mat[row][0]=='X' && mat[row][1] == 'X' && mat[row][2] == 'X'))
		rowb = 1;    //row is matched
	 
	 
	//col_check
	
	int colb = 0;
	
	if((mat[0][col]=='O' && mat[1][col] == 'O' && mat[2][col] == 'O') || ((mat[0][col]=='X' && mat[1][col] == 'X'&& mat[2][col] == 'X')))
		colb = 1;   //col is matched
		
		

	// diagonal check;
	int diag = 0;
	
	if(((mat[0][0]=='O' && mat[1][1] == 'O' && mat[2][2] == 'O') || (mat[2][0] == 'O' && mat[1][1] == 'O' && mat[0][2] == 'O') ) || ((mat[0][0]=='X' && mat[1][1] == 'X' && mat[2][2] == 'X') || (mat[2][0] == 'X' && mat[1][1] == 'X' && mat[0][2] == 'X')))
		diag = 1;  // matched
		
	return (rowb || colb || diag);   //0  indicates failed
	
}	


 char* find_mat(){
 
	
	char *ret = (char *)calloc(100,sizeof(char));
	ret[0] = 0;
	
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(j!=2){
				if(mat[i][j]=='X' || mat[i][j] == 'O'){
					strcat(ret," ");
					//printf("before is %c\n",mat[i][j]);
					strncat(ret,&mat[i][j],1);
					//printf("after is %c\n", mat[i][j]);
					strcat(ret,"  |");
					
					
				}
				else{
					fflush(stdout);
					strcat(ret," __ |");
				}
			}
			else{
				if(mat[i][j]=='X' || mat[i][j] == 'O'){
					strcat(ret," ");
					strncat(ret,&mat[i][j],1);
					strcat(ret,"  \n");
				}
				else{
					
					strcat(ret," __ \n");
				}
					
			}
		}
	}
	
			
	return ret;			
}
		
void print_start(int current,int sock,FILE *fp){
	printf("Player %d joined the sever \n", current);
	fp = fopen("output4.txt","a");
	fprintf(fp,"Player %d joined the sever time : %d ms \n ", current,clock());
	fclose(fp);
	
	char x = (char)(current + (int)('0'));
	int val = (send(sock, &x,1,0));
	if(val!=1){
		printf("Send ID Error");
		return;
	}
}

void send_message(int sock,char *message){
	int len = strlen(message);
	
	int mess = send(sock,&message[0],len,0);
	//printf("hi prends message = %s\n", message);
	if(mess!=len){
		printf("send error for message from server to client");
		return;
	}
	
}

int check_draw(){
	int count = 0;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(mat[i][j]=='s')
				count++;
		}
	}
	
	if(count==0)
		return 1;    //draw
		
	return 0;	
}

int play_game(int row, int col, char p){
	// first we have to check whether it is filled or not
	
	int val;
	if((check[row][col]==0 && row >=0 && row < 3 && col >=0 && col < 3)){  //it is not filled
		mat[row][col] = p;
		check[row][col] = 1;
		int value = check_winner(row,col);
		if(value==1)
			return 2;  //winner
		else{
			int x = check_draw();
			if(x==1) //draw
				return 5;
			else
				return 3;
				
		}
	}
	else{   //it is filled error
		return 4;
	}
}
			
			
int main(){



	//int master;
	int ply1_sock;
	int ply2_sock;
	int playerid1 = 1;
	int playerid2 = 2;
	int id = 1;
	int avail;
	int store[2];
	int ply1;
	int ply2;
	int sd;
	int sd2;
	int out[10000];
	
	//making all the values of check as 0
	
	
	 
	for(int i=0;i<2;i++)
		store[i] = 0;
	
	char buffer[10000] = {0};
	
	ply1_sock = 0;
	ply2_sock = 0;
	int addlen;
	
	int arr2[2];
	/*for(int i=0;i<2;i++){
		arr2[i]=0;
	}  */
		
	int val8 = 1;
	int sock44[2];
	for(int i=0;i<2;i++)
		sock44[i] = 0;
	int master = socket(AF_INET,SOCK_STREAM,0);
	if(master==0){
		printf("socket creation failed");
		return 0;
	}
	int val = setsockopt(master, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));
	if(val<0){
		printf("Setsocket failed \n");
		return 0;
	}
	
	struct sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons(8080);
	
	addlen = sizeof(add);
	int bind1 = bind(master,(struct sockaddr*)&add, sizeof(add));
	if(bind1<0){
		printf("Bind failed\n");
		return 0;
	}
	
	int list1 = listen(master,5);
	if(list1<0){
		printf("listen");
		exit(EXIT_FAILURE);
	}
	
	FILE *fp;
	fp = fopen("output4.txt","w");
	
	fprintf(fp, "Game server started. Waiting for players\n");
	fprintf(fp,"Server start time : %d ms \n",clock());
	
	fclose(fp);
	
	
	
	game_started(fp);
	
	int max_sd;
	fd_set readfds;
	int act;
	int sock1;
	
	int again1 = -1;
	int again2 = -1;
	
	int boo = 0;
	int valread;
	int row;
	int col;
	int socknum1;
	int socknum2;

	while(1) {
		if(boo==0){
			for(int i=0;i<3;i++){
				for(int j=0;j<3;j++){
					check[i][j] = 0;
				}
			}
		
			for(int i=0;i<3;i++){
				for(int j=0;j<3;j++){
					mat[i][j] = 's';
				}
			}
		}
		
		boo = 1;
		
		FD_ZERO(&readfds);
		FD_SET(master,&readfds);
		
		
		max_sd = master;
		
		for(int i=0;i<2;i++){
			sd = sock44[i];
			if(sd>0)
				FD_SET(sd,&readfds);
				
			if(sd>max_sd)
				max_sd = sd;
		}
		
			
			
		act = select(max_sd+1,&readfds,NULL,NULL,NULL);
		if(act<0){
			if(errno != EINTR){
				printf("Select Failed \n");
				return 0;
			}
			
		}
		
		int val3 = FD_ISSET(master,&readfds);
		if(val3==1){
			sock1 = accept(master,(struct sockaddr *)&add, (socklen_t*)&addlen);
			if(sock1<0){
				printf("fail accept");
				return 0;
			}
			
			int current;
			current = id++;
			int socknum;
			int flag = 0;
			
			for(int i=0;i<2;i++){
				if(sock44[i]==0){
					sock44[i] = sock1;
					socknum = i;
					flag = 1;
					break;
				}
			}
			if(flag==0){
				printf("No sockets are available\n");
				return 0;
				
			}  
			
			print_start(current,sock1,fp);
			
			char msg_0[4] = "0 ";
			char ch_0 = (char)(current+(int)('0'));
			strncat(msg_0,&ch_0,1);
			
			send_message(sock1,msg_0);
			
			sleep(1); 
			int val11;
			val11 = current%2;
			
			char val = (char) (val11+ (int)('0'));
			
			switch(val){
			
			case '1' :
				avail = current;
				ply1 = current;
				ply1_sock = sock1;
				socknum1 = socknum;
				break;
				
			case '0' :
				
				ply2_sock = sock1;
				ply2 = current;
				int partner= avail;
				socknum2 = socknum;
				
				fp = fopen("output4.txt","a");
				fprintf(fp,"PLayer1 ID : %d , Player2 ID : %d \n", ply1,ply2);
				fclose(fp);
				printf("PLayer1 ID : %d , Player2 ID : %d \n", ply1,ply2);
				//fprintf(fp,"PLayer1 ID : %d , Player2 ID : %d \n", ply1,ply2);
				
				char msg1[10] = "1 ";
				char x = (char)(current+(int)('0'));
				strncat(msg1,&x,1);
				strcat(msg1," X ");
				
				
				char *x_msg = find_mat();
				strcat(msg1,x_msg);
				
				char msg2[10] = "1 ";
				char x2 = (char)(partner +(int)('0'));
				strncat(msg2,&x2,1);
			//	char ch4 = 'O';
				strcat(msg2," O ");
				
				strcat(msg2,find_mat());
				
				send_message(ply1_sock,msg1);
				send_message(ply2_sock,msg2);
				
				sleep(1);
				char msg3[10] = "2";
				send_message(ply1_sock,msg3);
				
				break;
				
				
			}
			
		}
			int sock4;
			for(int i=0;i<2;i++){
			//printf("i = %d\n", i);
			//fflush(stdout);
				sock4 = sock44[i];
				int succ = FD_ISSET(sock4,&readfds);
				if(succ==1){
					//printf("at that instant the value read from buffer is %s \n",buffer);
					valread = read(sock4,buffer,10000);
					//printf("at that instant the value read from buffer is %s \n",buffer);
					if(valread==0){
						getpeername(sock4, (struct sockaddr*)&add, (socklen_t*)&addlen);
						printf("Host disconnected, ip %s, port %d \n", inet_ntoa(add.sin_addr), ntohs(add.sin_port));
						close(sock4);
						sock44[i] = 0;
						int part;
						int part_sock;
						if(i==socknum1){
							part_sock = socknum1;
							part = ply2;
						}
						else{
							part_sock = socknum2;
							part = ply1;
							
						}
							
							
						int xx = sock44[part_sock];
						
						
						/*int ind;
						if(i==0){
							arr2[i] = 0;
							ply1_sock = 0;
							ind = i;
						}	
						if(i==1){
							arr2[i] = 0;
							ply2_sock = 0;
							ind = i;
						} 
						
						int part;
						int part_sock;
						if(i==0)
							part_sock = ply1_sock;
						if(i==1)
							part_sock = ply2_sock;  */
							
						char msg1[2] = "6";
						send_message(xx,msg1);
						getpeername(sock4, (struct sockaddr*)&add, (socklen_t*)&addlen);
						printf("Host disconnected, ip %s, port %d \n", inet_ntoa(add.sin_addr),ntohs(add.sin_port));
						close(xx);
						sock44[part_sock]=0;
					//	goto(exit_server);
						
					}
					
				//	buffer[valread] = 0;
				
				else{
					buffer[valread] = 0;
					
					switch(buffer[0]){
					//if(buffer[0] == '1'){
					case '1' :
						printf("\n");
						int row;
						int col;
						int ply;
						int part;
						int sc = i;
						if(i==socknum1){
							ply = ply1;
							part = ply2;
						}
						else{
							ply = ply2;
							part = ply1;
						}
						
						fp =fopen("output4.txt","a");
						fprintf(fp,"player ID %d made the move for (ROW : %d,COL: %d)\n",ply,row,col);
						fclose(fp);	
						row = (buffer[2]-'0');
						col = (buffer[4]-'0');
						
						int ret;
						int flag3 = 0;
						
						if(ply==ply1){
							ret = play_game(row,col,'X');  //first player played
							flag3 = 1;
						}
						else
							ret = play_game(row,col,'O');
							
						
						//char t11 = (char)(ret+(int)('0'));
						
						//switch(t11){	
						if(ret == 4){
							printf("Invalid move\n");
							char msg6[20] = "3 ";
							strcat(msg6,find_mat());
							if(ply==ply1)
								send_message(ply1_sock,msg6);
							else
								send_message(ply2_sock,msg6);
								
							if(ply==ply1)
								send_message(ply1_sock,"2");
							else
								send_message(ply2_sock,"2");
							
							
							continue;
						}
						else if(ret ==2){  // winner
							char msg6[20] = "5 ";
							char msg7[20] = "5 ";
							if(flag3==1) { //first player won
								char ch = 'L';
								char ch2 = 'W';
								strncat(msg6,&ch,1);
								strncat(msg7,&ch2,1);
								printf("player ID %d won the game\n",ply);
								fp = fopen("output4.txt","a");
								fprintf(fp,"player ID %d won the game\n",ply);
								fclose(fp);
							}	
							else{
								char ch = 'W';
								char ch2 = 'L';
								strncat(msg6,&ch,1);
								strncat(msg7,&ch2,1);
								fp = fopen("output4.txt","a");
								fprintf(fp,"player ID %d won the game\n",ply);
								fclose(fp);
								printf("player ID %d won the game\n",ply);
								
								
							}
								
							send_message(ply1_sock,msg7);
							send_message(ply2_sock,msg6);
						}
						
					
						else if(ret==5){
							char msg6[20] = "5 ";
							char msg7[20] = "5 ";
							strcat(msg6,"D");
							strcat(msg7,"D");
							
							send_message(ply1_sock,msg6);
							send_message(ply2_sock,msg7);
							
							printf("The game is draw\n");
							fp = fopen("output4.txt","a");
							fprintf(fp,"The game is draw\n");
							fclose(fp);
						}
						else if(ret==3){
							char msg6[20] = "4 ";
							strcat(msg6,find_mat());
							send_message(ply1_sock,msg6);
							send_message(ply2_sock,msg6);
							sleep(1);
							if(ply==ply1){
                    						send_message(ply2_sock, "2");
                    					}
                    					else{
                    						send_message(ply1_sock, "2");
                    					}
                    					continue;
                    				}
                    				
                    				printf("Game ends at time = %d ms \n",clock());
                    				fp = fopen("output4.txt","a");
                    				fprintf(fp, "games end at time = %d ms \n",clock());
                    				fclose(fp);
                    			
                    				break;
                    				
                    				
                    			//else if(buffer[0]== '2'){
                    			case '2' :
                    				printf("\n");
                    				int d;
                    				d = buffer[2]-'0';
                    				int pl;
                    				//int part1;
                    				if(i==0){
                    					pl = ply1;
                    					//part1 = ply2;
                    				}else{
                    					pl = ply2;
                    					//part1 = ply2;
                    				}
                    				
                    				if(pl==ply1)
                    					again1 = d;
                    				else
                    					again2 = d;
                    					
                    				if(again1 == -1 || again2 == -1)
                    					continue;
                    				
                    				
                    				//int sc1, sc2;
                				//sc1 = client_socket[socknum_1];
                				//sc2 = client_socket[socknum_2];
                				
                				
                				if(again1 == 1 && again2 ==1)
                				{
                					char msg8[50] = "1 ";
                					char msg9[50] = "1 ";
                					
                					
                					
                					char x33 = ((char)(ply1+(int)('0') ));
     							strncat(msg8,&x33,1);
     							strcat(msg8," X ");
     						//	char msg10[20] = find_mat();
     							strcat(msg8,find_mat());
     							
     							char x22 = (char)(ply2+ (int)('0'));
     							strncat(msg9, &x22,1);
     							strcat(msg9," O ");
     							//char msg11[20] = find_mat();
     							strcat(msg9,find_mat());
     							
     							send_message(ply1_sock,msg8);
     							send_message(ply2_sock,msg9);
     							sleep(1);
     							send_message(ply1_sock,"2");
     							boo = 0;
     						}	
     						else{
     							send_message(ply1_sock, "7");
     							send_message(ply2_sock,"7");	
     						}
     						
     						break;
     						
     					}  //switch
     				}    //else
     			}
     			
			
     		}
	}
	
	return 0;
}
