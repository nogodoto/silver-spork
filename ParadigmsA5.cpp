#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

int main()
{
	printf("Starting up Trading Server\n");
	int PRODUCT = 100;
	SOCKET sock;
	HANDLE ghSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	WSADATA wsaData;
	sockaddr_in server;

	int ret = WSAStartup(0x101, &wsaData); // use highest version of winsock avalaible

	if (ret != 0)
	{
		return 0;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(123);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		return 0;
	}

	if (bind(sock, (sockaddr*)&server, sizeof(server)) != 0)
	{
		return 0;
	}

	if (listen(sock, 5) != 0)
	{
		SOCKET client;
		sockaddr_in from;
		int fromlen = sizeof(from);

		char buf[100];		// buffer to hold received data
		char sendData[100];	// buffer to hold sent data


		int res;	// for error checking 

		client = accept(sock, (struct sockaddr*)&from, &fromlen);
		while (true)
		{
			
			HANDLE WINAPI CreateThread(NULL, 0, receive_cmds, (LPVOID)client, 0, &threadId);
			
			

			{

				// recv/send cmds ...

				res = recv(client, buf, sizeof(buf), 0); // recv cmds

				Sleep(10);

				if (res == 0)
				{
					printf("error\n");
					closesocket(client);
					ExitThread(0);
				}

				if (strstr(buf, "hello"))
				{ // greet this user
					printf("\n Received hello command");

					strcpy(sendData, "Hello, greeting from Server \n");
					Sleep(10);
					send(client, sendData, sizeof(sendData), 0);
				}
				else if (strstr(buf, "bye"))
				{ // disconnected this user
					printf("\n Received bye command \n");

					strcpy(sendData, "See ya!\n");
					Sleep(10);
					send(client, sendData, sizeof(sendData), 0);

					// close the socket associated with this client and end this thread
					closesocket(client);
					ExitThread(0);
				}
				else if (strstr(buf, "buy"))
				{ // Allow user to buy one quant of product
					if (ghSemaphore == 0 && PRODUCT > 0)
					{
						WaitForSingleObject(ghSemaphore, 0L);
						printf("\n Received BUY/PURCHASE command \n");
						PRODUCT--;
						strcpy(sendData, "1 PRODUCT BOUGHT\n"); 
						
						
						
						Sleep(10);
						send(client, sendData, sizeof(sendData), 0);
						ReleaseSemaphore(ghSemaphore,1,NULL);
					}
					else
					{
						printf("\n High Server demand, please wait and retry\n ");
						Sleep(10);
						send(client, sendData, sizeof(sendData), 0);
					}
				else if (strstr(buf, "sell"))
				{ // Allow user to sell one quant of product
					if (ghSemaphore == 0)
					{
						WaitForSingleObject(ghSemaphore, 0L);
						printf("\n Received SELL command \n");

						strcpy(sendData, "1 PRODUCT SOLD\n");
						PRODUCT++;
						Sleep(10);
						send(client, sendData, sizeof(sendData), 0);
						ReleaseSemaphore(ghSemaphore,1,NULL);
					}
					else
					{
						printf("\n High Server demand, please wait and retry\n ");
						Sleep(10);
						send(client, sendData, sizeof(sendData), 0);
					}
				else
				{
					strcpy(sendData, "Invalid command\n");
					Sleep(10);
					send(client, sendData, sizeof(sendData), 0);
				}

				// clear buffers
				strcpy(sendData, "");
				strcpy(buf, "");

			}

			closesocket(sock);
			WSACleanup();

		}
		return 0;
	}

	
	return 0;
}