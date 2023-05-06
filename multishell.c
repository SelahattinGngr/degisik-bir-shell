#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int
	main(int argc, char *argv[])
{
	int	n;
	int	i;
	int	j;

	j = -1;
	i = -1;
	if (argc != 2)
	{
		printf("Kullanım: %s <terminal sayısı>\n", argv[0]);
		return (1);
	}
	n = atoi(argv[1]);
	while (++i < n)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			printf("Fork hatası!\n");
			return (1);
        }
		else if (pid == 0)
		{
			char *args[] = {"xterm", "-e", "./singleshell", NULL};
            if (execvp(args[0], args) == -1)
			{
                printf("Exec hatası!\n");
                return (1);
			}
		}
	}
	while (++j < n)
		wait(NULL);
	return (0);
}

