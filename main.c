#include <argp.h>
#include <stdbool.h>
#include "bitsquat.h"

#define BUFFER_SIZE 80

// CLI documentation and flag handling
const char *argp_program_version = "Bitsquat v0.1";
static char doc[] = "BitSquatter outputs all valid domains different by 1 bit \
from the input URL.\n\nExample: bitsquat --verbose https://foobar.com";
static char args_doc[] = "[URL]";
static struct argp_option options[] = {
		{"verbose", 'v', 0, 0, "Display domain name and extension bitstrings", 0},
		{"extension-too", 'e', 0, 0, "Generate URL permutations for the extension too", 0},
		{0}};

// arguments contains the values of the flags and arguments passed in command line
struct arguments
{
	char *url;
	bool isVerbose;
	bool permutateExtension;
};

// function used by argp to parse flags and arguments
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch (key)
	{
	case 'v':
		arguments->isVerbose = true;
		break;
	case 'e':
		arguments->permutateExtension = true;
		break;
	case ARGP_KEY_ARG:
		(state->arg_num == 0)
				? arguments->url = arg
				: argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
		break;
	case ARGP_KEY_END:
		if (arguments->url == NULL)
		{
			argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[])
{
	struct arguments arguments;
	arguments.isVerbose = false;
	arguments.permutateExtension = false;
	arguments.url = NULL;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	const char *url = trim_protocol(arguments.url);

	char *dom = (char *)malloc(BUFFER_SIZE * sizeof(char));
	char *ext = (char *)malloc(BUFFER_SIZE * sizeof(char));

	int status = split_url(url, dom, ext);
	if (status != EXIT_SUCCESS)
	{
		fprintf(stderr, "Failed to split URL: %s into domain name and extension\n", url);
		return EXIT_FAILURE;
	}

	const int BYTE = 8;
	const size_t dom_str_length = strlen(dom);
	const size_t ext_str_length = strlen(ext);
	const size_t dom_binary_length = dom_str_length * BYTE;
	const size_t ext_binary_length = ext_str_length * BYTE;

	char dom_binary_str[dom_binary_length];
	char ext_binary_str[ext_binary_length];
	memset(dom_binary_str, '0', (int)dom_binary_length);
	memset(ext_binary_str, '0', (int)ext_binary_length);

	get_binary_string(dom_str_length, dom, dom_binary_str);
	get_binary_string(ext_str_length, ext, ext_binary_str);

	if (arguments.isVerbose)
	{
		printf("Target Domain: %s\n", arguments.url);
		printf("Domain Name: %s\tDomain extension: %s\n", dom, ext);
		printf("%s:\t%.*s\n", dom, (int)dom_binary_length, dom_binary_str);
		printf("%s:\t%.*s\n", ext, (int)ext_binary_length, ext_binary_str);
	}

	// create and fill array with bitstring to be permutates
	char dom_binary_permutations[dom_binary_length + 1][dom_binary_length + 1];
	char ext_binary_permutations[ext_binary_length + 1][ext_binary_length + 1];
	int i;
	for (i = 0; i < (int)dom_binary_length + 1; i++)
	{
		memcpy(dom_binary_permutations[i], dom_binary_str, dom_binary_length + 1);
		dom_binary_permutations[i][dom_binary_length] = '\0';
	}

	if (arguments.permutateExtension)
	{
		for (i = 0; i < (int)ext_binary_length + 1; i++)
		{
			memcpy(ext_binary_permutations[i], ext_binary_str, ext_binary_length + 1);
			ext_binary_permutations[i][ext_binary_length] = '\0';
		}
	}

	// generate 1-bit permutations
	for (i = 1; i < (int)dom_binary_length + 1; i++)
	{
		dom_binary_permutations[i][i] = (dom_binary_permutations[i][i]) == '1' ? '0' : '1';
	}

	if (arguments.permutateExtension)
	{
		for (i = 1; i < (int)ext_binary_length + 1; i++)
		{
			ext_binary_permutations[i][i] = (ext_binary_permutations[i][i]) == '1' ? '0' : '1';
		}
	}

	// transform back to string
	char dom_str_permutations[dom_binary_length + 1][dom_str_length + 1];
	char ext_str_permutations[ext_binary_length + 1][ext_str_length + 1];
	for (i = 0; i < (int)dom_binary_length + 1; i++)
	{
		get_string(dom_binary_length, dom_binary_permutations[i], dom_str_permutations[i]);
		dom_str_permutations[i][dom_str_length] = '\0';
	}

	if (arguments.permutateExtension)
	{
		for (i = 0; i < (int)ext_binary_length + 1; i++)
		{
			get_string(ext_binary_length, ext_binary_permutations[i], ext_str_permutations[i]);
			ext_str_permutations[i][ext_str_length] = '\0';
		}
	}

	// generate all possible combination of domain name & extension
	int num_url_combinations = (arguments.permutateExtension)
																 ? dom_binary_length * ext_binary_length
																 : dom_binary_length;
	char url_permutations[num_url_combinations][dom_str_length + ext_str_length + 2];

	int counter = 0;
	for (i = 0; i < (int)dom_binary_length + 1; i++)
	{
		if (arguments.permutateExtension)
		{
			int j;
			for (j = 0; j < (int)ext_binary_length + 1; j++)
			{
				memcpy(url_permutations[counter], dom_str_permutations[i], dom_str_length);
				url_permutations[counter][dom_str_length] = '.';
				memcpy(&url_permutations[counter][dom_str_length + 1], ext_str_permutations[j], ext_str_length);
				url_permutations[counter][dom_str_length + ext_str_length + 1] = '\0';

				counter++;
			}
		}
		else
		{
			memcpy(url_permutations[counter], dom_str_permutations[i], dom_str_length);
			url_permutations[counter][dom_str_length] = '.';
			memcpy(&url_permutations[counter][dom_str_length + 1], ext, ext_str_length);
			url_permutations[counter][dom_str_length + ext_str_length + 1] = '\0';

			counter++;
		}
	}

	for (i = 0; i < num_url_combinations; i++)
	{
		if (is_valid_url(url_permutations[i]))
		{
			printf("%s\n", url_permutations[i]);
		}
	}

	free(dom);
	free(ext);

	return (EXIT_SUCCESS);
}
