#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h> /* Internet domain header */
#include <arpa/inet.h>	/* only needed on my mac */
#include <netdb.h>		/* gethostname */

#define USERPLAYSIZE 3
#define BUFSIZE 100
#define CHECK_INDEX 0
#define MAXCHAR 1000

// GOLBAL VAR.
int tot_game = 0;
int c1_win = 0;
int c2_win = 0;
int tie = 0;
int i, n;
struct replace
{
	/* data */
	char name[100];
	int w, l, t, p, d;
};
char s[500];
int no = 0;
struct replace r[100];
int search(struct replace *arr, int n, char *x);

void Sort(struct replace *arr, int no);
void display()
{
	printf("Sl.No\tName\tWin\tLose\tTie\tPoints\tDiff\n");

	for (int j = 0; j <= no; j++)
	{
		printf("%d\t%s\t%d\t%d\t%d\t%d\t%d\n", j + 1, r[j].name, r[j].w, r[j].l, r[j].t, r[j].p, r[j].d);
	}
}
void in()
{
	FILE *f;
	int x = 0;
	f = fopen("HoF.txt", "r");

	while (fgets(s, 100, f) != NULL)
	{
		for (int l = 0; l < 2; l++)
			for (int k = 0; k < 100; k++)
			{
				s[k] = s[k + 1];
			}
		for (int i = 0; !isspace(s[i]); i++)
		{
			r[x].name[i] = s[i];
		}
		for (int l = 0; l < strlen(r[x].name) + 1; l++)
			for (int k = 0; k < 100; k++)
			{
				s[k] = s[k + 1];
			}
		r[x].w = (s[0]) - '0';
		r[x].l = (s[2]) - '0';
		r[x].t = s[4] - '0';
		r[x].p = s[6] - '0';
		r[x].d = s[8] - '0';
		no = x;

		x++;
	}
}

void out(char *name, int w, int l, int t, int p, int d, int exist)
{
	if (exist == -1)
	{
		no++;
		strcpy(r[no].name, name);
		r[no].w = w;
		r[no].l = l;
		r[no].t = t;
		r[no].p = p;
		r[no].d = d;
	}
	else
	{
		strcpy(r[exist].name, name);
		r[exist].w = w;
		r[exist].l = l;
		r[exist].t = t;
		r[exist].p = p;
		r[exist].d = d;
	}
	Sort(r, no + 1);
}
void print()
{
	FILE *f;
	f = fopen("HoF.txt", "w");
	for (int j = 0; j <= no; j++)
	{
		fprintf(f, "%d %s %d %d %d %d %d\n", j + 1, r[j].name, r[j].w, r[j].l, r[j].t, r[j].p, r[j].d);
	}
	fclose(f);
}
int search(struct replace *arr, int n, char *x)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (!strcmp(arr[i].name, x))
		{
			return i;
		}
	}
	return -1;
}
void Sort(struct replace *arr, int s)
{
	int i, j;
	for (i = 0; i < s - 1; i++)
	{
		// Last i elements are already in place
		for (j = 0; j < s - i - 1; j++)
			if (arr[j].p < arr[j + 1].p)
			{
				struct replace temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
			else if (arr[j].p == arr[j + 1].p && arr[j].d < arr[j + 1].d)
			{
				struct replace temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
	}
}
void add_null_terminator(char *message)
{
	for (int x = 0; x < sizeof(message); x++)
	{
		if (message[x] == '\r' && (x + 1) < sizeof(message))
		{
			if (message[x + 1] == '\n')
			{
				message[x] = '\0';
			}
		}
	}
}

int network_newline_exist(char *buf, int n)
{

	for (int c = 0; c < n; c++)
	{
		if (buf[c] == '\r' && (c + 1) < n)
		{
			if (buf[c + 1] == '\n')
			{
				// Network New Line already exist, Return true.
				return 1;
			}
		}
	}

	// Network new line doesnt exist, Return false.
	return 0;
}

void add_network_line(char *str)
{
	int str_len = strlen(str);

	if (network_newline_exist(str, str_len) != 1)
	{
		// Add the new network line at the null terminator index.
		str[str_len] = '\r';
		str[str_len + 1] = '\n';
	}
}

int if_exit(char *c)
{
	if (c[0] == 'y' && c[1] == '\r' && c[2] == '\n')
	{
		// return true.
		return 1;
	}
	else
	{
		// return false.
		return 0;
	}
}

int is_e(char *c)
{
	if (c[0] == 'e' && c[1] == '\r' && c[2] == '\n')
	{
		// return true.
		return 1;
	}
	else
	{
		// return false.
		return 0;
	}
}

int determine_winner(char user1_play, char user2_play)
{
	tot_game += 1;
	if ((user1_play == 's' && user2_play == 'p') || (user1_play == 'p' && user2_play == 'r') ||
		(user1_play == 'r' && user2_play == 'l') || (user1_play == 'l' && user2_play == 'S') ||
		(user1_play == 'S' && user2_play == 's') || (user1_play == 's' && user2_play == 'l') ||
		(user1_play == 'l' && user2_play == 'p') || (user1_play == 'p' && user2_play == 'S') ||
		(user1_play == 'S' && user2_play == 'r') || (user1_play == 'r' && user2_play == 's'))
	{
		// user 1 won
		c1_win += 1;
		return 1;
	}
	else if ((user1_play == 'r' && user2_play == 'r') || (user1_play == 's' && user2_play == 's') ||
			 (user1_play == 'p' && user2_play == 'p') || (user1_play == 'l' && user2_play == 'l') ||
			 (user1_play == 'S' && user2_play == 'S'))
	{
		// tie, s one won.
		tie += 1;
		return 0;
	}
	else
	{
		// user 2 won.
		c2_win += 1;
		return 2;
	}
}
int main(int argc, char **argv)
{

	int PORT = 60000;

	if (argc > 2)
	{
		fprintf(stderr, "Usage: rpsls_server n(optional)\n");
		exit(1);
	}
	else if (argc == 2)
	{
		int n = strtol(argv[1], NULL, 10);
		PORT += n;
	}

	/**********************************************************************
	 * Start (Initialize server-side socket).
	 **********************************************************************/

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		exit(1);
	}

	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	memset(&(serv_addr.sin_zero), 0, 8);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		PORT += 1;
		serv_addr.sin_port = htons(PORT);
		if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			close(sockfd);
			perror("bind");
			exit(1);
		}
	}

	if (listen(sockfd, 2) < 0)
	{
		perror("listen");
		exit(1);
	}

	int c1_sockfd;
	struct sockaddr_in c1_addr;
	unsigned int c1_addr_len = sizeof(c1_addr);

	c1_sockfd = accept(sockfd, (struct sockaddr *)&c1_addr, &c1_addr_len);
	if (c1_sockfd < 0)
	{
		perror("client 1 accept");
		exit(1);
	}

	int c2_sockfd;
	struct sockaddr_in c2_addr;
	unsigned int c2_addr_len = sizeof(c2_addr);

	c2_sockfd = accept(sockfd, (struct sockaddr *)&c2_addr, &c2_addr_len);
	if (c2_sockfd < 0)
	{
		perror("client 2 accept");
		exit(1);
	}

	//  more connections. Only a two player game.
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

	while (if_exit(next_round) != 0)
	{
		memset(message, '\0', BUFSIZE);

		// Read the gesture from both clients.
		read(c1_sockfd, c1_play, USERPLAYSIZE);
		read(c2_sockfd, c2_play, USERPLAYSIZE);

		if (is_e(c1_play) == 1 && is_e(c2_play) == 0)
		{
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
		}
		else if (is_e(c1_play) == 0 && is_e(c2_play) == 1)
		{
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
		}
		else if (is_e(c1_play) == 1 && is_e(c2_play) == 1)
		{
			next_round[0] = 'n';
			add_network_line(next_round);
			write(c1_sockfd, next_round, USERPLAYSIZE);
			write(c2_sockfd, next_round, USERPLAYSIZE);

			strcpy(message, "Game ended. Both quit.");

			add_network_line(message);
			write(c1_sockfd, message, BUFSIZE);
			write(c2_sockfd, message, BUFSIZE);
		}
		else
		{
			add_network_line(next_round);
			write(c1_sockfd, next_round, USERPLAYSIZE);
			write(c2_sockfd, next_round, USERPLAYSIZE);

			winner = determine_winner(c1_play[CHECK_INDEX], c2_play[CHECK_INDEX]);
			if (winner == 1)
			{
				strcpy(message, c1_name);
				strcat(message, " Won!");

				add_network_line(message);
				write(c1_sockfd, message, BUFSIZE);
				write(c2_sockfd, message, BUFSIZE);
			}
			else if (winner == 2)
			{
				strcpy(message, c2_name);
				strcat(message, " Won!");

				add_network_line(message);
				write(c1_sockfd, message, BUFSIZE);
				write(c2_sockfd, message, BUFSIZE);
			}
			else if (winner == 0)
			{
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

	//read all entries from HoF file
	in();

	//Check if players exist

	if (c1_win > c2_win)
	{
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

		//Write to scorecard
		int exist1 = search(r, sizeof(r) / sizeof(r[0]), c1_name);
		out(c1_name, r[exist1].w + 1, r[exist1].l, r[exist1].t, r[exist1].p + 2, r[exist1].d + c1_win - c2_win, exist1);
		int exist2 = search(r, sizeof(r) / sizeof(r[0]), c2_name);
		out(c2_name, r[exist2].w, r[exist2].l + 1, r[exist2].t, r[exist2].p, r[exist2].d - c1_win + c2_win, exist2);
	}
	else if (c2_win > c1_win)
	{
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

		//Write to scorecard
		int exist1 = search(r, sizeof(r) / sizeof(r[0]), c1_name);

		out(c1_name, r[exist1].w, r[exist1].l + 1, r[exist1].t, r[exist1].p, r[exist1].d + c1_win - c2_win, exist1);
		int exist2 = search(r, sizeof(r) / sizeof(r[0]), c2_name);

		out(c2_name, r[exist2].w + 1, r[exist2].l, r[exist2].t, r[exist2].p + 2, r[exist2].d - c1_win + c2_win, exist2);
	}
	else
	{
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

		//Write to scorecard
		int exist1 = search(r, sizeof(r) / sizeof(r[0]), c1_name);

		out(c1_name, r[exist1].w, r[exist1].l, r[exist1].t + 1, r[exist1].p + 1, r[exist1].d, exist1);
		int exist2 = search(r, sizeof(r) / sizeof(r[0]), c2_name);

		out(c2_name, r[exist2].w, r[exist2].l, r[exist2].t + 1, r[exist2].p + 1, r[exist2].d, exist2);
	}
	FILE *fr, *fw;

	fw = fopen("history.txt", "a");
	fprintf(fw, "\n\t%s\t%d - %d\t%s", c1_name, c1_win, c2_win, c2_name);
	fclose(fw);
	char d[100], o = '0';
	fr = fopen("history.txt", "r");
a:
{
	printf("\n");
	printf("Press r to view results of all previous matches\nPress s to view scorecard\nPress e to exit\n->");
	scanf(" %c", &o);
	printf("\n");

	//	printf("%c\n", h);

	if (o == 'r')
	{
		while (fgets(d, 100, fr) != NULL)
			printf("%s", d);

		printf("\n");
	}
	//printf("press d to view table");
	else if (o == 's')
	{
		print();

		display();
	}
	/*	for(i = 0; i < n+1;i++)
		{
		fgets(str, sizeof str, stdin);
		fputs(str, fptr);
		}*/
	if (o != 'e')
		goto a;
	fclose(fr);
}
	// Close both sockets.
	close(c1_sockfd);
	close(c2_sockfd);

	return 0;
}
int checkUser(char *str, char *wrd)
{

	int i, j, n, m;
	n = strlen(str); // length of str
	m = strlen(wrd); // length of wrd

	// if size of wrd is bigger then str then str can never contain wrd
	if (m > n)
		return 0;

	while (i < n)
	{

		// checking if the current word in str
		// is equal to wrd or not
		j = 0;
		while (i < n && j < m && str[i] == wrd[j])
		{
			++i;
			++j;
		}

		if ((i == n || str[i] == ' ') && j == m)
			return 1;

		while (i < n && str[i] != ' ')
		{
			++i;
		}
		++i;
	}

	return 0;
}

void replace(char *word, char *read, char *replace, FILE *ifp)
{

	FILE *ofp;
	char ch;

	//ifp = fopen("file_search_input.txt", "r");
	ofp = fopen("file_replace_output.txt", "w+");
	if (ifp == NULL || ofp == NULL)
	{
		printf("Can't open file.");
		exit(0);
	}
	//puts("THE CONTENTS OF THE FILE ARE SHOWN BELOW :\n");

	// displaying file contents
	while (1)
	{
		ch = fgetc(ifp);
		if (ch == EOF)
		{
			break;
		}
		printf("%c", ch);
	}
	/*
    puts("\n\nEnter the word to find:");
    fgets(word, 100, stdin);
*/
	// removes the newline character from the string
	word[strlen(word) - 1] = word[strlen(word)];
	/*
    puts("Enter the word to replace it with :");
    fgets(replace, 100, stdin);
*/
	// removes the newline character from the string
	replace[strlen(replace) - 1] = replace[strlen(replace)];

	//fprintf(ofp, "%s - %s\n", word, replace);

	// comparing word with file
	rewind(ifp);
	while (!feof(ifp))
	{

		fscanf(ifp, "%s", read);

		if (strcmp(read, word) == 0)
		{

			// for deleting the word
			strcpy(read, replace);
		}

		// In last loop it runs twice
		fprintf(ofp, "%s ", read);
	}

	// Printing the content of the Output file
	/*rewind(ofp);
    while (1)
    {
        ch = fgetc(ofp);
        if (ch == EOF)
        {
            break;
        }
        printf("%c", ch);
    }
*/
	//	fclose(ifp);s
	fclose(ofp);
}
