#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

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
		printf("offset %d: %x %x", offset, bits[offset / 8], ~(1 << (7 - (offset % 8))));
		bits[offset / 8] = bits[offset / 8] & ~(1 << (7 - (offset % 8)));
		printf(" %x\n", bits[offset / 8]);
		return (1);
	}
	if (find_get_code(bits, offset + 1, deep, root->child_1, symbol) == 1)
	{
		printf("offset %d: %x %x", offset, bits[offset / 8], (1 << (7 - (offset % 8))));
		bits[offset / 8] = bits[offset / 8] | (1 << (7 - (offset % 8)));
		printf(" %x\n", bits[offset / 8]);
		return (1);
	}
	return (0);
}

void print_code(char bits[33])
{
	for (int i = 0; i < 33; i++)
		printf("%x ", bits[i]);
}

void feel_zeros(char bits[33])
{
	for (int i = 0; i < 33; i++)
		bits[i] = 0;
}

// void get_codes(unsigned char bits, int deep, t_node *list)
// {
// 	while (list)
// 	{
// 		if (list->child_0 == NULL && list->child_1 == NULL)
// 		{
// 			// while (deep > 0)
// 			// {
// 			// 	if (bits & (1 << (int)deep))
// 			// 		printf("1");
// 			// 	else
// 			// 		printf("0");
// 			// 	deep--;
// 			// }
// 			printf("%d %d ", deep, bits);
// 			if (!isalnum(list->byte))
// 				printf(" nn ");
// 			else
// 				printf(" %c ", list->byte);

// 			printf("%d\n", list->freq);
// 		}
// 		if (list->child_0)
// 			get_code( bits << 1, deep + 1, list->child_0);
// 		if (list->child_1)
// 			get_code(bits << 1 | 1, deep + 1, list->child_1);
// 		list = list->next;
// 	}
// }

int get_frequences(const char *filename, t_node **list)
{
	int fd = open(filename, O_RDONLY);
	if (fd > 0)
	{
		int symbol;
		int readres;
		while ((readres = read(fd, &symbol, 1)) == 1)
			if (add_node(symbol, list) == -1)
				return (-1);
		if (readres == 0)
		{
			if (add_node(symbol, list) == -1)
				return (-1);
			return (0);
		}
		else
		{
			printf("%s\n", strerror(errno));
			return (-1);
		}
	}
	printf("%s\n", strerror(errno));
	return (-1);
}

void append_to_sorted(t_node *node, t_node **list)
{
	t_node *prev = NULL;
	t_node *cur = *list;
	while (cur && node->freq < cur->freq)
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

int main(int argc, char **argv)
{
	t_node *list = NULL;
	if (argc == 3)
	{
		if (get_frequences(argv[1], &list) == -1)
			return (2);
		if (make_tree(&list) == -1)
			return (2);
		print_tree(0, list);
		// get_codes(0, 0, list);
		char bits[33];
		feel_zeros(bits);
		int deep;
		find_get_code(bits, 0, &deep, list, 'a');
		print_code(bits);
		return (0);
	}
	else if (argc == 4)
	{
		// t_node *root;
		// if ((root = read_key_tree(argv[3])) == NULL)
		// 	return (2);
		// if (decode(root, argv[1], argv[2]) == -1)
		// 	return (2);
		// free_node(root);
		return (0);
	}
	else
	{
		printf("./exe infile outfile [key_tree]\n");
		return (1);
	}
}
