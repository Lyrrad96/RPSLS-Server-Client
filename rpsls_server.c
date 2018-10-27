#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <arpa/inet.h>     /* only needed on my mac */
#include <netdb.h>         /* gethostname */

#define USERPLAYSIZE 3
#define BUFSIZE 100
#define CHECK_INDEX 0

// GOLBAL VAR.
int tot_game = 0;
int c1_win = 0;
int c2_win = 0;
int tie = 0;

void add_null_terminator (char * message) {
	for (int x = 0; x < sizeof(message); x++) {
        if (message[x] == '\r' && (x + 1) < sizeof(message)) {
        	if (message[x + 1] == '\n') {
				message[x] = '\0';
			}
		}
	}

}

int network_newline_exist(char *buf, int n) {

    for (int c = 0; c < n; c++) {
        if (buf[c] == '\r' && (c + 1) < n) {
        	if (buf[c + 1] == '\n') {
            	// Network New Line already exist, Return true.
            	return 1;
        	}
        }
    }
    
    // Network new line doesnt exist, Return false.
    return 0;
}

void add_network_line(char *str) {
	int str_len = strlen(str);

	if (network_newline_exist(str, str_len) != 1) {
		// Add the new network line at the null terminator index.
		str[str_len] = '\r';
		str[str_len + 1] = '\n';
	}

}

int if_exit(char * c) {
	if (c[0] == 'y' && c[1] == '\r' && c[2] == '\n') {
		// return true.
		return 1;
	} else {
		// return false.
		return 0;
	}
}

int is_e(char * c) {
	if (c[0] == 'e' && c[1] == '\r' && c[2] == '\n') {
		// return true.
		return 1;
	} else {
		// return false.
		return 0;
	}
}

int determine_winner(char user1_play, char user2_play) {
	tot_game += 1;
	if ((user1_play == 's' && user2_play == 'p') || (user1_play == 'p' && user2_play == 'r') ||
			(user1_play == 'r' && user2_play == 'l') || (user1_play == 'l' && user2_play == 'S') ||
				(user1_play == 'S' && user2_play == 's') || (user1_play == 's' && user2_play == 'l') ||
					(user1_play == 'l' && user2_play == 'p') || (user1_play == 'p' && user2_play == 'S') ||
						(user1_play == 'S' && user2_play == 'r') || (user1_play == 'r' && user2_play == 's')) {
		// user 1 won
		c1_win += 1;
		return 1;
	} else if ((user1_play == 'r' && user2_play == 'r') || (user1_play == 's' && user2_play == 's') ||
				(user1_play == 'p' && user2_play == 'p') || (user1_play == 'l' && user2_play == 'l') ||
					(user1_play == 'S' && user2_play == 'S')) {
		// tie, no one won.
		tie += 1;
		return 0;
	}else {
		// user 2 won.
		c2_win += 1;
		return 2;
	}
}

int main(int argc, char **argv) {

	int PORT = 60000;

	if (argc > 2) {
		fprintf(stderr, "Usage: rpsls_server n(optional)\n");
		exit(1);
	} else if (argc == 2) {
		int n = strtol(argv[1], NULL, 10);
		PORT += n;
	}

	/**********************************************************************
	 * Start (Initialize server-side socket).
	 **********************************************************************/

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	memset(&(serv_addr.sin_zero), 0, 8);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		PORT += 1;
		serv_addr.sin_port = htons(PORT);
		if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			close(sockfd);
			perror ("bind");
			exit(1);
		}
	}

	if(listen(sockfd, 2) < 0) {
		perror("listen");
		exit(1);
	}

	int c1_sockfd;
	struct sockaddr_in c1_addr;
	unsigned int c1_addr_len = sizeof(c1_addr);

	c1_sockfd = accept(sockfd, (struct sockaddr *)&c1_addr, &c1_addr_len);
	if (c1_sockfd < 0) {
		perror("client 1 accept");
		exit(1);
	}


	int c2_sockfd;
	struct sockaddr_in c2_addr;
	unsigned int c2_addr_len = sizeof(c2_addr);

	c2_sockfd = accept(sockfd, (struct sockaddr *)&c2_addr, &c2_addr_len);
	if (c2_sockfd < 0) {
		perror("client 2 accept");
		exit(1);
	} 

	// No more connections. Only a two player game.
	close(sockfd);

	/**********************************************************************
	 * End.
	 **********************************************************************/

	char message[BUFSIZE];
	char c1_name[BUFSIZE];
	char c2_name[BUFSIZE];

	memset(message, '\0', BUFSIZE);
	memset(c1_name, '\0', BUFSIZE);
	memset(c2_name, '\0', BUFSIZE);


	char c1_play[USERPLAYSIZE] = {'z', '\0', '\0'};
	char c2_play[USERPLAYSIZE] = {'z', '\0', '\0'};
	char next_round[USERPLAYSIZE] = {'y', '\r', '\n'};

	// Get the names of both the players.
	read(c1_sockfd, c1_name, BUFSIZE);
	read(c2_sockfd, c2_name, BUFSIZE);
	// Add null terminator and remove it before sending to the client.
	add_null_terminator(c1_name);
	add_null_terminator(c2_name);

	strcpy(message, "Player 1: ");
	strcat(message, c1_name);
	strcat(message, "\nPlayer 2: ");
	strcat(message, c2_name);

	// Sent the names to both the clients.
	add_network_line(message);
	write(c1_sockfd, message, BUFSIZE);
	write(c2_sockfd, message, BUFSIZE);


	int winner;

	while(if_exit(next_round) != 0) {
		memset(message, '\0', BUFSIZE);

		// Read the gesture from both clients.
		read(c1_sockfd, c1_play, USERPLAYSIZE);
		read(c2_sockfd, c2_play, USERPLAYSIZE);

		if (is_e(c1_play) == 1 && is_e(c2_play) == 0) {
			next_round[0] = 'n';
			add_network_line(next_round);
			write(c1_sockfd, next_round, USERPLAYSIZE);
			write(c2_sockfd, next_round, USERPLAYSIZE);
			
			strcpy(message, "Game ended. ");
			strcat(message, c1_name);
			strcat(message, " quit.");

			add_network_line(message);
			write(c1_sockfd, message, BUFSIZE);
			write(c2_sockfd, message, BUFSIZE);
		} else if (is_e(c1_play) == 0 && is_e(c2_play) == 1) {
			next_round[0] = 'n';
			add_network_line(next_round);
			write(c1_sockfd, next_round, USERPLAYSIZE);
			write(c2_sockfd, next_round, USERPLAYSIZE);
			
			strcpy(message, "Game ended. ");
			strcat(message, c2_name);
			strcat(message, " quit.");


			add_network_line(message);
			write(c1_sockfd, message, BUFSIZE);
			write(c2_sockfd, message, BUFSIZE);
		} else if (is_e(c1_play) == 1 && is_e(c2_play) == 1) {
			next_round[0] = 'n';
			add_network_line(next_round);
			write(c1_sockfd, next_round, USERPLAYSIZE);
			write(c2_sockfd, next_round, USERPLAYSIZE);
			
			strcpy(message, "Game ended. Both quit.");

			add_network_line(message);
			write(c1_sockfd, message, BUFSIZE);
			write(c2_sockfd, message, BUFSIZE);
		} else {
			add_network_line(next_round);
			write(c1_sockfd, next_round, USERPLAYSIZE);
			write(c2_sockfd, next_round, USERPLAYSIZE);

			winner = determine_winner(c1_play[CHECK_INDEX], c2_play[CHECK_INDEX]);
			if (winner == 1) {
				strcpy(message, c1_name);
				strcat(message, " Won!");

				add_network_line(message);
				write(c1_sockfd, message, BUFSIZE);
				write(c2_sockfd, message, BUFSIZE);
			} else if (winner == 2) {
				strcpy(message, c2_name);
				strcat(message, " Won!");

				add_network_line(message);
				write(c1_sockfd, message, BUFSIZE);
				write(c2_sockfd, message, BUFSIZE);
			} else if (winner == 0) {
				strcpy(message, "Tie!\n");

				add_network_line(message);
				write(c1_sockfd, message, BUFSIZE);
				write(c2_sockfd, message, BUFSIZE);
			}
		}
	}
	// Clear socket data.
	memset(message, '\0', BUFSIZE);
	write(c1_sockfd, message, BUFSIZE);
	write(c2_sockfd, message, BUFSIZE);

	// Send number total games played.
	write(c1_sockfd, (int *)&tot_game, sizeof(int));
	write(c2_sockfd, (int *)&tot_game, sizeof(int));

	if (c1_win > c2_win) {
		strcpy(message, c1_name);
		strcat(message, " wins by ");
		//add_network_line(message);
		write(c1_sockfd, message, BUFSIZE);
		write(c2_sockfd, message, BUFSIZE);

		// Send player one num wins.
		write(c1_sockfd, (int *)&c1_win, sizeof(int));
		write(c2_sockfd, (int *)&c1_win, sizeof(int));
		//send player 2 num wins.
		write(c1_sockfd, (int *)&c2_win, sizeof(int));
		write(c2_sockfd, (int *)&c2_win, sizeof(int));
	} else if (c2_win > c1_win){
		strcpy(message, c2_name);
		strcat(message, " wins by ");
		//add_network_line(message);
		write(c1_sockfd, message, BUFSIZE);
		write(c2_sockfd, message, BUFSIZE);

		//send player 2 num wins.
		write(c1_sockfd, (int *)&c2_win, sizeof(int));
		write(c2_sockfd, (int *)&c2_win, sizeof(int));
		// Send player one num wins.
		write(c1_sockfd, (int *)&c1_win, sizeof(int));
		write(c2_sockfd, (int *)&c1_win, sizeof(int));
	} else {
		strcpy(message, "No one won! Game tied ");
		//add_network_line(message);
		write(c1_sockfd, message, BUFSIZE);
		write(c2_sockfd, message, BUFSIZE);

		//send player 2 num wins.
		write(c1_sockfd, (int *)&c2_win, sizeof(int));
		write(c2_sockfd, (int *)&c2_win, sizeof(int));
		// Send player one num wins.
		write(c1_sockfd, (int *)&c1_win, sizeof(int));
		write(c2_sockfd, (int *)&c1_win, sizeof(int));
	}

	// Close both sockets.
	close(c1_sockfd);
	close(c2_sockfd);

	return 0;
}
