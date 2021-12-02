#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int ft_strlen(char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return i;
}

void per(char *s)
{
	write(2, s, ft_strlen(s));
}

void ex(char *s)
{
	per(s);
	exit(1);
}

int main(int ac, char **av, char **env)
{
	int pfd[2];
	int pid;
	int pip = 0;
	int lastfd = -1;
	int nbarg = 0;
	int i = 1;

	if(ac == 1)
		return 1;
	
	while (i <= ac)
	{
		if (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			nbarg++;
		else if (nbarg)
		{
			if (av[i])
				pip = (strcmp(av[i], "|") == 0 ? 1 : 0);
			av[i] = NULL;
			if (!strcmp(av[i - nbarg], "cd"))
			{
				if (nbarg == 2)
				{
					if ((chdir(av[i - nbarg + 1]) == -1))
					{
						per("error: cd: cannot change directory to ");
						per(av[i - nbarg + 1]);
						per("\n");
					}
				}
				else
					per("error: cd: bad arguments\n");
			}
			else
			{
				if(pip)
				{
					if(pipe(pfd) == -1)
						ex("error: fatal\n");
				}
				if ((pid = fork()) == -1)
					ex("error: fatal\n");
				if(!pid)
				{
					if(lastfd != -1 && (dup2(lastfd, 0) == -1))
						ex("error: fatal\n");
					if(pip && (dup2(pfd[1], 1) == -1))
						ex("error: fatal\n");
					if(execve(av[i - nbarg], &av[i - nbarg], env) == -1)
					{
						per("error: cannot execute ");
						per(av[i - nbarg]);
						ex("\n");
					}
				}
				else
				{
					waitpid(-1, 0, 0);
					close(pfd[1]);
					if(lastfd != -1)
						close(lastfd);
					if(pip)
						lastfd = pfd[0];
					else
						lastfd = -1;
				}
			}
			nbarg = 0;
		}
		pip = 0;
		i++;
	}
}