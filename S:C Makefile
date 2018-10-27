
CFLAGS = -g -Wall -std=gnu99

all: rpsls_server rpsls_client

rpsls_server: rpsls_server.c
	gcc ${CFLAGS} $^ -o $@

rpsls_client: rpsls_client.c
	gcc ${CFLAGS} $^ -o $@

clean:
	rm rpsls_server rpsls_client
