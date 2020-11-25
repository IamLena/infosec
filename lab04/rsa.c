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

// calculates a * *x + b * *y = gcd(a, b) = *d
void extended_euclid(int a, int b, int *x, int *y, int *d)
{
	int q, r, x1, x2, y1, y2;
	if (b == 0) {
		*d = a, *x = 1, *y = 0;
		return;
	}
	x2 = 1, x1 = 0, y2 = 0, y1 = 1;
	while (b > 0) {
		q = a / b;
		r = a - q * b;
		*x = x2 - q * x1;
		*y = y2 - q * y1;
		a = b;
		b = r;
		x2 = x1, x1 = *x;
		y2 = y1, y1 = *y;
	}
	*d = a;
	*x = x2;
	*y = y2;
}

int get_secret_key(int E, int Fi)
{
	int D, k, d;
	extended_euclid(E, Fi, &D, &k, &d);
	if (D < 0)
		D += Fi;
	return (D);
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
	int fdin, fdout;

	if (argc == 2)
	{
		get_prime(&p, &q);
		if (p == -1 || q == -1)
			return (0);
		N = p * q;
		Fi = (p - 1) * (q - 1);
		E = get_coprime(Fi);
		D = get_secret_key(E, Fi);

		fdin = open(argv[1], O_RDONLY);
		fdout = open("incrypt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
		crypt(E, N, fdin, fdout);
		close(fdin);
		close(fdout);

		fdin = open("incrypt", O_RDONLY);
		fdout = open("decrypt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
		decrypt(D, N, fdin, fdout);
		close(fdin);
		close(fdout);
	}
	else
		printf("./prog msgfile\n");
}
