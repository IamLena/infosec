#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

void	get_prime(int *p, int *q)
{
	*p = 17;
	*q = 31;
}

int		get_coprime(int Fi)
{
	return (77);
}

int get_secret_key(int E, int Fi)
{
	return (773);
}

unsigned long int powmod(unsigned long int a, int k, int n)
{
	unsigned long int res = 1;
	while (k > 0)
	{
		if (k & 1)
			res = (res * a ) % n;
		a = (a * a) % n;
		k = k >> 1;
	}
	return (res);
}

void crypt(int E, unsigned long int N, int fdin, int fdout)
{
	unsigned char byte;
	int readres;
	unsigned long int byteres;

	while (readres = read(fdin, &byte, 1))
	{
		byteres = powmod(byte, E, N);		//(byte ** E) mod N
		write(fdout, &byteres, 8);
	}
	if (readres == -1)
	{
		printf("error ocurred while reading\n");
		return;
	}
}

void decrypt(int D, unsigned long int N, int fdin, int fdout)
{
	unsigned long int	byte;
	unsigned char	byteres;
	int readres;

	while (readres = read(fdin, &byte, 8))
	{
		byteres = powmod(byte, D, N);		//(byte ** D) mod N
		write(fdout, &byteres, 1);
	}
	if (readres == -1)
	{
		printf("error ocurred while reading\n");
		return;
	}
}

int main(int argc, char **argv)
{
	int			p;
	int			q;
	unsigned long int	N;
	int			Fi;
	int			E;
	int			D;

	if (argc == 4)
	{
		int fdin = open(argv[1], O_RDONLY);
		if (fdin == -1)
		{
			printf("cant open msgfile\n");
			return (0);
		}
		int fdout = open(argv[2], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
		if (fdout == -1)
		{
			printf("cant open resfile\n");
			close(fdin);
			return (0);
		}

		get_prime(&p, &q);
		N = p * q;
		Fi = (p - 1) * (q - 1);
		E = get_coprime(Fi);
		D = get_secret_key(E, Fi);

		if (argv[3][0] == 'c')
			crypt(E, N, fdin, fdout);
		else if (argv[3][0] == 'd')
			decrypt(D, N, fdin, fdout);

		close(fdin);
		close(fdout);
	}
	else
		printf("./prog msgfile resfile c/d\n");
}
