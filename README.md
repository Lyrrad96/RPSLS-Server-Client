# Server-Client
A server-client version of the rock-paper-scissor-lizard-splock game created in C using system calls. The game server, which is implemented in rpsls_server.c, will moderate one or more games of rpsls between two clients, which is implemented in rpsls_client.c. Three
processes will be running when a game is being played, the server and two instances of the client.

# How To Play

![](https://codecademy-discourse.s3.amazonaws.com/original/5X/1/e/9/a/1e9ae22826a47a2d2e9f0e8f0f0cdf21a8479715.jpg)

# Rough
First an instance of a server is suppose to be excuted. This server instance uses port 60000 (unless you provide an optional argument "n", in which case, it uses the port 60000 + n) which moderates zero or more games between two players. "SCREENSHOT"
After a server instance is intialized, the two players can play a match by running the client instances. The two clients must connect to the server on the same port that was used to intialize it (i.e. if you used the optional argument to intialize the server, you must use the same optional argument to intialize the client instances when excuting it).

# Server
