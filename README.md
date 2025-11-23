columnify
=========

A utility to render monospace text into parallel columns.

This was born out of a curiosity to see how few pages I could fit a
book onto while still keeping it readable, the original printing was
about 150ish pages, and I got it down to 13.

This program can also be replaced (if only two columns are desired)
by the following vim macro (I originally used the macro):

	100j0\<C-V>99j$x100k73lhp100j0V99jc\<C-V>\<C-P>\<Esc>j0

Where:

	100 = lines per page
	99 = lines per page - 1
	73 = width of column + spacing
	virtualedit is set to all

Usage
-----

```
Usage: columnify [-hv] -n COLUMNS -w WIDTH -l PAGE_LENGTH [-sp SEPARATOR] [-o OUTPUT] [INPUT]

  -h display this help and exit
  -v display version info and exit
  -n set the number of columns to put on one page
  -l set the vertical length of a page
  -w set text/column width
  -s set the column separator (if one should be used)
  -p set the page separator (same as above)
  -o output file
```

For more info see the manpage.
