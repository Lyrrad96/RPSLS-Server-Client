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

void add_null_terminator (char * message) {
	for (int x = 0; x < sizeof(message); x++) {
        if (message[x] == '\r' && (x + 1) < sizeof(message)) {
        	if (message[x + 1] == '\n') {
				message[x] = '\0';
			}
		}
	}

}

int proper_play(char * c) {
    if ( (c[0] == 'r'||c[0] == 's' || c[0] == 'p'|| c[0] == 'l'|| c[0] == 'S' || c[0] == 'e') && c[1] == '\r' && c[2] == '\n') {
        // return true.
        return 1;
    }
    // return false.
    return 0;
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

int substring(char *str, char *substr, int substr_len) {
	int count = 0;

	for (int c = 0; c < BUFSIZE; c++) {
		if (str[c] == substr[c]) {
			count++;
		}
		if (c > substr_len) {
			break;
		}
	}

	if (count == substr_len) {
		return 1;
	} else {
		return 0;
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

int main (int argc, char **argv) {

	int PORT = 60000;

	if (argc <= 1 || argc > 3) {
		fprintf(stderr, "Usage: rpsls_client hostname n(optional)\n");
		exit(1);
	} else if (argc == 3) {
		int n = strtol(argv[2], NULL, 10);
		PORT += n;
	}

	/***************************************************************************
	 * Start (Intialize client-side socket).
	 ***************************************************************************/

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("sockect");
		exit(1);
	}

	struct sockaddr_in serv_addr;

	serv_addr.sin_family = PF_INET;
	serv_addr.sin_port = htons(PORT);
	memset(&(serv_addr.sin_zero), 0, 8);


	struct hostent *h_ip = gethostbyname(argv[1]);
	if(h_ip == NULL) {
		fprintf(stderr, "unknown host %s\n", argv[1]);
		exit(1);
	}

	serv_addr.sin_addr = *((struct in_addr *) h_ip->h_addr);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		PORT += 1;
		serv_addr.sin_port = htons(PORT);
		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			perror ("connect");
			exit(1);
		}
	}
	// REMOVE BEFORE SUBMISSION. (TESTING PURPOSE ONLY)
	printf("Connected to server.\n");

	/***************************************************************************
	 * End.
	 ***************************************************************************/

	char message[BUFSIZE];
	char user_name[BUFSIZE];
	char user_play[USERPLAYSIZE] = {'a', '\0', '\0'};
	char next_round[USERPLAYSIZE] = {'y', '\r', '\n'};

	memset(message, '\0', sizeof(message));
	memset(user_name, '\0', sizeof(message));


	printf("Welcome! Please enter your name: ");
	scanf("%s", user_name);
	int user_name_len = strlen(user_name);
	// Clear input buffer.
	while ((getchar()) != '\n');

	// Null terminator must exist.
	add_network_line(user_name);
	write(sockfd, user_name, BUFSIZE);

	// Read the names and print it.
	read(sockfd, message, BUFSIZE);
	add_null_terminator(message);
	printf("%s", message);

	while(if_exit(next_round) != 0) {
		memset(message, '\0', sizeof(message));

		printf("What is your play? (choose from: r, s, p, l, S, e): ");
		scanf("%s", user_play);
		add_network_line(user_play);

		while (proper_play(user_play) != 1) {
			printf("Try again. Enter valid input: ");
			scanf("%s", user_play);
			add_network_line(user_play);
		}

		if (proper_play(user_play) == 1) {
			write(sockfd, user_play, USERPLAYSIZE);
		}

		// Read if next round is a go.
		read(sockfd, next_round, USERPLAYSIZE);

		// Read winner or display of game ends.
		read(sockfd, message, BUFSIZE);
		add_null_terminator(message);
		printf("%s", message);

		if (if_exit(next_round) != 0) {
			//WRITES TO STDERR BUT NEED TO CHECK WHO IS WHICH CLIENT.
			char stderr_text[6];
			if (substring(message, user_name, user_name_len) == 1) {
				strcpy(stderr_text, "won\n");
				write(STDERR_FILENO, stderr_text, sizeof(stderr_text));
			} else {
				strcpy(stderr_text, "lose\n");
				write(STDERR_FILENO, stderr_text, sizeof(stderr_text));
			}
		}

	}

	memset(message, '\0', BUFSIZE);
	// Remove junk from stream.
	char dump[BUFSIZE];
	read(sockfd, dump, BUFSIZE);

	// Read game stats.
	int tot_game, p1_score, p2_score;
	read(sockfd, (int *)&tot_game, sizeof(tot_game));
	printf("Total Games Played: %d\n", tot_game);

	// Read message and print it in order.
	read(sockfd, message, BUFSIZE);
	printf("%s", message);
	read(sockfd, (int *)&p1_score, sizeof(p1_score));
	read(sockfd, (int *)&p2_score, sizeof(p2_score));
	printf("%d-%d\n", p1_score, p2_score);

	close(sockfd);
	return 0;
}
