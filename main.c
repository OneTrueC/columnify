/* a very simple program for column formatting monospace text files
 * this code is unlicensed and released to the public domain */
#define _POSIX_C_SOURCE 200809L

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>

#include <unistd.h>

char* shortopts = ":hl:n:o:p:s:vw:";

void die(int ecode, const char* message, ...);
void printPage(wchar_t* text, wchar_t* pageSep, wchar_t* colSep, int width,
               size_t length, int nCols, FILE* out);
void usage(void);
void version(void);

int
main(int argc, char** argv)
{
	char opt, *end, *linePtr;
	int i, width, column, length, nCols;
	size_t n, wn, len, temp;
	wchar_t *text, *widePtr, *pageSep, *colSep, *wTemp;
	FILE *in, *out;

	in = stdin;
	out = stdout;
	linePtr = NULL;
	widePtr = NULL;
	n = wn = 0;
	width = length = nCols = 0;
	pageSep = colSep = L"";

	setlocale(LC_ALL, "");

	while ((opt = getopt(argc, argv, shortopts)) != -1) {
		switch (opt) {
		case 'h':
			usage();
			exit(0);
		case 'v':
			version();
			exit(0);
		case 'w':
		case 'n':
		case 'l':
			errno = 0;
			temp = strtoul(optarg, &end, 10);

			if (errno == ERANGE || temp > INTMAX_MAX)
				die(1, "argument %c too large", opt);

			if (end == NULL || temp == 0)
				die(11, "argument %c is not a positive integer", opt);

			switch (opt) {
			case 'w':
				width = temp;
				break;
			case 'n':
				nCols = temp;
				break;
			case 'l':
				length = temp;
				break;
			}
			break;
		case 'p':
		case 's':
			len = strlen(optarg) + 1;
			wTemp = malloc(sizeof(wchar_t) * len);
			len = mbstowcs(wTemp, optarg, len);

			if (len == (size_t)-1)
				die(8, "invalid character encoding in option %c", opt);

			switch (opt) {
			case 's':
				colSep = wTemp;
				break;
			case 'p':
				pageSep = wTemp;
				break;
			}
			break;
		case 'o':
			out = fopen(optarg, "w");
			if (out == NULL)
				die(5, "could not open output file: %s", strerror(errno));
			break;
		case '?':
			usage();
			die(9, "invalid option -- '%c'", optopt);
			break;
		case ':':
			usage();
			die(10, "option '%c' requires an argument", optopt);
			break;
		default:
			usage();
			exit(0);
		}
	}

	if (argc != optind) {
		in = fopen(argv[optind++], "r");
		if (out == NULL)
			die(6, "could not open input file: %s", strerror(errno));
		if (argv[optind] != NULL)
			die(7, "too many options given");
	}

	if (!(width && length && nCols)) {
		usage();
		die(2, "width, length, and number of columns MUST be defined");
	}

	text = malloc(sizeof(wchar_t) * width * length * nCols);
	wmemset(text, L' ', width * length * nCols);

	i = column = 0;

	while (getline(&linePtr, &n, in) != -1) {
		if (wn < n + 1) {
			wn = n + 1;
			widePtr = realloc(widePtr, wn * sizeof(wchar_t));
		}

		len = mbstowcs(widePtr, linePtr, strlen(linePtr) + 1);

		if (len == (size_t)-1)
			die(3, "invalid character: this likely means that input is encoded "
			       "differently than your selected locale");
		/* decrement len to remove newline character */
		else if (--len > (size_t)width)
			die(4, "line was longer than specified text width");

		wmemcpy(&text[i * width], widePtr, len);

		i += nCols;

		if (i >= length * nCols) {
			column++;

			if (column == nCols) {
				printPage(text, pageSep, colSep, width, length, nCols, out);
				wmemset(text, L' ', width * length * nCols);
				i = column = 0;
			} else
				i = column;
		}
	}

	if (i != 0)
		printPage(text, L"", colSep, width, length, nCols, out);

	if (in != stdin)
		fclose(in);

	if (out != stdout)
		fclose(out);

	free(text);
	free(linePtr);
	free(widePtr);
	if (pageSep[0] != L'\0')
		free(pageSep);
	if (colSep[0] != L'\0')
		free(colSep);

	return 0;
}

void
die(int ecode, const char* message, ...)
{
	va_list ap;

	fprintf(stderr, "columnify: ");
	va_start(ap, message);
	vfprintf(stderr, message, ap);
	va_end(ap);
	fputs("\n", stderr);

	exit(ecode);
}

void
printPage(wchar_t* text, wchar_t* pageSep, wchar_t* colSep, int width,
          size_t length, int nCols, FILE* out)
{
	size_t i;
	wchar_t buf[width + 1];
	int j;

	buf[width] = L'\0';

	for (i = 0; i < length * nCols; i += nCols)
		for (j = 0; j < nCols; j++) {
			wmemcpy(buf, &text[(i + j) * width], width);
			fputws(buf, out);
			fputws(j == (nCols - 1) ? L"\n" : colSep, out);
		}

	fwprintf(out, L"%ls", pageSep);
}

void
usage(void)
{
	puts("Usage: columnify [-hv] -n COLUMNS -w WIDTH -l PAGE_LENGTH "
	     "[-sp SEPARATOR] [-o OUTPUT] [INPUT]\n"
	     "\n"
	     "  -h display this help and exit\n"
	     "  -v display version info and exit\n"
	     "  -n set the number of columns to put on one page\n"
	     "  -l set the vertical length of a page\n"
	     "  -w set text/column width\n"
	     "  -s set the column separator (if one should be used)\n"
	     "  -p set the page separator (same as above)\n"
	     "  -o output file\n");
}

void
version(void)
{
	puts("columnify (v"VERSION")\n"
	     "\n"
	     "written by Drew Marino");
}
