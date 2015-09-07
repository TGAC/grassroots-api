/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20150711

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 1

#line 2 "css_syntax.y"
#include <stdio.h>
#include <string.h>
#include "css_lex.h"
#include "parser.h"

#define YYPARSE_PARAM yyparam
#define YYERROR_VERBOSE 1
/*#define YYDEBUG 1*/

#line 15 "css_syntax.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	char *lexeme;
	char letter;
	struct property_t *property;
	struct selector_t *selector;
	struct selector_list_t *selector_list;
	int pseudo_class;
	int pseudo_element;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 48 "css_syntax.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# ifdef YYLEX_PARAM_TYPE
#  define YYLEX_DECL() yylex(YYSTYPE *yylval, YYLEX_PARAM_TYPE YYLEX_PARAM)
# else
#  define YYLEX_DECL() yylex(YYSTYPE *yylval, void * YYLEX_PARAM)
# endif
# define YYLEX yylex(&yylval, YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(YYSTYPE *yylval)
# define YYLEX yylex(&yylval)
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define IMPORT_SYM 257
#define IMPORTANT_SYM 258
#define IDENT 259
#define STRING 260
#define NUMBER 261
#define PERCENTAGE 262
#define LENGTH 263
#define EMS 264
#define EXS 265
#define LINK_PSCLASS_AFTER_IDENT 266
#define VISITED_PSCLASS_AFTER_IDENT 267
#define ACTIVE_PSCLASS_AFTER_IDENT 268
#define FIRST_LINE_AFTER_IDENT 269
#define FIRST_LETTER_AFTER_IDENT 270
#define HASH_AFTER_IDENT 271
#define CLASS_AFTER_IDENT 272
#define LINK_PSCLASS 273
#define VISITED_PSCLASS 274
#define ACTIVE_PSCLASS 275
#define FIRST_LINE 276
#define FIRST_LETTER 277
#define HASH 278
#define CLASS 279
#define URL 280
#define RGB 281
#define CDO 282
#define CDC 283
#define CSL 284
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    8,    8,   27,   27,   26,   26,   29,   29,   29,
   28,   25,   25,   11,   11,   12,   12,   12,   20,    7,
    6,    6,   14,   14,    3,    3,    3,    3,    3,    3,
    5,    5,    4,    4,    4,    4,    4,    4,    4,    4,
    4,    4,    4,    4,    4,    4,    4,   22,    2,    2,
    2,    1,    1,    1,   23,   24,    9,    9,   10,   10,
   17,   18,   13,   13,   13,   13,   30,   21,   21,   21,
   19,   19,   16,   16,   16,   16,   16,   16,   16,   16,
   16,   16,   15,   15,
};
static const YYINT yylen[] = {                            2,
    3,    3,    0,    3,    0,    2,    0,    1,    1,    1,
    3,    1,    1,    1,    1,    1,    1,    0,    1,    4,
    1,    3,    1,    3,    2,    2,    1,    1,    2,    1,
    1,    2,    4,    3,    3,    3,    2,    2,    2,    1,
    3,    2,    2,    1,    2,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    4,    3,    1,    0,    1,    1,    3,    2,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,
};
static const YYINT yydefred[] = {                         7,
    0,    0,    0,    8,    9,   10,    0,    7,    6,   12,
   13,    0,   30,   48,   52,   53,   54,   60,   59,   62,
   56,   47,    0,    0,    0,    0,    7,    1,   28,    0,
    0,    0,    0,   11,   29,    0,   32,   58,   57,   25,
   26,    0,    0,   49,   50,   51,   55,   43,    0,   61,
   39,    0,    0,   45,    4,   22,   65,   19,    0,    0,
    0,    2,   41,   34,    0,   35,    0,   20,    0,   33,
   24,   79,   74,   73,   75,   76,   77,   78,   84,   83,
   81,   82,   14,   15,    0,   80,   72,   68,    0,   71,
   70,   67,   16,   17,    0,   63,   69,
};
static const YYINT yydgoto[] = {                          1,
   22,   48,   23,   24,   25,   26,   27,   28,   40,   29,
   85,   95,   59,   60,   86,   87,   52,   30,   88,   61,
   89,   31,   49,   32,   12,    2,    7,    8,    9,   96,
};
static const YYINT yysindex[] = {                         0,
    0, -251, -245,    0,    0,    0, -217,    0,    0,    0,
    0,  -54,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -41, -207, -180, -114,    0,    0,    0, -160,
 -168, -203, -251,    0,    0, -217,    0,    0,    0,    0,
    0, -238, -229,    0,    0,    0,    0,    0, -203,    0,
    0, -160, -203,    0,    0,    0,    0,    0,  -49, -106,
  -38,    0,    0,    0, -203,    0, -238,    0,  -19,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -186,    0,    0,    0,  -33,    0,
    0,    0,    0,    0,  -19,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    1,    0,    0,    0,    0,   23,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -86,  -22,  -32,    0,    0,    0,    0,   -6,
   25,   37,    1,    0,    0,    0,    0,    0,    0,    0,
    0,  -55,   23,    0,    0,    0,    0,    0,   49,    0,
    0,   61,   85,    0,    0,    0,    0,    0,  -85,    0,
    0,    0,    0,    0,  109,    0,  -55,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -43,    0,
    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT yygindex[] = {                         0,
    0,  -24,    0,    0,   19,   15,    0,   40,    0,   55,
    0,    0,    0,   17,    0,    2,    0,    0,   -9,    0,
    0,    0,  -18,    0,    0,    9,   69,    0,    0,    0,
};
#define YYTABLESIZE 388
static const YYINT yytable[] = {                         18,
    5,   18,   36,   66,   34,    3,   51,   54,   42,   67,
   94,   27,   53,   93,   10,   64,   33,   57,   68,   69,
   58,   31,    3,   84,   63,   83,   13,   64,   66,   14,
    4,    5,    6,   65,   11,   43,   21,   44,   13,   23,
   70,   14,   37,   15,   16,   17,   18,   19,   20,   21,
   56,   14,    4,    5,    6,   15,   16,   17,   18,   19,
   20,   21,   44,   45,   46,   15,   16,   17,   40,   66,
   20,   21,   72,   73,   74,   75,   76,   77,   78,   41,
   46,   64,   62,   71,   79,   97,   90,    0,   38,   39,
   27,   80,   42,   81,   82,   18,   19,   44,   45,   46,
   31,   55,   50,   47,   37,   44,   45,   46,    0,    0,
    0,   47,    0,    0,    0,    0,   44,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   38,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   40,    0,    0,
    0,    0,   36,    0,    0,    0,    0,    0,    0,   46,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   42,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   37,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   38,    0,    0,
    0,    0,    0,    0,   35,   18,   18,   18,   18,   18,
   18,   18,   91,   27,   92,    0,    0,   18,    0,    0,
    0,   36,    0,   31,   18,    0,   18,   18,    0,   72,
   73,   74,   75,   76,   77,   78,   31,   31,    0,   44,
    0,   79,   44,   31,   31,    0,    5,    0,   80,    5,
   81,   82,   44,   44,    0,    0,   44,   44,   44,   44,
   44,   44,   44,    5,    5,    5,    5,    5,    5,    5,
   40,    0,    0,   40,    0,    0,    0,    0,    0,    0,
    0,    0,   46,   40,   40,   46,    0,   40,   40,   40,
   40,   40,   40,   40,   42,   46,   46,   42,    0,   46,
   46,   46,   46,   46,   46,   46,   37,   42,   42,   37,
    0,   42,   42,   42,   42,   42,   42,   42,    0,   37,
   37,    0,    0,   37,   37,   37,   37,   37,   37,   37,
   38,    0,    0,   38,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   38,   38,    0,    0,   38,   38,   38,
   38,   38,   38,   38,   36,    0,    0,   36,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   36,   36,    0,
    0,   36,   36,   36,   36,   36,   36,   36,
};
static const YYINT yycheck[] = {                         43,
    0,   45,   44,   59,   59,  257,   31,   32,  123,   59,
   44,   44,   31,   47,  260,   59,    8,  256,  125,   58,
  259,   44,    0,   43,   49,   45,  256,   52,   53,  259,
  282,  283,  284,   52,  280,   27,  123,   44,  256,  125,
   65,  259,   24,  273,  274,  275,  276,  277,  278,  279,
   36,  259,  282,  283,  284,  273,  274,  275,  276,  277,
  278,  279,  266,  267,  268,  273,  274,  275,   44,  125,
  278,  279,  259,  260,  261,  262,  263,  264,  265,   25,
   44,  125,   43,   67,  271,   95,   85,   -1,  269,  270,
  123,  278,   44,  280,  281,  276,  277,  266,  267,  268,
  123,   33,  271,  272,   44,  266,  267,  268,   -1,   -1,
   -1,  272,   -1,   -1,   -1,   -1,  123,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   44,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,   -1,
   -1,   -1,   44,   -1,   -1,   -1,   -1,   -1,   -1,  123,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  123,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  123,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,   -1,
   -1,   -1,   -1,   -1,  256,  259,  260,  261,  262,  263,
  264,  265,  256,  256,  258,   -1,   -1,  271,   -1,   -1,
   -1,  123,   -1,  256,  278,   -1,  280,  281,   -1,  259,
  260,  261,  262,  263,  264,  265,  269,  270,   -1,  256,
   -1,  271,  259,  276,  277,   -1,  256,   -1,  278,  259,
  280,  281,  269,  270,   -1,   -1,  273,  274,  275,  276,
  277,  278,  279,  273,  274,  275,  276,  277,  278,  279,
  256,   -1,   -1,  259,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  256,  269,  270,  259,   -1,  273,  274,  275,
  276,  277,  278,  279,  256,  269,  270,  259,   -1,  273,
  274,  275,  276,  277,  278,  279,  256,  269,  270,  259,
   -1,  273,  274,  275,  276,  277,  278,  279,   -1,  269,
  270,   -1,   -1,  273,  274,  275,  276,  277,  278,  279,
  256,   -1,   -1,  259,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  269,  270,   -1,   -1,  273,  274,  275,
  276,  277,  278,  279,  256,   -1,   -1,  259,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  269,  270,   -1,
   -1,  273,  274,  275,  276,  277,  278,  279,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 284
#define YYUNDFTOKEN 317
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,"'+'","','","'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,"':'","';'",0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"IMPORT_SYM","IMPORTANT_SYM","IDENT","STRING","NUMBER","PERCENTAGE","LENGTH",
"EMS","EXS","LINK_PSCLASS_AFTER_IDENT","VISITED_PSCLASS_AFTER_IDENT",
"ACTIVE_PSCLASS_AFTER_IDENT","FIRST_LINE_AFTER_IDENT",
"FIRST_LETTER_AFTER_IDENT","HASH_AFTER_IDENT","CLASS_AFTER_IDENT",
"LINK_PSCLASS","VISITED_PSCLASS","ACTIVE_PSCLASS","FIRST_LINE","FIRST_LETTER",
"HASH","CLASS","URL","RGB","CDO","CDC","CSL",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : stylesheet",
"stylesheet : comments imports rulesets",
"rulesets : ruleset comments rulesets",
"rulesets :",
"imports : import comments imports",
"imports :",
"comments : comments comment",
"comments :",
"comment : CDO",
"comment : CDC",
"comment : CSL",
"import : IMPORT_SYM string_or_url ';'",
"string_or_url : STRING",
"string_or_url : URL",
"unary_operator : '-'",
"unary_operator : '+'",
"operator : '/'",
"operator : ','",
"operator :",
"property : IDENT",
"ruleset : selectors '{' declarations '}'",
"selectors : selector",
"selectors : selector ',' selectors",
"declarations : declaration",
"declarations : declaration ';' declarations",
"selector : simple_selectors pseudo_element",
"selector : simple_selectors solitary_pseudo_element",
"selector : simple_selectors",
"selector : solitary_pseudo_element",
"selector : selector error",
"selector : error",
"simple_selectors : simple_selector",
"simple_selectors : simple_selector simple_selectors",
"simple_selector : element_name id class pseudo_class",
"simple_selector : element_name id pseudo_class",
"simple_selector : element_name class pseudo_class",
"simple_selector : element_name id class",
"simple_selector : element_name id",
"simple_selector : element_name class",
"simple_selector : element_name pseudo_class",
"simple_selector : element_name",
"simple_selector : solitary_id class pseudo_class",
"simple_selector : solitary_id class",
"simple_selector : solitary_id pseudo_class",
"simple_selector : solitary_id",
"simple_selector : solitary_class pseudo_class",
"simple_selector : solitary_class",
"simple_selector : solitary_pseudo_class",
"element_name : IDENT",
"pseudo_class : LINK_PSCLASS_AFTER_IDENT",
"pseudo_class : VISITED_PSCLASS_AFTER_IDENT",
"pseudo_class : ACTIVE_PSCLASS_AFTER_IDENT",
"solitary_pseudo_class : LINK_PSCLASS",
"solitary_pseudo_class : VISITED_PSCLASS",
"solitary_pseudo_class : ACTIVE_PSCLASS",
"class : CLASS_AFTER_IDENT",
"solitary_class : CLASS",
"pseudo_element : FIRST_LETTER_AFTER_IDENT",
"pseudo_element : FIRST_LINE_AFTER_IDENT",
"solitary_pseudo_element : FIRST_LETTER",
"solitary_pseudo_element : FIRST_LINE",
"id : HASH_AFTER_IDENT",
"solitary_id : HASH",
"declaration : property ':' expr prio",
"declaration : property ':' expr",
"declaration : error",
"declaration :",
"prio : IMPORTANT_SYM",
"expr : term",
"expr : expr operator term",
"expr : expr error",
"term : unary_operator value",
"term : value",
"value : NUMBER",
"value : STRING",
"value : PERCENTAGE",
"value : LENGTH",
"value : EMS",
"value : EXS",
"value : IDENT",
"value : hexcolor",
"value : URL",
"value : RGB",
"hexcolor : HASH",
"hexcolor : HASH_AFTER_IDENT",

};
#endif

int      yydebug;
int      yynerrs;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
#line 573 "css_syntax.y"

int yyerror(char *s) {
#if YYDEBUG
	fprintf(stderr, "Error: %s\n", s);
#endif
	return 0;
}

struct selector_list_t* css_parse(const char *buffer, int buf_len) {
	struct selector_list_t *ret = NULL;
	//yydebug = 1;
	init_yylex(buffer, buf_len);
	yyparse(&ret);
	end_yylex();
	return ret;
}
#line 424 "css_syntax.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int      yyerrflag;
    int      yychar;
    YYSTYPE  yyval;
    YYSTYPE  yylval;

    /* variables for the parser stack */
    YYSTACKDATA yystack;
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 96 "css_syntax.y"
	{
						*(struct selector_list_t**) yyparam = yystack.l_mark[0].selector_list;
					}
break;
case 2:
#line 102 "css_syntax.y"
	{
								struct selector_list_t *pos = yystack.l_mark[-2].selector_list;
								if (pos != NULL) {
									while (pos->next != NULL) {
										pos = pos->next;
									}
									pos->next = yystack.l_mark[0].selector_list;
								} else {
									yystack.l_mark[-2].selector_list = yystack.l_mark[0].selector_list;
								}
								yyval.selector_list = yystack.l_mark[-2].selector_list;
							}
break;
case 3:
#line 114 "css_syntax.y"
	{ yyval.selector_list = NULL;  }
break;
case 12:
#line 138 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 13:
#line 139 "css_syntax.y"
	{
			char *begin = yystack.l_mark[0].lexeme;
			char *end = yystack.l_mark[0].lexeme + strlen(yystack.l_mark[0].lexeme);

			/* Skip url( */
			begin += 4;
			/* skip whitespace */
			while (*begin == ' ')
				++begin;

			/* Skip ) */
			end -= 2;
			/* skip whitespace */
			while (*end == ' ')
				--end;

			end[1] = 0;

			yyval.lexeme = strdup(begin);
			free(yystack.l_mark[0].lexeme);
		}
break;
case 14:
#line 163 "css_syntax.y"
	{ yyval.letter = '-'; }
break;
case 15:
#line 164 "css_syntax.y"
	{ yyval.letter = '+'; }
break;
case 16:
#line 168 "css_syntax.y"
	{ yyval.letter = '/'; }
break;
case 17:
#line 169 "css_syntax.y"
	{ yyval.letter = ','; }
break;
case 18:
#line 170 "css_syntax.y"
	{yyval.letter = ' '; }
break;
case 19:
#line 174 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 20:
#line 178 "css_syntax.y"
	{
										struct selector_list_t *pos = yystack.l_mark[-3].selector_list;
										while (pos != NULL) {
											struct property_t *i = yystack.l_mark[-1].property;
											while (i != NULL) {
												i->count++;
												i = i->next;
											}
											pos->selector->property = yystack.l_mark[-1].property;
											pos = pos->next;
										}
										yyval.selector_list = yystack.l_mark[-3].selector_list;
									}
break;
case 21:
#line 194 "css_syntax.y"
	{
				if (yystack.l_mark[0].selector != NULL) {
					yyval.selector_list = (struct selector_list_t*)
						malloc (sizeof(struct selector_list_t));
					yyval.selector_list->selector = yystack.l_mark[0].selector;
					yyval.selector_list->next = NULL;
				} else {
					yyval.selector_list = NULL;
				}
			}
break;
case 22:
#line 204 "css_syntax.y"
	{
								if (yystack.l_mark[-2].selector != NULL) {
									struct selector_list_t *new;
									new = (struct selector_list_t*)
										malloc (sizeof(struct selector_list_t));
									new->selector = yystack.l_mark[-2].selector;
									new->next = yystack.l_mark[0].selector_list;
									yyval.selector_list = new;
								} else {
									yyval.selector_list = yystack.l_mark[0].selector_list;
								}
							}
break;
case 23:
#line 219 "css_syntax.y"
	{
									yyval.property = yystack.l_mark[0].property;
								}
break;
case 24:
#line 222 "css_syntax.y"
	{
									if (yystack.l_mark[-2].property != NULL) {
										yystack.l_mark[-2].property->next = yystack.l_mark[0].property;
										yyval.property = yystack.l_mark[-2].property;
									} else {
										yyval.property = yystack.l_mark[0].property;
									}
								}
break;
case 25:
#line 233 "css_syntax.y"
	{
										struct selector_t *pos = yystack.l_mark[-1].selector;
										while (pos->next != NULL) {
											pos = pos->next;
										}
										pos->pseudo_element = yystack.l_mark[0].pseudo_element;
										yyval.selector = yystack.l_mark[-1].selector;
									}
break;
case 26:
#line 241 "css_syntax.y"
	{
												struct selector_t *pos = yystack.l_mark[-1].selector;
												while (pos->next != NULL) {
													pos = pos->next;
												}
												pos->next = yystack.l_mark[0].selector;
												yyval.selector = yystack.l_mark[-1].selector;
											}
break;
case 27:
#line 249 "css_syntax.y"
	{ yyval.selector = yystack.l_mark[0].selector; }
break;
case 28:
#line 250 "css_syntax.y"
	{ yyval.selector = yystack.l_mark[0].selector; }
break;
case 29:
#line 251 "css_syntax.y"
	{ yyval.selector = NULL; }
break;
case 30:
#line 252 "css_syntax.y"
	{ yyval.selector = NULL; }
break;
case 31:
#line 256 "css_syntax.y"
	{ yyval.selector = yystack.l_mark[0].selector; }
break;
case 32:
#line 257 "css_syntax.y"
	{
										yystack.l_mark[-1].selector->next = yystack.l_mark[0].selector;
										yyval.selector = yystack.l_mark[-1].selector;
									}
break;
case 33:
#line 270 "css_syntax.y"
	{
										yyval.selector = (struct selector_t*)
											malloc(sizeof(struct selector_t));
										yyval.selector->element_name = yystack.l_mark[-3].lexeme;
										yyval.selector->id = yystack.l_mark[-2].lexeme;
										yyval.selector->e_class = yystack.l_mark[-1].lexeme;
										yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
										yyval.selector->pseudo_element = 0;
										yyval.selector->next = NULL;
									 }
break;
case 34:
#line 280 "css_syntax.y"
	{
									yyval.selector = (struct selector_t*)
										malloc(sizeof(struct selector_t));
									yyval.selector->element_name = yystack.l_mark[-2].lexeme;
									yyval.selector->id = yystack.l_mark[-1].lexeme;
									yyval.selector->e_class = NULL;
									yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
									yyval.selector->pseudo_element = 0;
									yyval.selector->next = NULL;
								}
break;
case 35:
#line 290 "css_syntax.y"
	{
										yyval.selector = (struct selector_t*)
											malloc(sizeof(struct selector_t));
										yyval.selector->element_name = yystack.l_mark[-2].lexeme;
										yyval.selector->id = NULL;
										yyval.selector->e_class = yystack.l_mark[-1].lexeme;
										yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
										yyval.selector->pseudo_element = 0;
										yyval.selector->next = NULL;
									}
break;
case 36:
#line 300 "css_syntax.y"
	{
							yyval.selector = (struct selector_t*)
								malloc(sizeof(struct selector_t));
							yyval.selector->element_name = yystack.l_mark[-2].lexeme;
							yyval.selector->id = yystack.l_mark[-1].lexeme;
							yyval.selector->e_class = yystack.l_mark[0].lexeme;
							yyval.selector->pseudo_class = 0;
							yyval.selector->pseudo_element = 0;
							yyval.selector->next = NULL;
						}
break;
case 37:
#line 310 "css_syntax.y"
	{
						yyval.selector = (struct selector_t*)
							malloc(sizeof(struct selector_t));
						yyval.selector->element_name = yystack.l_mark[-1].lexeme;
						yyval.selector->id = yystack.l_mark[0].lexeme;
						yyval.selector->e_class = NULL;
						yyval.selector->pseudo_class = 0;
						yyval.selector->pseudo_element = 0;
						yyval.selector->next = NULL;
					}
break;
case 38:
#line 320 "css_syntax.y"
	{
							yyval.selector = (struct selector_t*)
								malloc(sizeof(struct selector_t));
							yyval.selector->element_name = yystack.l_mark[-1].lexeme;
							yyval.selector->id = NULL;
							yyval.selector->e_class = yystack.l_mark[0].lexeme;
							yyval.selector->pseudo_class = 0;
							yyval.selector->pseudo_element = 0;
							yyval.selector->next = NULL;
						}
break;
case 39:
#line 330 "css_syntax.y"
	{
								yyval.selector = (struct selector_t*)
									malloc(sizeof(struct selector_t));
								yyval.selector->element_name = yystack.l_mark[-1].lexeme;
								yyval.selector->id = NULL;
								yyval.selector->e_class = NULL;
								yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
								yyval.selector->pseudo_element = 0;
								yyval.selector->next = NULL;
							}
break;
case 40:
#line 340 "css_syntax.y"
	{
					yyval.selector = (struct selector_t*)
						malloc(sizeof(struct selector_t));
					yyval.selector->element_name = yystack.l_mark[0].lexeme;
					yyval.selector->id = NULL;
					yyval.selector->e_class = NULL;
					yyval.selector->pseudo_class = 0;
					yyval.selector->pseudo_element = 0;
					yyval.selector->next = NULL;
				}
break;
case 41:
#line 350 "css_syntax.y"
	{
										yyval.selector = (struct selector_t*)
											malloc(sizeof(struct selector_t));
										yyval.selector->element_name = NULL;
										yyval.selector->id = yystack.l_mark[-2].lexeme;
										yyval.selector->e_class = yystack.l_mark[-1].lexeme;
										yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
										yyval.selector->pseudo_element = 0;
										yyval.selector->next = NULL;
									}
break;
case 42:
#line 360 "css_syntax.y"
	{
						yyval.selector = (struct selector_t*)
							malloc(sizeof(struct selector_t));
						yyval.selector->element_name = NULL;
						yyval.selector->id = yystack.l_mark[-1].lexeme;
						yyval.selector->e_class = yystack.l_mark[0].lexeme;
						yyval.selector->pseudo_class = 0;
						yyval.selector->pseudo_element = 0;
						yyval.selector->next = NULL;
					}
break;
case 43:
#line 370 "css_syntax.y"
	{
								yyval.selector = (struct selector_t*)
									malloc(sizeof(struct selector_t));
								yyval.selector->element_name = NULL;
								yyval.selector->id = yystack.l_mark[-1].lexeme;
								yyval.selector->e_class = NULL;
								yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
								yyval.selector->pseudo_element = 0;
								yyval.selector->next = NULL;
							}
break;
case 44:
#line 380 "css_syntax.y"
	{
					yyval.selector = (struct selector_t*)
						malloc(sizeof(struct selector_t));
					yyval.selector->element_name = NULL;
					yyval.selector->id = yystack.l_mark[0].lexeme;
					yyval.selector->e_class = NULL;
					yyval.selector->pseudo_class = 0;
					yyval.selector->pseudo_element = 0;
					yyval.selector->next = NULL;
				}
break;
case 45:
#line 390 "css_syntax.y"
	{
									yyval.selector = (struct selector_t*)
										malloc(sizeof(struct selector_t));
									yyval.selector->element_name = NULL;
									yyval.selector->id = NULL;
									yyval.selector->e_class = yystack.l_mark[-1].lexeme;
									yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
									yyval.selector->pseudo_element = 0;
									yyval.selector->next = NULL;
								}
break;
case 46:
#line 400 "css_syntax.y"
	{
						yyval.selector = (struct selector_t*)
							malloc(sizeof(struct selector_t));
						yyval.selector->element_name = NULL;
						yyval.selector->id = NULL;
						yyval.selector->e_class = yystack.l_mark[0].lexeme;
						yyval.selector->pseudo_class = 0;
						yyval.selector->pseudo_element = 0;
						yyval.selector->next = NULL;
					}
break;
case 47:
#line 410 "css_syntax.y"
	{
							yyval.selector = (struct selector_t*)
								malloc(sizeof(struct selector_t));
							yyval.selector->element_name = NULL;
							yyval.selector->id = NULL;
							yyval.selector->e_class = NULL;
							yyval.selector->pseudo_class = yystack.l_mark[0].pseudo_class;
							yyval.selector->pseudo_element = 0;
							yyval.selector->next = NULL;
						}
break;
case 48:
#line 423 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 49:
#line 427 "css_syntax.y"
	{ yyval.pseudo_class = PS_CLASS_LINK; }
break;
case 50:
#line 428 "css_syntax.y"
	{ yyval.pseudo_class = PS_CLASS_VISITED; }
break;
case 51:
#line 429 "css_syntax.y"
	{ yyval.pseudo_class = PS_CLASS_ACTIVE; }
break;
case 52:
#line 433 "css_syntax.y"
	{ yyval.pseudo_class = PS_CLASS_LINK; }
break;
case 53:
#line 434 "css_syntax.y"
	{ yyval.pseudo_class = PS_CLASS_VISITED; }
break;
case 54:
#line 435 "css_syntax.y"
	{ yyval.pseudo_class = PS_CLASS_ACTIVE; }
break;
case 55:
#line 439 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 56:
#line 443 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 57:
#line 447 "css_syntax.y"
	{ yyval.pseudo_element = PS_ELEMENT_FIRST_LETTER; }
break;
case 58:
#line 448 "css_syntax.y"
	{ yyval.pseudo_element = PS_ELEMENT_FIRST_LINE; }
break;
case 59:
#line 452 "css_syntax.y"
	{
					yyval.selector = (struct selector_t*)
						malloc(sizeof(struct selector_t));
					yyval.selector->element_name = NULL;
					yyval.selector->id = NULL;
					yyval.selector->e_class = NULL;
					yyval.selector->pseudo_class = 0;
					yyval.selector->pseudo_element = PS_ELEMENT_FIRST_LETTER;
					yyval.selector->next = NULL;
				}
break;
case 60:
#line 462 "css_syntax.y"
	{
					yyval.selector = (struct selector_t*)
						malloc(sizeof(struct selector_t));
					yyval.selector->element_name = NULL;
					yyval.selector->id = NULL;
					yyval.selector->e_class = NULL;
					yyval.selector->pseudo_class = 0;
					yyval.selector->pseudo_element = PS_ELEMENT_FIRST_LINE;
					yyval.selector->next = NULL;
				}
break;
case 61:
#line 479 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 62:
#line 483 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 63:
#line 487 "css_syntax.y"
	{
								yyval.property = (struct property_t*)
									malloc(sizeof(struct property_t));
								yyval.property->name = yystack.l_mark[-3].lexeme;
								yyval.property->val = yystack.l_mark[-1].lexeme;
								yyval.property->important = 1;
								yyval.property->count = 0;
								yyval.property->next = NULL;
							}
break;
case 64:
#line 496 "css_syntax.y"
	{
								yyval.property = (struct property_t*)
									malloc(sizeof(struct property_t));
								yyval.property->name = yystack.l_mark[-2].lexeme;
								yyval.property->val = yystack.l_mark[0].lexeme;
								yyval.property->important = 0;
								yyval.property->count = 0;
								yyval.property->next = NULL;
							}
break;
case 65:
#line 505 "css_syntax.y"
	{ yyval.property = NULL; }
break;
case 66:
#line 506 "css_syntax.y"
	{
								yyval.property = NULL;
							}
break;
case 67:
#line 512 "css_syntax.y"
	{ }
break;
case 68:
#line 516 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 69:
#line 517 "css_syntax.y"
	{
							char *s = (char*) malloc (strlen(yystack.l_mark[-2].lexeme)+strlen(yystack.l_mark[0].lexeme)+2);
							strcpy(s, yystack.l_mark[-2].lexeme);
							s[strlen(s)+1] = 0;
							s[strlen(s)] = yystack.l_mark[-1].letter;
							strcat(s, yystack.l_mark[0].lexeme);
							free(yystack.l_mark[-2].lexeme);
							free(yystack.l_mark[0].lexeme);
							yyval.lexeme = s;
						}
break;
case 70:
#line 527 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[-1].lexeme; }
break;
case 71:
#line 531 "css_syntax.y"
	{
							char *s = (char*) malloc(strlen(yystack.l_mark[0].lexeme)+2);
							s[0] = yystack.l_mark[-1].letter;
							s[1] = 0;
							strcat(s, yystack.l_mark[0].lexeme);
							free(yystack.l_mark[0].lexeme);
							yyval.lexeme = s;
						}
break;
case 72:
#line 539 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 73:
#line 543 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 74:
#line 544 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 75:
#line 545 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 76:
#line 546 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 77:
#line 547 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 78:
#line 548 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 79:
#line 549 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 80:
#line 550 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 81:
#line 551 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 82:
#line 552 "css_syntax.y"
	{ yyval.lexeme = yystack.l_mark[0].lexeme; }
break;
case 83:
#line 560 "css_syntax.y"
	{ 
			yyval.lexeme = (char*) malloc (strlen(yystack.l_mark[0].lexeme)+2);
			sprintf(yyval.lexeme, "#%s", yystack.l_mark[0].lexeme);
			free(yystack.l_mark[0].lexeme);
		}
break;
case 84:
#line 565 "css_syntax.y"
	{ 
						yyval.lexeme = (char*) malloc (strlen(yystack.l_mark[0].lexeme)+2);
						sprintf(yyval.lexeme, "#%s", yystack.l_mark[0].lexeme);
						free(yystack.l_mark[0].lexeme);
					}
break;
#line 1223 "css_syntax.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
