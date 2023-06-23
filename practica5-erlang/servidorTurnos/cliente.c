#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define BUFF_SIZE 1024
#define N 1

int main()
{
	int sock[N], cto, i;
	struct sockaddr_in servaddr;
	char buffer[BUFF_SIZE], buffer2[BUFF_SIZE];
	struct timespec tsp;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	tsp.tv_sec = 0;
	tsp.tv_nsec = 100000;

	for (i = 0; i < N; i++) {
		int rc;

		printf("Conectando cliente %d...\n", i);
		sock[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (sock[i] < 0) {
			perror("CLNT: Error creando socket");
			return -1;
		}

		/*
		 * Esto sólo está para no agotar los puertos efímeros
		 * de la dirección 127.0.0.1, de los cuales hay aprox
		 * 30000. Hacemos un bind() explicito a puertos e IPs
		 * (de origen) distintas a modo de evitarlo. (Todas
		 * las IP 127.x.y.z representan al host local.)
		 */
		{
			struct sockaddr_in clientaddr;
			int portnum = i % 10000 + 10000;
			int ipaddrnum = i / 10000 + 10;
			char ipaddr[32];

			memset(&clientaddr, 0, sizeof(clientaddr));
			clientaddr.sin_family = AF_INET;
			clientaddr.sin_port = htons(portnum);
			sprintf(ipaddr, "127.0.0.%d", ipaddrnum);
			inet_pton(AF_INET, ipaddr, &clientaddr.sin_addr);
			rc = bind(sock[i], (struct sockaddr *)&clientaddr, sizeof(clientaddr));
			if (rc < 0) {
				perror("ECHOCLNT: Error llamando a bind()");
				return -1;
			}
		}

		rc = connect(sock[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (rc < 0) {
			perror("CLNT: Error conectando");
			return -1;
		}

		nanosleep(&tsp, NULL);
	}

	for (i = 0; i < N; i++) {
		int rc;

		//sprintf(buffer2, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245\n");
		sprintf(buffer2,"NUEVO\nNUEVO\nNU");
		cto = strlen(buffer2);

#if 0
		rc = write(sock[i], buffer2, cto);
		if (rc < 0) {
			perror("CLNT: Error escribiendo");
			return -1;
		}
#else
		/* Escribe de a un byte, tal vez útil para testear el servidor */
		for (int j = 0; j < cto; j++) {
			rc = write(sock[i], buffer2+j, 1);
			printf("%d - ",j);
			if (rc < 0) {
				perror("CLNT: Error escribiendo");
				return -1;
			}
		}
#endif

		cto = read(sock[i], buffer, sizeof(buffer) - 1);
		if (cto < 0) {
			perror("CLNT: Error leyendo");
			return -1;
		}

		buffer[cto] = 0;
		printf("A [%s] devuelve [%s]\n", buffer2, buffer);
	}

	for (i = 0; i < N; i++)
		close(sock[i]);

	return 0;
}