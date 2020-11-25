#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PRIME_MAX 50

void	get_prime(int *p, int *q)
{
	*q = -1;
	*p = -1;
	//[2, 3, 4, 5, 6, ...]
	//index = number - 2
	unsigned char array[PRIME_MAX];
	for (int i = 0; i < PRIME_MAX; i++)
		array[i] = 0;
	for (int i = 2; i < PRIME_MAX + 2; i++)
	{
		if (array[i - 2] == 0)
		{
			for (int j = i + i; j < PRIME_MAX + 2; j += i)
				array[j - 2] = 1;
		}
	}
	for (int i = PRIME_MAX - 1; i >= 0; i--)
	{
		if (array[i] == 0)
		{
			*p = i + 2;
			break;
		}
	}
	for (int i = *p - 3; i >= 0; i--)
	{
		if (array[i] == 0)
		{
			*q = i + 2;
			break;
		}
	}
}

int		get_coprime(int Fi)
{
	int a, b, E;

	time_t t;
	srand((unsigned) time(&t));

	a = 1;
	b = 1;
	while (a + b != 1) // while E is not prime
	{
		E = rand();
		a = E;
		b = Fi;
		while (a != 0 && b != 0)
		{
			if (a > b)
				a = a % b;
			else
				b = b % a;
		}
	}
	return (E);
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
		if (p == -1 || q == -1)
		{
			printf("error in get_prime\n");
			close(fdin);
			close(fdout);
			return (0);
		}
		N = p * q;
		Fi = (p - 1) * (q - 1);
		E = get_coprime(Fi);
		printf("p = %d\tq=%d\tFi=%d\tE=%d\tN=%ld\n", p, q, Fi, E, N);
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
