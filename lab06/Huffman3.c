#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>

typedef struct s_node
{
	unsigned char byte;
	int freq;
	struct s_node *child_0;
	struct s_node *child_1;
	struct s_node *next;
} t_node;

void free_node(t_node *node)
{
	if (node->child_0 == NULL)
	{
		free(node);
		return;
	}
	if (node->child_1 == NULL)
	{
		free(node);
		return;
	}
	free_node(node->child_0);
	free_node(node->child_1);
}

void free_list(t_node *list)
{
	t_node *next;
	while(list)
	{
		next = list->next;
		free_node(list);
		list = next;
	}
}

int add_node(unsigned char byte, t_node **list)
{
	t_node *new;
	t_node *cur;

	if (*list == NULL)
	{
		new = malloc(sizeof(t_node));
		if (!new)
			return (-1);
		new->byte = byte;
		new->freq = 1;
		new->child_0 = NULL;
		new->child_1 = NULL;
		new->next = NULL;
		*list = new;
		return (0);
	}
	cur = *list;
	while (cur->next != NULL && cur->byte != byte)
		cur = cur->next;
	if (cur->byte == byte)
	{
		cur->freq++;
		return (0);
	}
	new = malloc(sizeof(t_node));
	if (!new)
	{
		free_list(*list);
		return (-1);
	}
	new->byte = byte;
	new->freq = 1;
	new->child_0 = NULL;
	new->child_1 = NULL;
	new->next = NULL;
	cur->next = new;
	return (0);
}

void print_tree(int tabs, t_node *list)
{
	while (list)
	{
		if (list->child_0 == NULL && list->child_1 == NULL)
		{
			if (list->byte == '\n')
				printf("\\n ");
			else
				printf("%c ", list->byte);
		}
		printf("%d\n", list->freq);
		if (list->child_0)
		{
			for (int i = 0; i <= tabs; i++)
				printf("\t");
			print_tree(tabs + 1, list->child_0);
		}
		if (list->child_1)
		{
			for (int i = 0; i <= tabs; i++)
				printf("\t");
			print_tree(tabs + 1, list->child_1);
		}
		list = list->next;
	}
}

int find_get_code(char bits[33], int offset, int *deep, t_node *root, unsigned char symbol)
{
	if (!root)
		return (0);
	if (!root->child_0 && !root->child_1 && root->byte == symbol)
	{
		*deep = offset;
		return (1);
	}
	if (find_get_code(bits, offset + 1, deep, root->child_0, symbol) == 1)
	{
		bits[offset / 8] = bits[offset / 8] & ~(1 << (7 - (offset % 8)));
		return (1);
	}
	if (find_get_code(bits, offset + 1, deep, root->child_1, symbol) == 1)
	{
		bits[offset / 8] = bits[offset / 8] | (1 << (7 - (offset % 8)));
		return (1);
	}
	return (0);
}

void shiftbyte(char bits[33])
{
	for (int i = 0; i < 32; i++)
	{
		bits[i] = bits[i + 1];
	}
	bits[32] = 0;
}

void print_code(char bits[33])
{
	for (int i = 0; i < 33; i++)
		printf("%x ", bits[i]);
	printf("\n");
}

void feel_zeros(char bits[33])
{
	for (int i = 0; i < 33; i++)
		bits[i] = 0;
}

int get_frequences(const char *filename, t_node **list)
{
	int fd = open(filename, O_RDONLY);
	if (fd > 0)
	{
		unsigned char symbol;
		int readres;
		while ((readres = read(fd, &symbol, 1)) == 1)
			if (add_node(symbol, list) == -1)
			{
				close(fd);
				return (-1);
			}
		// if (readres == 0)
		// {
		// 	if (add_node(symbol, list) == -1)
		// 	{
		// 		close(fd);
		// 		return (-1);
		// 	}
		// 	close(fd);
		// 	return (0);
		// }
		if (readres == -1)
		{
			printf("%s\n", strerror(errno));
			{
				close(fd);
				return (-1);
			}
		}
		return (0);
	}
	printf("%s\n", strerror(errno));
	return (-1);
}

void append_to_sorted(t_node *node, t_node **list)
{
	t_node *prev = NULL;
	t_node *cur = *list;
	while (cur && node->freq <= cur->freq && ((node->freq != cur->freq) || (node->byte < cur->byte)))
	{
		prev = cur;
		cur = cur->next;
	}
	node->next = cur;
	if (prev == NULL)
		*list = node;
	else
		prev->next = node;
}

void sort(t_node **list)
{
	t_node *cur = *list;
	t_node *tmp;
	t_node *sorted_list = NULL;
	while(cur)
	{
		tmp = cur->next;
		append_to_sorted(cur, &sorted_list);
		cur = tmp;
	}
	*list = sorted_list;
}

int form_branch(t_node **list)
{
	t_node *cur = *list;
	t_node *prev = NULL;

	if (!cur || !cur->next)
		return (-1);
	while (cur->next->next != NULL)
	{
		prev = cur;
		cur = cur->next;
	}
	t_node *new_node = malloc(sizeof(t_node));
	if (!new_node)
		return (-1);
	new_node->next = NULL;
	new_node->freq = cur->freq + cur->next->freq;
	new_node->child_0 = cur->next;
	new_node->child_1 = cur;
	new_node->child_0->next = NULL;
	new_node->child_1->next = NULL;
	if (prev == NULL)
		*list = new_node;
	else
		prev->next = new_node;
	return (0);
}

int make_tree(t_node **list)
{
	while ((*list)->next != NULL)
	{
		sort(list);
		if (form_branch(list) == -1)
		{
			free_list(*list);
			return (-1);
		}
	}
	return (0);
}

void put_tree_to_file(int fd, t_node *list)
{
	if (list)
	{
		if (list->child_0 == NULL && list->child_1 == NULL)
		{
			write(fd, &(list->byte), 1);
			write(fd, &(list->freq), 4);
		}
		if (list->child_0)
			put_tree_to_file(fd, list->child_0);
		if (list->child_1)
			put_tree_to_file(fd, list->child_1);
	}
}

int read_tree(int fd, t_node **list)
{
	int size;
	unsigned char symbol;
	int freq;

	int readres1;
	int readres2;

	t_node *cur = *list;
	t_node *new;

	if (read(fd, &size, 4) != 4)
		return (-1);
	while (size > 0 && (readres1 = read(fd, &symbol, 1)) == 1 && (readres2 = read(fd, &freq, 4)) == 4) // read all amount of symbols to file, and check for the end, then read data
	{
		new = malloc(sizeof(t_node));
		if (!new)
		{
			free_list(*list);
			return (-1);
		}
		new->byte = symbol;
		new->freq = freq;
		new->child_0 = NULL;
		new->child_1 = NULL;
		new->next = NULL;
		if (cur == NULL)
			*list = new;
		else
			cur->next = new;
		cur = new;
		size -= new->freq;
	}
	if (readres1 == -1 || readres1 == 0 || readres2 == -1)
	{
		free_list(*list);
		return (-1);
	}
	if (readres2 == 0)
	{
		new = malloc(sizeof(t_node));
		if (!new)
		{
			free_list(*list);
			return (-1);
		}
		new->byte = symbol;
		new->freq = freq;
		new->child_0 = NULL;
		new->child_1 = NULL;
		new->next = NULL;
		if (cur == NULL)
			*list = new;
		else
			cur->next = new;
		cur = new;
	}
	return (0);
}

int code(const char *filein, const char *fileout, t_node *root)
{
	int fdin = open(filein, O_RDONLY);
	if (fdin == -1)
		return (-1);
	int fdout = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fdout == -1)
	{
		close(fdin);
		return (-1);
	}
	write(fdout, &(root->freq), 4);
	put_tree_to_file(fdout, root);
	// print_tree(0, root);
	unsigned char symbol;
	int offset = 0;
	char bits[33];
	feel_zeros(bits);
	int rc;

	while ((rc = read(fdin, &symbol, 1)) > 0)
	{
		find_get_code(bits, offset, &offset, root, symbol);
		if (offset >= 8)
		{
			if (write(fdout, bits, 1) == -1)
			{
				close(fdin);
				close(fdout);
				return (-1);
			}
			shiftbyte(bits);
			offset -= 8;
		}
	}
	if (rc == -1)
	{
		close(fdin);
		close(fdout);
		return (-1);
	}
	find_get_code(bits, offset, &offset, root, symbol);
	if (offset % 8)
		rc = write(fdout, bits, offset / 8 + 1);
	else
		rc = write(fdout, bits, offset / 8);
	close(fdin);
	close(fdout);
	return (rc > 0 ? 0 : -1);
}

unsigned char findchar(char bits[33], t_node *root, int deep, int *offset)
{
	if (root->child_0 == NULL && root->child_1 == NULL)
	{
		*offset = deep;
		return root->byte;
	}
	if (bits[deep / 8] & (1 << (7 - deep % 8)))
		return (findchar(bits, root->child_1, deep + 1, offset));
	else
		return (findchar(bits, root->child_0, deep + 1, offset));
}

int decode(int fdin, const char *fileout, t_node *root)
{
	int fdout = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fdout == -1)
	{
		close(fdin);
		return (-1);
	}
	char bits[33];
	int ends;
	int offset = 0;
	unsigned char symbol;
	ends = read(fdin, bits, 33) * 8;
	while (offset < ends && ends >= 0 && root->freq > 0)
	{
		symbol = findchar(bits, root, offset, &offset);
		write(fdout, &symbol, 1);
		root->freq--;
		if (offset >= 8)
		{
			shiftbyte(bits);
			offset -= 8;
			ends -= 8;
			if (read(fdin, bits + 32, 1) == 1)
				ends += 8;
		}
	}
}

int main(int argc, char **argv)
{
	t_node *list = NULL;
	if (argc == 4 && argv[3][0] == 'c')
	{
		if (get_frequences(argv[1], &list) == -1)
			return (2);
		if (make_tree(&list) == -1)
			return (2);
		int rc = code(argv[1], argv[2], list);
		free_list(list);
		return (rc);
	}
	else if (argc == 4 && argv[3][0] == 'd')
	{
		int fdin = open(argv[1], O_RDONLY);
		if (fdin == -1)
			return (2);
		if (read_tree(fdin, &list) == -1)
			return (2);
		if (make_tree(&list) == -1)
			return (2);
		decode(fdin, argv[2], list);
		// if (decode(root, argv[1], argv[2]) == -1)
		// 	return (2);
		free_node(list);
		return (0);
	}
	else
	{
		printf("./exe infile outfile c/d\n");
		return (1);
	}
}
