/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "./parser.y"

#include<bits/stdc++.h>
#include<iostream>
#include<sstream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cmath>
#include<fstream>
#include "symbol_table.h"


using namespace std;


int yyparse(void);
int yylex(void);


extern FILE *yyin;
extern long long int yylineno;
extern ofstream tok;
extern long long int error_cnt;



SymbolTable st(30);
ofstream fp2,fp3,fp4,fp5;

int labelCount=0;
int tempCount=0;


/*
char *newTemp()
{
	char *t= new char[4];
	strcpy(t,"t");
	char b[3];
	sprintf(b,"%d", tempCount);
	tempCount++;
	strcat(t,b);
	return t;
}
*/


void print_line()
{
	fp2<<"Line "<<yylineno<<": ";
}

void error_print_line()
{
	fp3<<"Error at line "<<yylineno<<": ";
	fp2<<"Error at line "<<yylineno<<": ";
}

void yyerror(char *s)
{
	 error_print_line();
	 fp2<<s<<endl<<endl;
	 fp3<<s<<endl<<endl;
}

//declaration_list vector
vector<pair<string,int>> decl_list;

//function param_list
vector<pair<string,string>> param_list;

//function call argument list
vector<string> arg_list;

//keep track of where function started
int temp_line;
string temp_id;


//mismatch cascade control

map<int,int> mismatch_map;

//code generation

//data segment lines
vector<string> data_seg;
//array_index

//function_parameter_symbol
vector<string> param_symbol;

//function_argument_symbol
vector<string> arg_symbol;

//track temp variables used in a statement
vector<string> current_temp;

//to store temp variables
vector<pair<int,int>> temp_var(10001,make_pair(-1,0));
int max_temp_var = 10001;

void free_var(string var)
{
	string sub = var.substr(1);
 
    // object from the class stringstream
    stringstream geek(sub);

    int x = 0;
    geek >> x;

	temp_var[x].first = -1;
}

string newTemp()
{
	string var_name = "t";
	int i=0;
	for(;i<max_temp_var;i++)
	{
		if(temp_var[i].first == -1)
		{
			var_name += to_string(i);
			temp_var[i].first = 0;

			if(temp_var[i].second == 0)
			{
				data_seg.push_back(var_name+" dw ?");
				temp_var[i].second = 1;
			}
			break;
		}
	}

	if(i == max_temp_var)
	{
		//out of temp_vars
		int prev_size = max_temp_var;
		max_temp_var+=10001;
		vector<pair<int,int>> new_temp_var(max_temp_var,make_pair(-1,0));

		for(int j=0;j<prev_size;j++)
		{
			new_temp_var[j] = temp_var[j];
		}

		temp_var = new_temp_var;

		i = 0;
		for(;i<max_temp_var;i++)
		{
			if(temp_var[i].first == -1)
			{
				var_name += to_string(i);
				temp_var[i].first = 0;

				if(temp_var[i].second == 0)
				{
					data_seg.push_back(var_name+" dw ?");
					temp_var[i].second = 1;
				}
				break;
			}
		}
	}

	return var_name;
}


char *newLabel()
{
	char *lb= new char[4];
	strcpy(lb,"L");
	char b[3];
	sprintf(b,"%d", labelCount);
	labelCount++;
	strcat(lb,b);
	return lb;
}


bool insert_ID(string name, string data_type, int is_array)
{
	SymbolInfo* s = st.Look_up_current(name);

	if(s == NULL)
	{
		st.Insert(name,"ID");
		SymbolInfo* temp = st.Look_up_current(name);
		temp->set_data_type(data_type);

		string scope_id = st.get_curr()->get_id();
		//replace all '.' with '_'
		for(int i=0;i<scope_id.length();i++)
		{
			if(scope_id[i] == '.')
			{
				scope_id[i] = '_';
			}
		}
		string var_name = name+scope_id;
		temp->set_symbol(var_name);

		if(is_array != 0)
		{
			//array
			temp->set_array(true);
			temp->setType("array");
			//set size
			temp->set_size(is_array);
			//add to data segment
			string val = var_name+" dw "+ to_string(is_array)+" dup(?)";
			data_seg.push_back(val);
		}

		else
		{
			//not array
			
			//add to data segment
			data_seg.push_back(var_name+" dw ?");
		}
		return true;
	}

	return false;
}

bool insert_function(string name, string return_type,int flag)
{
	SymbolInfo* s = st.Look_up(name);

	if(s == NULL)
	{
		st.Insert(name,"ID");
		SymbolInfo* temp = st.Look_up_current(name);
		temp->set_func((int)param_list.size(),return_type,param_list,flag);

		return true;
	}

	return false;

}

bool check_var_declared(string name)
{
	SymbolInfo *s = st.Look_up(name);

	if(s != NULL)
		return true;
	else
		return false;

}

bool check_func_declared(string name)
{
	SymbolInfo* s = st.Look_up(name);
	func_param* f = s->get_func();

	if(f->get_flag() == 0)
	{
		//make it defined
		f->set_flag(1);
		return true;
	}

	else
		return false;
}


void set_data_type(SymbolInfo* s1, SymbolInfo* s2)
{
	s1->set_data_type(s2->get_data_type());
}

void enterScope_parser()
{
	cout<<"entered scope"<<endl;
	st.enter_scope();

	string scope_id = st.get_curr()->get_id();

	//replace all '.' with '_'
	for(int i=0;i<scope_id.length();i++)
	{
		if(scope_id[i] == '.')
		{
			scope_id[i] = '_';
		}
	}
	//string var_name = name+scope_id;

	//add func parameters int the new scope
	int stack_index = (int)(param_list.size()); 
	for(auto x:param_list)
	{
		//cout<<"("<<x.first<<", "<<x.second<<")"<<endl;
		//fp2<<"("<<x.first<<", "<<x.second<<")"<<endl;
		
		bool ok = st.Insert(x.first, "ID");

		if(ok)
		{
			SymbolInfo* si = st.Look_up(x.first);
			si->set_data_type(x.second);

			string para_sym = x.first+scope_id;
			int temp_index = 10+stack_index*2;
			si->set_symbol("[bp+"+to_string(temp_index)+"]");
			//data_seg.push_back(para_sym+" dw ?");
			param_symbol.push_back(para_sym);

			stack_index--;
		}
		else //check multiple declaration in the parameter
		{
			error_cnt++;
			error_print_line();
			fp3<<"Multiple declaration of "+x.first +" in parameter"<<endl<<endl;
			fp2<<"Multiple declaration of "+x.first +" in parameter"<<endl<<endl;
		}
	}
}

void exitScope_parser()
{
	cout<<"exited scope"<<endl;
	st.Print_all(fp2);
	st.exit_scope();
}







void print_proc(ofstream &fp4)
{
	fp4<<"print PROC"<<endl;
	fp4<<"\tPUSH ax"<<endl;
	fp4<<"\tPUSH bx"<<endl; 
	fp4<<"\tPUSH cx"<<endl;
	fp4<<"\tPUSH dx"<<endl;
	fp4<<"\tMOV cx, 0"<<endl;
	fp4<<"\tXOR dx,dx"<<endl;
	fp4<<"check_if_negative:"<<endl;
	fp4<<"\tCMP print_var,32767"<<endl;
	fp4<<"\tJA negative"<<endl;
	fp4<<"\tJMP loop_begin"<<endl;
	fp4<<"negative:"<<endl;
	fp4<<"\tMOV ax,65535"<<endl;
	fp4<<"\tSUB ax, print_var"<<endl;
	fp4<<"\tMOV print_var,ax"<<endl;
	fp4<<"\tINC print_var"<<endl;
	fp4<<"\tMOV dx,'-'"<<endl;
	fp4<<"\tMOV ah,2"<<endl;
	fp4<<"\tINT 21H"<<endl;
	fp4<<"loop_begin:"<<endl;
	fp4<<"\tMOV ax,print_var"<<endl;
	fp4<<"\tMOV dx, 0"<<endl;
	fp4<<"\tMOV bx,10"<<endl;
	fp4<<"\tDIV bx"<<endl;
	fp4<<"\tMOV print_var,ax"<<endl;
	fp4<<"\tADD dx,'0'"<<endl;
	fp4<<"\tPUSH dx"<<endl;
	fp4<<"\tINC cx"<<endl;
	fp4<<"\tCMP print_var,0"<<endl;
	fp4<<"\tJE loop_exit"<<endl;
	fp4<<"\tJMP loop_begin"<<endl;
	fp4<<"loop_exit:"<<endl;
	fp4<<"\tPOP dx"<<endl;
	fp4<<"\tMOV ah,2"<<endl;
	fp4<<"\tINT 21H"<<endl;
	fp4<<"\tDEC cx"<<endl;
	fp4<<"\tCMP cx,0"<<endl;
	fp4<<"\tJE func_end"<<endl;
	fp4<<"\tJMP loop_exit"<<endl;
	fp4<<"func_end:"<<endl;
	fp4<<"\tMOV dl,10"<<endl;
	fp4<<"\tMOV ah,02h"<<endl;
	fp4<<"\tINT 21H"<<endl;
	fp4<<"\tMOV dl,13"<<endl;
	fp4<<"\tMOV ah,02h"<<endl;
	fp4<<"\tINT 21H"<<endl;
	fp4<<"\tPOP dx"<<endl;
	fp4<<"\tPOP cx"<<endl;
	fp4<<"\tPOP bx"<<endl;
	fp4<<"\tPOP ax"<<endl;
	fp4<<"\tRET"<<endl;
	fp4<<"ENDP print"<<endl;
}

void init()
{
	fp4<<".MODEL SMALL"<<endl;
	fp4<<".STACK 100H"<<endl;
	fp4<<".DATA"<<endl;
	fp4<<"\tprint_var dw ?"<<endl;
	fp4<<"\tret_temp dw ?"<<endl;

	fp5<<".MODEL SMALL"<<endl;
	fp5<<".STACK 100H"<<endl;
	fp5<<".DATA"<<endl;
	fp5<<"\tprint_var dw ?"<<endl;
	fp5<<"\tret_temp dw ?"<<endl;

	for(string x:data_seg)
	{
		fp4<<"\t"<<x<<endl;
		fp5<<"\t"<<x<<endl;
	}

	fp4<<".CODE"<<endl;
	fp5<<".CODE"<<endl;
	print_proc(fp4);
	print_proc(fp5);
}

void optimize_code(string unop_code)
{
	vector <string> lines;
      
    // stringstream class check1
    stringstream check1(unop_code);
      
    string intermediate;
      
    // Tokenizing w.r.t. space '\n'
    while(getline(check1, intermediate, '\n'))
    {
        lines.push_back(intermediate);
    }

	string opt_code = "";

	for(int i=0;i<(int)lines.size()-1;i++)
	{
		opt_code+=lines[i]+"\n";
		if(lines[i].length() > 4)
		{
			if(lines[i].substr(1,3) == "MOV" && lines[i+1].substr(1,3) == "MOV")
			{
				int pos1 = lines[i].find(",");
				int pos2 = lines[i+1].find(",");

				if(pos1 >=5 && pos2>= 5){
				/*
				string val1 = lines[i].substr(5,pos1-5);
				string val2 = lines[i+1].substr(pos2+1);
				string val3 = lines[i+1].substr(5,pos2-5);
				string val4 = lines[i].substr(pos1+1);

				fp5<<","+val1+"->"+val2+"->"+val3+"->"+val4<<"\n";
				*/

				if(lines[i].substr(5,pos1-5) == lines[i+1].substr(pos2+1) && lines[i+1].substr(5,pos2-5) == lines[i].substr(pos1+1))
				{
					i++; //skip the next line
				}
				}
			}
		}
	}

	fp5<<opt_code;
	fp5<<"END main\n";
}




#line 548 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CONST_INT = 258,
    CONST_FLOAT = 259,
    CONST_CHAR = 260,
    ID = 261,
    INT = 262,
    FLOAT = 263,
    VOID = 264,
    CHAR = 265,
    DOUBLE = 266,
    IF = 267,
    ELSE = 268,
    FOR = 269,
    WHILE = 270,
    RETURN = 271,
    PRINTLN = 272,
    CONTINUE = 273,
    DO = 274,
    ASSIGNOP = 275,
    NOT = 276,
    INCOP = 277,
    DECOP = 278,
    LOGICOP = 279,
    RELOP = 280,
    ADDOP = 281,
    MULOP = 282,
    LPAREN = 283,
    RPAREN = 284,
    LCURL = 285,
    RCURL = 286,
    LTHIRD = 287,
    RTHIRD = 288,
    COMMA = 289,
    SEMICOLON = 290,
    LOWER_THAN_ELSE = 291
  };
#endif
/* Tokens.  */
#define CONST_INT 258
#define CONST_FLOAT 259
#define CONST_CHAR 260
#define ID 261
#define INT 262
#define FLOAT 263
#define VOID 264
#define CHAR 265
#define DOUBLE 266
#define IF 267
#define ELSE 268
#define FOR 269
#define WHILE 270
#define RETURN 271
#define PRINTLN 272
#define CONTINUE 273
#define DO 274
#define ASSIGNOP 275
#define NOT 276
#define INCOP 277
#define DECOP 278
#define LOGICOP 279
#define RELOP 280
#define ADDOP 281
#define MULOP 282
#define LPAREN 283
#define RPAREN 284
#define LCURL 285
#define RCURL 286
#define LTHIRD 287
#define RTHIRD 288
#define COMMA 289
#define SEMICOLON 290
#define LOWER_THAN_ELSE 291

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 480 "./parser.y"
int ival;SymbolInfo* si;

#line 675 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   182

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  37
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  77
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  134

#define YYUNDEFTOK  2
#define YYMAXUTOK   291


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   503,   503,   522,   532,   544,   551,   558,   571,   620,
     673,   789,   916,   938,   956,   971,   982,   994,  1002,  1009,
    1020,  1032,  1045,  1053,  1090,  1127,  1135,  1142,  1149,  1158,
    1167,  1182,  1192,  1208,  1220,  1246,  1274,  1283,  1293,  1304,
    1332,  1355,  1380,  1405,  1426,  1440,  1449,  1463,  1472,  1487,
    1500,  1508,  1558,  1633,  1649,  1712,  1727,  1800,  1814,  1888,
    1902,  1960,  1974,  2078,  2110,  2139,  2156,  2186,  2359,  2373,
    2391,  2404,  2443,  2483,  2493,  2501,  2517,  2535
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CONST_INT", "CONST_FLOAT", "CONST_CHAR",
  "ID", "INT", "FLOAT", "VOID", "CHAR", "DOUBLE", "IF", "ELSE", "FOR",
  "WHILE", "RETURN", "PRINTLN", "CONTINUE", "DO", "ASSIGNOP", "NOT",
  "INCOP", "DECOP", "LOGICOP", "RELOP", "ADDOP", "MULOP", "LPAREN",
  "RPAREN", "LCURL", "RCURL", "LTHIRD", "RTHIRD", "COMMA", "SEMICOLON",
  "LOWER_THAN_ELSE", "$accept", "start", "program", "unit",
  "func_declaration", "func_definition", "func_begin", "parameter_list",
  "compound_statement", "dummy_token_begin", "var_declaration",
  "type_specifier", "declaration_list", "statements", "statement",
  "expression_statement", "variable", "expression", "logic_expression",
  "rel_expression", "simple_expression", "term", "unary_expression",
  "factor", "argument_list", "arguments", "id", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291
};
# endif

#define YYPACT_NINF (-77)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-13)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     121,   -77,   -77,   -77,     3,   121,   -77,   -77,   -77,   -77,
      22,   -77,   -77,   -16,   -77,    19,    69,   -77,    25,    15,
     -77,    27,     8,    15,   -77,    30,    18,    56,   -77,    80,
     -77,   -77,    -1,    21,    15,    39,   -77,   -77,   -77,   -77,
      36,   162,   -77,   -77,    72,   -77,   -77,   -77,    15,    57,
     -77,   -77,    66,    71,    82,   154,    96,   154,   154,   154,
     -77,   -77,   -77,   -77,   -77,   -77,   105,   -77,   -77,   156,
      10,   -77,    94,    79,    98,   -77,   -77,    95,   -77,   -77,
     154,    11,   154,    97,    15,    93,   -77,   -77,   108,   -77,
     -77,   154,   -77,   -77,    99,   -77,   154,   154,   154,   154,
     154,   154,   109,    11,   114,   -77,   119,   -77,   -77,   -77,
     -77,   123,    98,   -77,   -77,   122,   127,   129,   138,   154,
     138,   130,   -77,   154,   -77,   143,   145,   -77,   -77,   -77,
     138,   138,   -77,   -77
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    26,    27,    28,     0,     2,     4,     6,     7,     5,
       0,     1,     3,     0,    77,     0,    31,    25,     0,     0,
      23,     0,     0,     0,    24,    29,     0,     0,    33,     0,
      32,    19,     0,     0,    16,     0,    22,     9,    11,    18,
       0,     0,    15,    30,     0,     8,    10,    17,    14,     0,
      69,    70,     0,     0,     0,     0,     0,     0,     0,     0,
      21,    47,    45,    46,    38,    36,     0,    34,    37,    66,
       0,    53,    55,    57,    59,    61,    65,    51,    13,    49,
       0,     0,     0,     0,     0,    66,    64,    63,     0,    20,
      35,     0,    71,    72,     0,    48,     0,     0,     0,     0,
      74,     0,     0,     0,     0,    44,     0,    68,    54,    50,
      56,    58,    60,    62,    76,     0,    73,     0,     0,     0,
       0,     0,    67,     0,    52,    40,     0,    42,    43,    75,
       0,     0,    41,    39
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -77,   -77,   -77,   167,    77,    90,   -77,   -77,   -14,   -77,
      91,    20,   -77,   -77,   -62,   -76,   -56,   -49,   -75,    81,
      70,    83,   -50,   -77,   -77,   -77,   -10
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,    62,    63,    22,    33,    64,    44,
      65,    10,    15,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,   115,   116,    77
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      16,    85,    85,    11,    90,   103,    83,    86,    87,    25,
      88,    94,    49,    28,    50,    51,   108,    14,    38,    17,
      18,    14,    39,    13,    42,   114,    46,   119,    14,    36,
      26,   102,    57,   104,    37,    85,    27,    58,    78,    59,
      85,    85,    85,    85,    85,    95,    61,    34,   129,   113,
      40,    30,   117,    19,    20,    41,   125,    31,   127,    23,
      24,    48,    29,     1,     2,     3,    36,    85,   132,   133,
     126,    45,    43,    49,   106,    50,    51,     7,    14,     1,
       2,     3,     7,    35,    52,    32,    53,    54,    55,    56,
       8,     9,    79,    57,    80,     8,     9,   -12,    58,    81,
      59,    21,    36,    60,    97,    98,    49,    61,    50,    51,
      82,    14,     1,     2,     3,    92,    93,    52,    96,    53,
      54,    55,    56,   100,    84,    99,    57,   101,     1,     2,
       3,    58,   105,    59,   109,    36,    89,   107,   118,    49,
      61,    50,    51,   120,    14,     1,     2,     3,   121,    98,
      52,   122,    53,    54,    55,    56,   130,    50,    51,    57,
      14,   123,   124,    47,    58,   128,    59,   111,    36,     1,
       2,     3,    12,    61,   131,    57,    91,   110,    92,    93,
      58,   112,    59
};

static const yytype_uint8 yycheck[] =
{
      10,    57,    58,     0,    66,    81,    55,    57,    58,    19,
      59,     1,     1,    23,     3,     4,    91,     6,    32,    35,
       1,     6,     1,     1,    34,   100,    40,   103,     6,    30,
       3,    80,    21,    82,    35,    91,    28,    26,    48,    28,
      96,    97,    98,    99,   100,    35,    35,    27,   123,    99,
      29,    33,   101,    34,    35,    34,   118,     1,   120,    34,
      35,    41,    32,     7,     8,     9,    30,   123,   130,   131,
     119,    35,    33,     1,    84,     3,     4,     0,     6,     7,
       8,     9,     5,     3,    12,    29,    14,    15,    16,    17,
       0,     0,    35,    21,    28,     5,     5,    28,    26,    28,
      28,    32,    30,    31,    25,    26,     1,    35,     3,     4,
      28,     6,     7,     8,     9,    22,    23,    12,    24,    14,
      15,    16,    17,    28,    28,    27,    21,    32,     7,     8,
       9,    26,    35,    28,    35,    30,    31,    29,    29,     1,
      35,     3,     4,    29,     6,     7,     8,     9,    29,    26,
      12,    29,    14,    15,    16,    17,    13,     3,     4,    21,
       6,    34,    33,     1,    26,    35,    28,    97,    30,     7,
       8,     9,     5,    35,    29,    21,    20,    96,    22,    23,
      26,    98,    28
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     7,     8,     9,    38,    39,    40,    41,    42,    47,
      48,     0,    40,     1,     6,    49,    63,    35,     1,    34,
      35,    32,    43,    34,    35,    63,     3,    28,    63,    32,
      33,     1,    29,    44,    48,     3,    30,    35,    45,     1,
      29,    34,    63,    33,    46,    35,    45,     1,    48,     1,
       3,     4,    12,    14,    15,    16,    17,    21,    26,    28,
      31,    35,    41,    42,    45,    47,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    63,    63,    35,
      28,    28,    28,    54,    28,    53,    59,    59,    54,    31,
      51,    20,    22,    23,     1,    35,    24,    25,    26,    27,
      28,    32,    54,    52,    54,    35,    63,    29,    55,    35,
      56,    57,    58,    59,    55,    61,    62,    54,    29,    52,
      29,    29,    29,    34,    33,    51,    54,    51,    35,    55,
      13,    29,    51,    51
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    37,    38,    39,    39,    40,    40,    40,    41,    41,
      42,    42,    43,    44,    44,    44,    44,    44,    44,    44,
      45,    45,    46,    47,    47,    47,    48,    48,    48,    49,
      49,    49,    49,    49,    50,    50,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    52,    52,    52,
      52,    53,    53,    54,    54,    55,    55,    56,    56,    57,
      57,    58,    58,    59,    59,    59,    60,    60,    60,    60,
      60,    60,    60,    61,    61,    62,    62,    63
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     7,     6,
       7,     6,     0,     4,     3,     2,     1,     3,     2,     1,
       4,     3,     0,     3,     4,     3,     1,     1,     1,     3,
       6,     1,     4,     4,     1,     2,     1,     1,     1,     7,
       5,     7,     5,     5,     3,     1,     1,     1,     2,     2,
       3,     1,     4,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     2,     2,     1,     1,     4,     3,     1,
       1,     2,     2,     1,     0,     3,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 504 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(), "NON_TERMINAL");
		print_line();
		fp2<<"start : program\n"<<endl;
		//fp2<<$$->getName()<<endl<<endl;

		if(error_cnt == 0) // generate code
		{	
			init();
			fp4<<(yyvsp[0].si)->code<<endl;
			optimize_code((yyvsp[0].si)->code);
		}
		// else blank code.asm

		fp2<<(yyval.si)->get_code()<<endl;
	}
#line 1969 "y.tab.c"
    break;

  case 3:
#line 523 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			//add two code segments
			(yyval.si)->set_code((yyvsp[-1].si)->get_code()+(yyvsp[0].si)->get_code());
			print_line();
			fp2<<"program : program unit\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
		}
#line 1983 "y.tab.c"
    break;

  case 4:
#line 533 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(), "NON_TERMINAL");
			//set_code
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			print_line();
			fp2<<"program : unit\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
		}
#line 1997 "y.tab.c"
    break;

  case 5:
#line 545 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName()+"\n", "NON_TERMINAL");
			print_line();
			fp2<<"unit : var_declaration\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
		}
#line 2008 "y.tab.c"
    break;

  case 6:
#line 552 "./parser.y"
        {	
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName()+"\n", "NON_TERMINAL");
			print_line();
			fp2<<"unit : func_declaration\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
		}
#line 2019 "y.tab.c"
    break;

  case 7:
#line 559 "./parser.y"
        {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName()+"\n", "NON_TERMINAL");
			//set code
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
		 	print_line();
			fp2<<"unit : func_definition\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
		}
#line 2033 "y.tab.c"
    break;

  case 8:
#line 572 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-6].si)->getName()+" "+(yyvsp[-5].si)->getName()+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON\n"<<endl;
			

			SymbolInfo* s = st.Look_up((yyvsp[-5].si)->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == 1)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
				}

				else if(f->get_flag() == 0)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
				}

				else if(f->get_flag() == -1)
				{
					//set declaration
					s->set_func((int)param_list.size(),(yyvsp[-6].si)->getName(),param_list,0);
				}
			}

			else
			{
				error_cnt++;
				fp3<<"Error at line "<<temp_line<<": ";
				fp3<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;

			}
			
			//clear the parameter list
			param_list.clear();

			fp2<<(yyval.si)->getName()<<endl<<endl;
		}
#line 2086 "y.tab.c"
    break;

  case 9:
#line 621 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-5].si)->getName()+" "+(yyvsp[-4].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON\n"<<endl;
			

			SymbolInfo* s = st.Look_up((yyvsp[-4].si)->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == 1)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
				}

				else if(f->get_flag() == 0)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
				}

				else if(f->get_flag() == -1)
				{
					//set declaration
					s->set_func((int)param_list.size(),(yyvsp[-5].si)->getName(),param_list,0);
					f->set_flag(0);
				}
			}

			else
			{
				error_cnt++;
				fp3<<"Error at line "<<temp_line<<": ";
				fp3<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
			}
			
			
			//clear the parameter list
			param_list.clear();

			fp2<<(yyval.si)->getName()<<endl<<endl;
			
		}
#line 2141 "y.tab.c"
    break;

  case 10:
#line 674 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-6].si)->getName()+" "+(yyvsp[-5].si)->getName()+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement\n"<<endl;
			

			SymbolInfo* s = st.Look_up((yyvsp[-5].si)->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == -1)
				{
					s->set_func((int)param_list.size(),(yyvsp[-6].si)->getName(),param_list,1);
					f->set_flag(1);
				}

				else if(f->get_flag() == 1)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple definition of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
					fp2<<"Multiple definition of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
				}

				else if(f->get_flag() == 0)// declared
				{
					f->set_flag(1);
					if((yyvsp[-6].si)->getName() != f->getReturn_type())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp2<<"Error at line "<<temp_line<<": ";
						fp3<<"Return type mismatch with function declaration in function "<<(yyvsp[-5].si)->getName()<<endl<<endl;
						fp2<<"Return type mismatch with function declaration in function "<<(yyvsp[-5].si)->getName()<<endl<<endl;
					}

					else if((int)param_list.size() != f->getNumber_of_param())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp2<<"Error at line "<<temp_line<<": ";
						fp3<<"Total number of arguments mismatch with declaration in function "<<(yyvsp[-5].si)->getName()<<endl<<endl;
						fp2<<"Total number of arguments mismatch with declaration in function "<<(yyvsp[-5].si)->getName()<<endl<<endl;
					}

					else
					{
						vector<pair<string,string>> func_param_list = f->getParam_list();
						int i = 0;
						for(auto x:param_list)
						{
							if(x.second != func_param_list[i].second)
							{
								error_cnt++;
								fp3<<"Error at line "<<temp_line<<": ";
								fp3<<i+1<<"th argument mismatch in function "<<(yyvsp[-5].si)->getName()<<endl<<endl;
								fp2<<"Error at line "<<temp_line<<": ";
								fp2<<i+1<<"th argument mismatch in function "<<(yyvsp[-5].si)->getName()<<endl<<endl;
								break;
							}
							i++;
						}
					}

					
				}
			}

			else
			{
				error_cnt++;
				fp3<<"Error at line "<<temp_line<<": ";
				fp3<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<(yyvsp[-5].si)->getName()<<endl<<endl;
			}	

			//code
			if(f!=NULL)
			{
				f->set_param_symbol(param_symbol);

				(yyval.si)->code +=(yyvsp[-5].si)->getName()+" PROC\n";
				(yyval.si)->code +="\tPUSH ax\n";
				(yyval.si)->code +="\tPUSH bx\n";
				(yyval.si)->code +="\tPUSH cx\n";
				(yyval.si)->code +="\tPUSH dx\n";
				(yyval.si)->code +="\tPUSH bp\n";
				(yyval.si)->code +="\tMOV bp,sp\n";
				//function body start
				(yyval.si)->code += (yyvsp[0].si)->get_code();
				//end
				(yyval.si)->code +="\tPOP bp\n";
				(yyval.si)->code +="\tPOP dx\n";
				(yyval.si)->code +="\tPOP cx\n";
				(yyval.si)->code +="\tPOP bx\n";
				(yyval.si)->code +="\tPOP ax\n";

				/*
				if($1->getName() != "void")
				{
					$$->code +="\tPUSH ret_temp\n";
				}
				*/
				(yyval.si)->code +="\tRET 2\n";
				(yyval.si)->code +=(yyvsp[-5].si)->getName()+" endp\n";
			}	
		
				
			param_list.clear();
			param_symbol.clear();

			fp2<<(yyval.si)->getName()<<endl<<endl;

		}
#line 2261 "y.tab.c"
    break;

  case 11:
#line 790 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-5].si)->getName()+" "+(yyvsp[-4].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_definition : type_specifier ID LPAREN RPAREN compound_statement\n"<<endl;
			

			SymbolInfo* s = st.Look_up((yyvsp[-4].si)->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == -1)
				{
					s->set_func((int)param_list.size(),(yyvsp[-5].si)->getName(),param_list,1);
				}

				else if(f->get_flag() == 0)// declared
				{
					f->set_flag(1);
					if((yyvsp[-5].si)->getName() != f->getReturn_type())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp3<<"Return type mismatch with function declaration in function "<<(yyvsp[-4].si)->getName()<<endl<<endl;
						fp2<<"Error at line "<<temp_line<<": ";
						fp2<<"Return type mismatch with function declaration in function "<<(yyvsp[-4].si)->getName()<<endl<<endl;
					}

					else if((int)param_list.size() != f->getNumber_of_param())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp3<<"Total number of arguments mismatch with declaration in function "<<(yyvsp[-4].si)->getName()<<endl<<endl;
						fp2<<"Error at line "<<temp_line<<": ";
						fp2<<"Total number of arguments mismatch with declaration in function "<<(yyvsp[-4].si)->getName()<<endl<<endl;
					}

					else
					{
						vector<pair<string,string>> func_param_list = f->getParam_list();
						int i = 0;
						for(auto x:param_list)
						{
							if(x.second != func_param_list[i].second)
							{
								error_cnt++;
								fp3<<"Error at line "<<temp_line<<": ";
								fp3<<i+1<<"th argument mismatch in function "<<(yyvsp[-4].si)->getName()<<endl<<endl;
								fp2<<"Error at line "<<temp_line<<": ";
								fp2<<i+1<<"th argument mismatch in function "<<(yyvsp[-4].si)->getName()<<endl<<endl;
								break;
							}
							i++;
						}
					}
				}
			}

			else
			{
				error_cnt++;
				fp3<<"Error at line "<<temp_line<<": ";
				fp3<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<(yyvsp[-4].si)->getName()<<endl<<endl;
			}

			vector<string> empty_list;
			f->set_param_symbol(empty_list);	

			//code
			if((yyvsp[-4].si)->getName() == "main")
			{
				(yyval.si)->code +="main proc\n";
				(yyval.si)->code+="\tmov ax,@data\n";
				(yyval.si)->code+="\tmov ds,ax\n\n\n";
				(yyval.si)->code+=(yyvsp[0].si)->get_code();
				(yyval.si)->code+="\tMOV AH,4CH\n";
				(yyval.si)->code+="\tINT 21H\n";
				(yyval.si)->code+="ENDP main\n";
				(yyval.si)->code+="END main";
			}

			

			//code
			if(f!=NULL && (yyvsp[-4].si)->getName() != "main")
			{
				f->set_param_symbol(param_symbol);

				(yyval.si)->code +=(yyvsp[-4].si)->getName()+" PROC\n";
				(yyval.si)->code +="\tPUSH ax\n";
				(yyval.si)->code +="\tPUSH bx\n";
				(yyval.si)->code +="\tPUSH cx\n";
				(yyval.si)->code +="\tPUSH dx\n";
				(yyval.si)->code +="\tPUSH bp\n";
				(yyval.si)->code +="\tMOV bp,sp\n";
				//function body start
				(yyval.si)->code += (yyvsp[0].si)->get_code();
				//end
				(yyval.si)->code +="\tPOP bp\n";
				(yyval.si)->code +="\tPOP dx\n";
				(yyval.si)->code +="\tPOP cx\n";
				(yyval.si)->code +="\tPOP bx\n";
				(yyval.si)->code +="\tPOP ax\n";
				/*if($1->getName() != "void")
				{
					$$->code +="\tPUSH ret_temp\n";
				}*/
				(yyval.si)->code +="\tRET 2\n";
				(yyval.si)->code +="ENDP "+(yyvsp[-4].si)->getName()+"\n";
			}	
			
					
			param_list.clear();
			param_symbol.clear();

			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

		}
#line 2386 "y.tab.c"
    break;

  case 12:
#line 916 "./parser.y"
                {
			//save the line for error reporting 
			temp_line = yylineno;

			//fp2<<"here"<<endl;
			//last id -> temp_id
			//fp2<<temp_id<<endl;

			SymbolInfo* s = st.Look_up(temp_id);
			if(s == NULL)
			{
				st.Insert(temp_id,"ID");
				SymbolInfo* s1 = st.Look_up(temp_id); 
				vector<pair<string,string>> temp_param;
				//initialize with dummy arguments
				s1->set_func(0,"",temp_param,-1);
			}

			
		}
#line 2411 "y.tab.c"
    break;

  case 13:
#line 939 "./parser.y"
                {
			if((yyvsp[-3].si)->getName() != "")
				(yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+" "+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			else
				(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+" "+(yyvsp[0].si)->getName(), "NON_TERMINAL");


			print_line();
			fp2<<"parameter_list : parameter_list COMMA type_specifier ID\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

			//add to param list
			param_list.push_back(make_pair((yyvsp[0].si)->getName(),(yyvsp[-1].si)->getName()));
			temp_line = yylineno;

			
		}
#line 2433 "y.tab.c"
    break;

  case 14:
#line 957 "./parser.y"
                {
			if((yyvsp[-2].si)->getName() != "")
				(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			else
				(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(), "NON_TERMINAL");

			print_line();
			fp2<<"parameter_list : parameter_list COMMA type_specifier\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

			//add to param list
			param_list.push_back(make_pair("",(yyvsp[0].si)->getName()));
			temp_line = yylineno;
		}
#line 2452 "y.tab.c"
    break;

  case 15:
#line 972 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+" "+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"parameter_list : type_specifier ID\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

			//add to param_list
			param_list.push_back(make_pair((yyvsp[0].si)->getName(),(yyvsp[-1].si)->getName()));
			temp_line = yylineno;
		}
#line 2467 "y.tab.c"
    break;

  case 16:
#line 983 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"parameter_list : type_specifier\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

			//add to param list
			param_list.push_back(make_pair("",(yyvsp[0].si)->getName()));
			temp_line = yylineno;
		}
#line 2482 "y.tab.c"
    break;

  case 17:
#line 996 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName(), "NON_TERMINAL");
			error_cnt++;

		}
#line 2492 "y.tab.c"
    break;

  case 18:
#line 1003 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName(), "NON_TERMINAL");
			error_cnt++;

		}
#line 2502 "y.tab.c"
    break;

  case 19:
#line 1010 "./parser.y"
                {
			(yyval.si) = new SymbolInfo("", "NON_TERMINAL");
			error_cnt++;

		}
#line 2512 "y.tab.c"
    break;

  case 20:
#line 1021 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+"\n"+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
				//set_code
				(yyval.si)->set_code((yyvsp[-1].si)->get_code());
				print_line();
				fp2<<"compound_statement : LCURL statements RCURL\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;
				exitScope_parser();

				fp2<<(yyval.si)->get_code()<<endl;
			}
#line 2528 "y.tab.c"
    break;

  case 21:
#line 1033 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+" "+(yyvsp[0].si)->getName(), "NON_TERMINAL");
				print_line();
				fp2<<"compound_statement : LCURL RCURL\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;
				exitScope_parser();

				fp2<<(yyval.si)->get_code()<<endl;
			}
#line 2542 "y.tab.c"
    break;

  case 22:
#line 1045 "./parser.y"
                                 {
					 //fp2<<"here in dummy token begin"<<endl;
					 enterScope_parser();
					 //add_function_param();

				 }
#line 2553 "y.tab.c"
    break;

  case 23:
#line 1054 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+" "+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"var_declaration : type_specifier declaration_list SEMICOLON\n"<<endl;
			
			if((yyvsp[-2].si)->getName() == "void")
			{
				error_cnt++;
				error_print_line();
				fp3<<"Variable type cannot be void"<<endl<<endl;
				fp2<<"Variable type cannot be void"<<endl<<endl;
			}

			else
			{
				for(auto x:decl_list)
				{
					cout<<x.first<<" : "<<x.second<<endl;
					
					bool val = insert_ID(x.first,(yyvsp[-2].si)->getName(),x.second);

					if(!val)
					{
						error_cnt++;
						error_print_line();
						fp3<<"Multiple declaration of "<<x.first<<endl<<endl;
						fp2<<"Multiple declaration of "<<x.first<<endl<<endl;
					}
				}
			}
			decl_list.clear();

			fp2<<(yyval.si)->getName()<<endl<<endl;

		}
#line 2593 "y.tab.c"
    break;

  case 24:
#line 1091 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+" "+(yyvsp[-2].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"var_declaration : type_specifier declaration_list SEMICOLON\n"<<endl;
			
			if((yyvsp[-3].si)->getName() == "void")
			{
				error_cnt++;
				error_print_line();
				fp3<<"Variable type cannot be void"<<endl<<endl;
				fp2<<"Variable type cannot be void"<<endl<<endl;
			}

			else
			{
				for(auto x:decl_list)
				{
					cout<<x.first<<" : "<<x.second<<endl;
					
					bool val = insert_ID(x.first,(yyvsp[-3].si)->getName(),x.second);

					if(!val)
					{
						error_cnt++;
						error_print_line();
						fp3<<"Multiple declaration of "<<x.first<<endl<<endl;
						fp2<<"Multiple declaration of "<<x.first<<endl<<endl;
					}
				}
			}
			decl_list.clear();

			fp2<<(yyval.si)->getName()<<endl<<endl;

		}
#line 2633 "y.tab.c"
    break;

  case 25:
#line 1128 "./parser.y"
                {
			(yyval.si) = new SymbolInfo("","NON_TERMINAL");
			error_cnt++;

		}
#line 2643 "y.tab.c"
    break;

  case 26:
#line 1136 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo("int","NON_TERMINAL");
				print_line();
				fp2<<"type_specifier : "<<"INT\n"<<endl;
				fp2<<"int\n"<<endl;
			}
#line 2654 "y.tab.c"
    break;

  case 27:
#line 1143 "./parser.y"
                {
			(yyval.si) = new SymbolInfo("float","NON_TERMINAL");
			print_line();
			fp2<<"type_specifier : "<<"FLOAT\n"<<endl;
			fp2<<"float\n"<<endl;
		}
#line 2665 "y.tab.c"
    break;

  case 28:
#line 1150 "./parser.y"
                {
			(yyval.si) = new SymbolInfo("void","NON_TERMINAL");
			print_line();
			fp2<<"type_specifier : "<<"VOID\n"<<endl;
			fp2<<"void\n"<<endl;
		}
#line 2676 "y.tab.c"
    break;

  case 29:
#line 1159 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
				print_line();
				fp2<<"declaration_list : declaration_list COMMA ID\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;

				decl_list.push_back(make_pair((yyvsp[0].si)->getName(),0));
			}
#line 2689 "y.tab.c"
    break;

  case 30:
#line 1168 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-5].si)->getName()+(yyvsp[-4].si)->getName()+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
				print_line();
				fp2<<"declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;


				//size-> convert string to int
				int size = 0;
				stringstream val((yyvsp[-1].si)->getName());
				val >> size;

				decl_list.push_back(make_pair((yyvsp[-3].si)->getName(),size));
			}
#line 2708 "y.tab.c"
    break;

  case 31:
#line 1183 "./parser.y"
                  {
 		  		(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
 		  		print_line();
				fp2<<"declaration_list : ID\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;

				//decl_list.clear();
				decl_list.push_back(make_pair((yyvsp[0].si)->getName(),0));
 		  }
#line 2722 "y.tab.c"
    break;

  case 32:
#line 1193 "./parser.y"
                  {
			    (yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
 		  		print_line();
				fp2<<"declaration_list : ID LTHIRD CONST_INT RTHIRD\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;

				//size-> convert string to int
				int size = 0;
				stringstream val((yyvsp[-1].si)->getName());
				val >> size;

				//decl_list.clear();
				decl_list.push_back(make_pair((yyvsp[-3].si)->getName(),size));
 		  }
#line 2741 "y.tab.c"
    break;

  case 33:
#line 1209 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
				decl_list.push_back(make_pair((yyvsp[0].si)->getName(),0));
				error_cnt++;
			}
#line 2751 "y.tab.c"
    break;

  case 34:
#line 1221 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName()+"\n","NON_TERMINAL");
				//set_code
				stringstream ss((yyvsp[0].si)->getName());
				string word;
				string temp ="";
				while (ss >> word) {
					temp+=";"+word+"\n";
				}

				(yyval.si)->set_code(temp+"\n"+(yyvsp[0].si)->get_code());
 		  		print_line();
				fp2<<"statements : statement\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl;
				fp2<<(yyval.si)->get_code()<<endl;

				//free the temporary variables used in the statement
				for(string x:current_temp)
				{
					free_var(x);
				}

				current_temp.clear();
 		  	}
#line 2780 "y.tab.c"
    break;

  case 35:
#line 1247 "./parser.y"
           {
		   	(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName()+"\n","NON_TERMINAL");
			//add two code segments
			stringstream ss((yyvsp[0].si)->getName());
			string word;
			string temp ="";
			while (ss >> word) {
				temp+=";"+word+"\n";
			}
			(yyval.si)->set_code((yyvsp[-1].si)->get_code()+temp+"\n"+(yyvsp[0].si)->get_code());
 		  	print_line();
			fp2<<"statements : statements statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

			//free the temporary variables used in the statement
			for(string x:current_temp)
			{
				free_var(x);
			}

			current_temp.clear();

 		}
#line 2809 "y.tab.c"
    break;

  case 36:
#line 1275 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
 		  	print_line();
			fp2<<"statement : var_declaration\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

 		}
#line 2821 "y.tab.c"
    break;

  case 37:
#line 1284 "./parser.y"
           {
		   	(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
			//set_code
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
 		  	print_line();
			fp2<<"statement : expression_statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 2835 "y.tab.c"
    break;

  case 38:
#line 1294 "./parser.y"
          {
		  	(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
			//set_code
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
 		  	print_line();
			fp2<<"statement : compound_statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 2850 "y.tab.c"
    break;

  case 39:
#line 1305 "./parser.y"
          {
			(yyval.si) = new SymbolInfo("for"+(yyvsp[-5].si)->getName()+(yyvsp[-4].si)->getName()+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
 		  	print_line();
			//code
			string label1 = newLabel();
			string label2 = newLabel();

			//initiallize
			(yyval.si)->set_code((yyvsp[-4].si)->get_code());
			//condition check
			(yyval.si)->code+=label1+":\n";
			(yyval.si)->code+=(yyvsp[-3].si)->get_code();
			(yyval.si)->code+="\tCMP "+(yyvsp[-3].si)->get_symbol()+",0\n";
			(yyval.si)->code+="\tJE "+label2+"\n";
			//loop body
			(yyval.si)->code+=(yyvsp[0].si)->get_code();
			//increment
			(yyval.si)->code+=(yyvsp[-2].si)->get_code();
			(yyval.si)->code+="\tJMP "+label1+"\n";
			//exit_loop
			(yyval.si)->code+=label2+":\n";


			fp2<<"statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

 		}
#line 2882 "y.tab.c"
    break;

  case 40:
#line 1333 "./parser.y"
           {
		   	(yyval.si) = new SymbolInfo("if "+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			//code
			(yyval.si)->set_code((yyvsp[-2].si)->get_code());

			string label1 = newLabel();
			string label2 = newLabel();

			(yyval.si)->code+="\tCMP "+(yyvsp[-2].si)->get_symbol()+",1\n";
			(yyval.si)->code+="\tJE "+label1+"\n";
			(yyval.si)->code+="\tJMP "+label2+"\n";
			(yyval.si)->code+=label1+":\n";
			(yyval.si)->code +=(yyvsp[0].si)->get_code();
			(yyval.si)->code+=label2+":\n";


 		  	print_line();
			fp2<<"statement : IF LPAREN expression RPAREN statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl<<endl;

 		}
#line 2909 "y.tab.c"
    break;

  case 41:
#line 1356 "./parser.y"
          {
		  	(yyval.si) = new SymbolInfo("if "+(yyvsp[-5].si)->getName()+(yyvsp[-4].si)->getName()+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+"\nelse\n"+(yyvsp[0].si)->getName(), "NON_TERMINAL");

			//code
			(yyval.si)->set_code((yyvsp[-4].si)->get_code());

			string label1 = newLabel();
			string label2 = newLabel();

			(yyval.si)->code+="\tCMP "+(yyvsp[-4].si)->get_symbol()+",1\n";
			(yyval.si)->code+="\tJE "+label1+"\n";
			(yyval.si)->code +=(yyvsp[0].si)->get_code();
			(yyval.si)->code+="\tJMP "+label2+"\n";
			(yyval.si)->code+=label1+":\n";
			(yyval.si)->code +=(yyvsp[-2].si)->get_code();
			(yyval.si)->code+=label2+":\n";


 		  	print_line();
			fp2<<"statement : IF LPAREN expression RPAREN statement ELSE statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 2938 "y.tab.c"
    break;

  case 42:
#line 1381 "./parser.y"
          {
		  	(yyval.si) = new SymbolInfo("while "+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
 		  	print_line();

			//code
			string label1 = newLabel();
			string label2 = newLabel();

			//condition check
			(yyval.si)->code+=label1+":\n";
			(yyval.si)->code+=(yyvsp[-2].si)->get_code();
			(yyval.si)->code+="\tCMP "+(yyvsp[-2].si)->get_symbol()+",0\n";
			(yyval.si)->code+="\tJE "+label2+"\n";
			//loop body
			(yyval.si)->code+=(yyvsp[0].si)->get_code();
			(yyval.si)->code+="\tJMP "+label1+"\n";
			//exit_loop
			(yyval.si)->code+=label2+":\n";


			fp2<<"statement : WHILE LPAREN expression RPAREN statement\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

 		}
#line 2967 "y.tab.c"
    break;

  case 43:
#line 1406 "./parser.y"
           {
		   	
		   	(yyval.si) = new SymbolInfo("printf"+(yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			SymbolInfo *s = st.Look_up((yyvsp[-2].si)->getName());
			if(s == NULL)
			{
				error_cnt++;
				error_print_line();
				fp3<<"Undeclared variable "<<(yyvsp[-2].si)->getName()<<endl<<endl;
				fp2<<"Undeclared variable "<<(yyvsp[-2].si)->getName()<<endl<<endl;
			}
			//code
			(yyval.si)->code += "\tMOV ax,"+s->get_symbol()+"\n";
			(yyval.si)->code += "\tMOV print_var,ax\n";
			(yyval.si)->code += "\tCALL print\n";
 		  	print_line();
			fp2<<"statement : PRINTLN LPAREN ID RPAREN SEMICOLON\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

 		}
#line 2992 "y.tab.c"
    break;

  case 44:
#line 1427 "./parser.y"
                {
			//fp2<<"RETURN symbol->name: "<<$1->getName()<<endl<<endl;
		  	(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+" "+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			//set_code
			(yyval.si)->set_code((yyvsp[-1].si)->get_code());
			//code
			(yyval.si)->code += "\tMOV ax,"+(yyvsp[-1].si)->get_symbol()+"\n";
			(yyval.si)->code += "\tMOV ret_temp,ax\n";
 		  	print_line();
			fp2<<"statement : RETURN expression SEMICOLON\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;

 		}
#line 3010 "y.tab.c"
    break;

  case 45:
#line 1441 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(), "NON_TERMINAL");
			error_cnt++;
			error_print_line();
			fp2<<"Illegal scoping, function declared inside a function\n"<<endl;
			fp3<<"Illegal scoping, function declared inside a function\n"<<endl;
		}
#line 3022 "y.tab.c"
    break;

  case 46:
#line 1450 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(), "NON_TERMINAL");
			//set_code
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			error_cnt++;
			error_print_line();
			fp2<<"Illegal scoping, function defined inside a function\n"<<endl;
			fp3<<"Illegal scoping, function defined inside a function\n"<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
		}
#line 3037 "y.tab.c"
    break;

  case 47:
#line 1464 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
 		  	print_line();
			fp2<<"expression_statement : SEMICOLON\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3050 "y.tab.c"
    break;

  case 48:
#line 1473 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");
				//set_code
				(yyval.si)->set_code((yyvsp[-1].si)->get_code());
				(yyval.si)->set_data_type((yyvsp[-1].si)->get_data_type());
				(yyval.si)->set_symbol((yyvsp[-1].si)->get_symbol());

	 		  	print_line();
				fp2<<"expression_statement : expression SEMICOLON\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;
				fp2<<(yyval.si)->get_code()<<endl;

 			}
#line 3068 "y.tab.c"
    break;

  case 49:
#line 1489 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo("","NON_TERMINAL");
				error_cnt++;
				//error_print_line();
				//yyerror("syntax error exp");

				//yyerror("invalid expression");
				yyerrok;
				yyclearin;
			}
#line 3083 "y.tab.c"
    break;

  case 50:
#line 1502 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");
				error_cnt++;
			}
#line 3092 "y.tab.c"
    break;

  case 51:
#line 1509 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");

			print_line();
			fp2<<"variable : ID\n"<<endl;

			//check if ID is declared or not
			if(!check_var_declared((yyvsp[0].si)->getName()))
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Undeclared variable "<<(yyvsp[0].si)->getName()<<endl<<endl;
				fp2<<"Undeclared variable "<<(yyvsp[0].si)->getName()<<endl<<endl;
			}

			else
			{
				SymbolInfo *s = st.Look_up((yyvsp[0].si)->getName());
				if(s->get_array())
				{
					mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();
					fp3<<"Type mismatch, "+(yyvsp[0].si)->getName()+" is an array"<<endl<<endl;
					fp2<<"Type mismatch, "+(yyvsp[0].si)->getName()+" is an array"<<endl<<endl;
					(yyval.si)->setType("array");
				}	
			}
			
			
			//set variable data type and symbol according to id's data type
			SymbolInfo* s = st.Look_up((yyvsp[0].si)->getName());
			if(s != NULL)
			{
				//data_type
				set_data_type((yyval.si),s);
				//type
				(yyval.si)->setType(s->getType());
				//symbol
				(yyval.si)->set_symbol(s->get_symbol());

			}
			
			
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3146 "y.tab.c"
    break;

  case 52:
#line 1559 "./parser.y"
                {
			print_line();
			fp2<<"variable : ID LTHIRD expression RTHIRD\n"<<endl;

			//check if ID is declared or not
			if(!check_var_declared((yyvsp[-3].si)->getName()))
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"variable "<<(yyvsp[-3].si)->getName()<<" not declared"<<endl;
				fp2<<"variable "<<(yyvsp[-3].si)->getName()<<" not declared"<<endl;
			}

			else
			{	
				//check is the id actually declared as an array
				SymbolInfo *s = st.Look_up((yyvsp[-3].si)->getName());
				if(!s->get_array())
				{
					mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();
					fp3<<(yyvsp[-3].si)->getName()+" not an array"<<endl<<endl;
					fp2<<(yyvsp[-3].si)->getName()+" not an array"<<endl<<endl;

				}		
			}

			//check array's index is integer type
			//fp2<<$3->get_data_type()<<endl<<endl;
			if((yyvsp[-1].si)->get_data_type() != "int")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Expression inside third brackets not an integer"<<endl<<endl;
				fp2<<"Expression inside third brackets not an integer"<<endl<<endl;
			}
			
			(yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
			//set variable data type according to id's data type
			SymbolInfo* s = st.Look_up((yyvsp[-3].si)->getName());
			if(s != NULL)
			{
				//data_type and symbol
				set_data_type((yyval.si),s);
				(yyval.si)->set_size(s->get_size());
				(yyval.si)->setType("array");

				//index calculation
				//saving the index in bx
				(yyval.si)->code += "\tMOV bx,"+(yyvsp[-1].si)->get_symbol()+"\n";
				//index = index*2
				(yyval.si)->code += "\tADD bx,bx\n";

				(yyval.si)->set_symbol(s->get_symbol());

				/*
				//added code
				string temp = newTemp();
				data_seg.push_back(temp+"dw ?");
				
				$$->code +="\tMOV cx,"+$1->get_symbol()+"\n";
				$$->code +="\tMOV "+temp+",cx[bx]\n";
				$$->set_symbol(temp);
				*/
			}
			
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3223 "y.tab.c"
    break;

  case 53:
#line 1634 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			(yyval.si)->set_size((yyvsp[0].si)->get_size());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"expression : logic expression\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;


 		}
#line 3243 "y.tab.c"
    break;

  case 54:
#line 1650 "./parser.y"
           {
		   	(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");

			//set data type of logic expression
			set_data_type((yyval.si),(yyvsp[-2].si));

			if((yyvsp[0].si)->get_data_type() == "void" | (yyvsp[-2].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			else if((yyvsp[0].si)->get_data_type() == "int" && (yyvsp[-2].si)->get_data_type() == "float")
			{
				//type convert -> no error
			}

			//find error if operands of assignment operations are not compatible
			else if((yyvsp[-2].si)->get_data_type() != (yyvsp[0].si)->get_data_type() && mismatch_map[yylineno] == 0)
			{
				error_cnt++;
				error_print_line();
				fp3<<"Type Mismatch"<<endl<<endl;
				fp2<<"Type Mismatch"<<endl<<endl;
			}

			// fp3<<yylineno<<": ";
			// fp3<<mismatch_map[yylineno]<<endl;
			// fp2<<yylineno<<": ";
			// fp2<<mismatch_map[yylineno]<<endl;

			mismatch_map.clear();

			//add two code segments
			(yyval.si)->set_code((yyvsp[0].si)->get_code()+(yyvsp[-2].si)->get_code());

			//code
			if((yyvsp[-2].si)->getType() == "array")
			{
				(yyval.si)->code += "\tMOV ax,"+(yyvsp[0].si)->get_symbol()+"\n";
				(yyval.si)->code += "\tMOV "+(yyvsp[-2].si)->get_symbol()+"[bx],ax\n";
			}

			else
			{
				(yyval.si)->code += "\tMOV ax,"+(yyvsp[0].si)->get_symbol()+"\n";
				(yyval.si)->code += "\tMOV "+(yyvsp[-2].si)->get_symbol()+",ax\n";
			}
			(yyval.si)->set_symbol((yyvsp[-2].si)->get_symbol());

			print_line();
			fp2<<"expression : variable ASSIGNOP logic_expression\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3307 "y.tab.c"
    break;

  case 55:
#line 1713 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of logic expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			(yyval.si)->set_size((yyvsp[0].si)->get_size());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"logic_expression : rel_expression\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3326 "y.tab.c"
    break;

  case 56:
#line 1728 "./parser.y"
                 {
			(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");
			//add two code segments
			(yyval.si)->set_code((yyvsp[-2].si)->get_code()+(yyvsp[0].si)->get_code());
			//set data type of logic expression
			//set_data_type($$,$1);
			(yyval.si)->set_data_type("int");

			if((yyvsp[-2].si)->get_data_type() == "void" | (yyvsp[0].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			if((yyvsp[-1].si)->getName() == "||")
			{
				string label1 = newLabel();
				string label2 = newLabel();

				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");

				(yyval.si)->code+="\tCMP "+(yyvsp[-2].si)->get_symbol()+",0\n";
				(yyval.si)->code+="\tJNE "+label1+"\n";
				(yyval.si)->code+="\tCMP "+(yyvsp[-2].si)->get_symbol()+",0\n";
				(yyval.si)->code+="\tJNE "+label1+"\n";
				(yyval.si)->code+="\tMOV "+temp+",0\n";
				(yyval.si)->code+="\tJMP "+label2+"\n";
				(yyval.si)->code+=label1+":\n";
				(yyval.si)->code+="\tMOV "+temp+",1\n";
				(yyval.si)->code+=label2+":\n";

				(yyval.si)->set_symbol(temp);
			}

			else if((yyvsp[-1].si)->getName() == "&&")
			{
				string label1 = newLabel();
				string label2 = newLabel();

				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");

				(yyval.si)->code+="\tCMP "+(yyvsp[-2].si)->get_symbol()+",0\n";
				(yyval.si)->code+="\tJE "+label1+"\n";
				(yyval.si)->code+="\tCMP "+(yyvsp[0].si)->get_symbol()+",0\n";
				(yyval.si)->code+="\tJE "+label1+"\n";
				(yyval.si)->code+="\tMOV "+temp+",1\n";
				(yyval.si)->code+="\tJMP "+label2+"\n";
				(yyval.si)->code+=label1+":\n";
				(yyval.si)->code+="\tMOV "+temp+",0\n";
				(yyval.si)->code+=label2+":\n";

				(yyval.si)->set_symbol(temp);
			}

			

 		  	print_line();
			fp2<<"logic_expression : rel_expression LOGICOP rel_expression\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3401 "y.tab.c"
    break;

  case 57:
#line 1801 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of rel expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			(yyval.si)->set_size((yyvsp[0].si)->get_size());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"rel_expression : simple_expression\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3419 "y.tab.c"
    break;

  case 58:
#line 1815 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");
			//add two code segments
			(yyval.si)->set_code((yyvsp[-2].si)->get_code()+(yyvsp[0].si)->get_code());
			//set data type of rel expression
			//set_data_type($$,$1);
			(yyval.si)->set_data_type("int");

			if((yyvsp[-2].si)->get_data_type() == "void" | (yyvsp[0].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");
			string label1 = newLabel();
			string label2 = newLabel();

			(yyval.si)->code+="\tMOV ax,"+(yyvsp[-2].si)->get_symbol()+"\n";
			(yyval.si)->code+="\tCMP ax,"+(yyvsp[0].si)->get_symbol()+"\n";

			if((yyvsp[-1].si)->getName() == "==")
			{
				(yyval.si)->code+="\tJE "+label1+"\n";
			}

			else if((yyvsp[-1].si)->getName() == "<")
			{
				(yyval.si)->code+="\tJL "+label1+"\n";
			}

			else if((yyvsp[-1].si)->getName() == ">")
			{
				(yyval.si)->code+="\tJG "+label1+"\n";
			}

			else if((yyvsp[-1].si)->getName() == "<=")
			{
				(yyval.si)->code+="\tJLE "+label1+"\n";
			}

			else if((yyvsp[-1].si)->getName() == ">=")
			{
				(yyval.si)->code+="\tJGE "+label1+"\n";
			}

			else if((yyvsp[-1].si)->getName() == "!=")
			{
				(yyval.si)->code+="\tJNE "+label1+"\n";
			}

			(yyval.si)->code+="\tMOV cx,0\n";
			(yyval.si)->code+="\tJMP "+label2+"\n";
			(yyval.si)->code+=label1+":\n";
			(yyval.si)->code+="\tMOV cx,1\n";
			(yyval.si)->code+=label2+":\n";
			(yyval.si)->code+="\tMOV "+temp+",cx\n";

			(yyval.si)->set_symbol(temp);

 		  	print_line();
			fp2<<"rel_expression : simple_expression RELOP simple_expression\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3495 "y.tab.c"
    break;

  case 59:
#line 1889 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of simple expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			(yyval.si)->set_size((yyvsp[0].si)->get_size());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"simple_expression : term\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3513 "y.tab.c"
    break;

  case 60:
#line 1903 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");
			//add two code segments
			(yyval.si)->set_code((yyvsp[-2].si)->get_code()+(yyvsp[0].si)->get_code());
			//set data type of simple expression

			if((yyvsp[-2].si)->get_data_type() == "void" | (yyvsp[0].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				(yyval.si)->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//type conversion
			else if((yyvsp[-2].si)->get_data_type() == "float" | (yyvsp[0].si)->get_data_type() == "float")
			{
				(yyval.si)->set_data_type("float");
			}

			else
			{
				(yyval.si)->set_data_type("int");
			}

			//code
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");
			(yyval.si)->code +="\tMOV ax,"+(yyvsp[-2].si)->get_symbol()+"\n";
			
			if((yyvsp[-1].si)->getName() == "+")
			{
				(yyval.si)->code +="\tADD ax,"+(yyvsp[0].si)->get_symbol()+"\n";
			}
			else
			{
				(yyval.si)->code +="\tSUB ax,"+(yyvsp[0].si)->get_symbol()+"\n";
			}

			(yyval.si)->code+="\tMOV "+temp+",ax\n";

			(yyval.si)->set_symbol(temp);


 		  	print_line();
			fp2<<(yyvsp[-1].si)->getName()<<endl;
			//fp2<<$$->code<<endl;
			fp2<<"simple_expression : simple_expression ADDOP term\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3572 "y.tab.c"
    break;

  case 61:
#line 1961 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of term
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			(yyval.si)->set_size((yyvsp[0].si)->get_size());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"term : unary_expression\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3590 "y.tab.c"
    break;

  case 62:
#line 1975 "./parser.y"
        {
		 	(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");

			//adding the code segments together
			(yyval.si)->set_code((yyvsp[-2].si)->get_code()+(yyvsp[0].si)->get_code());

			print_line();
			fp2<<"term : term MULOP unary_expression\n"<<endl;
			
			if((yyvsp[-2].si)->get_data_type() == "void" | (yyvsp[0].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				(yyval.si)->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//type conversion
			else if((yyvsp[-2].si)->get_data_type() == "float" | (yyvsp[0].si)->get_data_type() == "float")
			{
				(yyval.si)->set_data_type("float");
			}

			else
			{
				(yyval.si)->set_data_type("int");
			}

			//check operands of modulus operator
			if((yyvsp[-1].si)->getName() == "/" && (yyvsp[0].si)->getType() == "zero")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Divided by Zero"<<endl<<endl;
				fp2<<"Divided by Zero"<<endl<<endl;
			}

			if((yyvsp[-1].si)->getName() == "%" && (yyvsp[0].si)->getType() == "zero")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Modulus by Zero"<<endl<<endl;
				fp2<<"Modulus by Zero"<<endl<<endl;

				(yyval.si)->set_data_type("int");
			}

			else if((yyvsp[-1].si)->getName() == "%")
			{
				if((yyvsp[-2].si)->get_data_type() != "int" | (yyvsp[0].si)->get_data_type() != "int")
				{
					mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();
					fp3<<"Non-Integer operand on modulus operator"<<endl<<endl;
					fp2<<"Non-Integer operand on modulus operator"<<endl<<endl;
				}

				(yyval.si)->set_data_type("int");
			}

			//code
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");

			if((yyvsp[-1].si)->getName() == "*")
			{
				(yyval.si)->code+="\tMOV ax,"+(yyvsp[-2].si)->get_symbol()+"\n";
				(yyval.si)->code+="\tMOV dx,"+(yyvsp[0].si)->get_symbol()+"\n";
				(yyval.si)->code+="\tMUL dx\n";
				//result lower half is in ax, upper half in dx
				(yyval.si)->code+="\tMOV "+temp+",ax\n";
				
			}

			else
			{
				(yyval.si)->code+="\tMOV ax,"+(yyvsp[-2].si)->get_symbol()+"\n";
				(yyval.si)->code+="\tXOR dx,dx\n";
				(yyval.si)->code+="\tMOV cx,"+(yyvsp[0].si)->get_symbol()+"\n";
				(yyval.si)->code+="\tDIV cx\n";

				if((yyvsp[-1].si)->getName() == "/")
				{
					(yyval.si)->code+="\tMOV "+temp+",ax\n";
				}
				else if((yyvsp[-1].si)->getName() == "%")
				{
					(yyval.si)->code+="\tMOV "+temp+",dx\n";
				}
			}
			(yyval.si)->set_symbol(temp);

			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3696 "y.tab.c"
    break;

  case 63:
#line 2079 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of unary expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			set_data_type((yyval.si),(yyvsp[0].si));		
			(yyval.si)->set_size((yyvsp[0].si)->get_size());

			if((yyvsp[0].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				(yyval.si)->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			if((yyvsp[-1].si)->getName() == "-")
			{
				(yyval.si)->code+="\tNEG "+(yyvsp[0].si)->get_symbol()+"\n";	
			}
			//symbol same as unary_expression
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"unary_expression : ADDOP unary expression\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3732 "y.tab.c"
    break;

  case 64:
#line 2111 "./parser.y"
                 {
			(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of unary expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_size((yyvsp[0].si)->get_size());

			if((yyvsp[0].si)->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				(yyval.si)->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			(yyval.si)->code+="\tNOT "+(yyvsp[0].si)->get_symbol()+"\n";
			//symbol same as unary_expression
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"unary_expression : NOT unary expression\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3765 "y.tab.c"
    break;

  case 65:
#line 2140 "./parser.y"
                 {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			//set data type of unary expression
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			set_data_type((yyval.si),(yyvsp[0].si));
			(yyval.si)->set_size((yyvsp[0].si)->get_size());
			(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());

 		  	print_line();
			fp2<<"unary_expression : factor\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;

 		}
#line 3784 "y.tab.c"
    break;

  case 66:
#line 2157 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			//set data type of factor
			set_data_type((yyval.si),(yyvsp[0].si));
			
			//the symbol is same as variable symbol
			if((yyvsp[0].si)->getType() == "array")
			{
				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");
				(yyval.si)->set_size((yyvsp[0].si)->get_size());

				(yyval.si)->code+= "\tMOV ax,"+(yyvsp[0].si)->get_symbol()+"[bx]\n";
				(yyval.si)->code+= "\tMOV "+temp+",ax\n";
				(yyval.si)->set_symbol(temp);
			}

			else
			{
				(yyval.si)->set_symbol((yyvsp[0].si)->get_symbol());
			}

 		  	print_line();
			fp2<<"factor : variable\n"<<endl;
			fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
			fp2<<(yyval.si)->get_code()<<endl;
 		}
#line 3818 "y.tab.c"
    break;

  case 67:
#line 2187 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[-3].si)->getName()+(yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
		//set_code
		(yyval.si)->set_code((yyvsp[-1].si)->get_code());
		print_line();
		fp2<<"factor : ID LPAREN argument_list RPAREN\n"<<endl;

		
		//check if declared
		SymbolInfo* s = st.Look_up((yyvsp[-3].si)->getName());
		if(s == NULL)
		{
			mismatch_map[yylineno]++;
			error_cnt++;
			error_print_line();
			fp3<<"Undeclared function "<<(yyvsp[-3].si)->getName()<<endl<<endl;
			fp2<<"Undeclared function "<<(yyvsp[-3].si)->getName()<<endl<<endl;
		}

		else //is declared
		{
			func_param* f = s->get_func();

			//check if id is really a function
			if(f == NULL)
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<(yyvsp[-3].si)->getName() + " is not a function"<<endl<<endl;
				fp2<<(yyvsp[-3].si)->getName() + " is not a function"<<endl<<endl;
			}

			else //is a function
			{
				//set type according to return type
				(yyval.si)->set_data_type(f->getReturn_type());

				//check if defined
				if(f->get_flag() == 0)
				{
					//mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();

					fp3<<(yyvsp[-3].si)->getName() + " function declared but not defined"<<endl<<endl;
					fp2<<(yyvsp[-3].si)->getName() + " function declared but not defined"<<endl<<endl;

				}

				else if(f->get_flag() == 1)//function defined
				{
					//check if arguments are consistent
					
					//number of arguments
					if((int)arg_list.size() != f->getNumber_of_param())
					{
						//mismatch_map[yylineno]++;
						error_cnt++;
						error_print_line();
						fp3<<"Total number of arguments mismatch in function "+(yyvsp[-3].si)->getName()<<endl<<endl;
						fp2<<"Total number of arguments mismatch in function "+(yyvsp[-3].si)->getName()<<endl<<endl;
					}

					else //data types of arguments
					{
						//fp2<<"here"<<endl;
						vector<pair<string,string>> p_list = f->getParam_list();

						int i = 0;
						for(string x:arg_list)
						{
							//fp2<<x<<" : "<<p_list[i].second<<endl;
							if(x != p_list[i].second)
							{
								if(x != "array")
								{
									//mismatch_map[yylineno]++;
									error_cnt++;
									error_print_line();
									fp3<<i+1<<"th argument mismatch in function " + (yyvsp[-3].si)->getName()<<endl<<endl;
									fp2<<i+1<<"th argument mismatch in function " + (yyvsp[-3].si)->getName()<<endl<<endl;
									break;
								}
							}
							i++;
						}

					}

				}

				else
				{
					//check if arguments are consistent
					
					//number of arguments
					if((int)arg_list.size() != (int)param_list.size())
					{
						//mismatch_map[yylineno]++;
						error_cnt++;
						error_print_line();
						fp3<<"Total number of arguments mismatch in function "+(yyvsp[-3].si)->getName()<<endl<<endl;
						fp2<<"Total number of arguments mismatch in function "+(yyvsp[-3].si)->getName()<<endl<<endl;
					}

					else //data types of arguments
					{
						//fp2<<"here"<<endl;
						vector<pair<string,string>> p_list = param_list;

						int i = 0;
						for(string x:arg_list)
						{
							//fp2<<x<<" : "<<p_list[i].second<<endl;
							if(x != p_list[i].second)
							{
								if(x != "array")
								{
									//mismatch_map[yylineno]++;
									error_cnt++;
									error_print_line();
									fp3<<i+1<<"th argument mismatch in function " + (yyvsp[-3].si)->getName()<<endl<<endl;
									fp2<<i+1<<"th argument mismatch in function " + (yyvsp[-3].si)->getName()<<endl<<endl;
									break;
								}
							}
							i++;
						}
					}

				}

							
			}

			//code
			vector<string> para_sym_list = f->get_param_symbol();
			for(string x:arg_symbol)
			{
				(yyval.si)->code+="\tPUSH "+x+"\n";
				
			}
			(yyval.si)->code+="\tCALL "+(yyvsp[-3].si)->getName()+"\n";

			if(f->getReturn_type() != "void")
			{
				//$$->code+="\tPOP ret_temp\n";
				(yyval.si)->code+="\tMOV ax,ret_temp\n";

				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");
				(yyval.si)->code+="\tMOV "+temp+",ax\n";

				(yyval.si)->set_symbol(temp);
			}

			/*
			for(int i=(int)(arg_symbol.size()-1);i>=0;i--)
			{
				$$->code+="\tPOP "+arg_symbol[i]+"\n";
			}
			*/
		}

		arg_list.clear();
		arg_symbol.clear();
 	  	
		fp2<<(yyval.si)->getName()<<endl;
		fp2<<(yyval.si)->get_code()<<endl;
 	}
#line 3995 "y.tab.c"
    break;

  case 68:
#line 2360 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), (yyvsp[-1].si)->getType());
		(yyval.si)->set_code((yyvsp[-1].si)->get_code());

		set_data_type((yyval.si),(yyvsp[-1].si));
		//set symbol same as the expression symbol
		(yyval.si)->set_symbol((yyvsp[-1].si)->get_symbol());
 		print_line();
		fp2<<"factor : LPAREN expression RPAREN\n"<<endl;
		fp2<<(yyval.si)->get_code()<<endl;
		fp2<<(yyval.si)->getName()<<endl<<endl;

	}
#line 4013 "y.tab.c"
    break;

  case 69:
#line 2374 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
		(yyval.si)->set_data_type("int");

		if((yyvsp[0].si)->getName() == "0")
		{
			(yyval.si)->setType("zero");
		}

		//the symbol is same as value
		(yyval.si)->set_symbol((yyvsp[0].si)->getName());

 		print_line();
		fp2<<"factor : CONST_INT\n"<<endl;
		
		fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
	}
#line 4035 "y.tab.c"
    break;

  case 70:
#line 2392 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
		(yyval.si)->set_data_type("float");

		//the symbol is same as value
		(yyval.si)->set_symbol((yyvsp[0].si)->getName());

	  	print_line();
		fp2<<"factor : CONST_FLOAT\n"<<endl;
		
		fp2<<(yyvsp[0].si)->getName()<<endl<<endl;
 	}
#line 4052 "y.tab.c"
    break;

  case 71:
#line 2405 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),(yyvsp[-1].si)->getType());
		(yyval.si)->set_code((yyvsp[-1].si)->get_code());
		//set data type of factor
		set_data_type((yyval.si),(yyvsp[-1].si));

		if((yyvsp[-1].si)->getType() == "array")
		{
			string temp = newTemp();
			current_temp.push_back(temp);
			
			//data_seg.push_back(temp+" dw ?");

			(yyval.si)->code += "\tMOV ax,"+(yyvsp[-1].si)->get_symbol()+"[bx]\n";
			(yyval.si)->code += "\tMOV "+temp+",ax\n";
			(yyval.si)->code += "\tINC "+(yyvsp[-1].si)->get_symbol()+"[bx]\n";

			(yyval.si)->set_symbol(temp);
		}

		else
		{
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");
			
			(yyval.si)->code += "\tMOV ax,"+(yyvsp[-1].si)->get_symbol()+"\n";
			(yyval.si)->code += "\tMOV "+temp+",ax\n";
			(yyval.si)->code += "\tINC "+(yyvsp[-1].si)->get_symbol()+"\n";

			(yyval.si)->set_symbol(temp);
		}

 	  	print_line();
		fp2<<"factor : variable INCOP\n"<<endl;
		fp2<<(yyval.si)->get_code()<<endl;
		fp2<<(yyval.si)->getName()<<endl<<endl;
 	}
#line 4095 "y.tab.c"
    break;

  case 72:
#line 2444 "./parser.y"
        {
		(yyval.si) = new SymbolInfo((yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(),"NON_TERMINAL");
		(yyval.si)->set_code((yyvsp[-1].si)->get_code());
		//set data type of factor
		set_data_type((yyval.si),(yyvsp[-1].si));

		if((yyvsp[-1].si)->getType() == "array")
		{
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");

			(yyval.si)->code += "\tMOV ax,"+(yyvsp[-1].si)->get_symbol()+"[bx]\n";
			(yyval.si)->code += "\tMOV "+temp+",ax\n";
			(yyval.si)->code += "\tDEC "+(yyvsp[-1].si)->get_symbol()+"[bx]\n";

			(yyval.si)->set_symbol(temp);
		}

		else
		{
			string temp = newTemp();
			//data_seg.push_back(temp+" dw ?");
			current_temp.push_back(temp);
			
			(yyval.si)->code += "\tMOV ax,"+(yyvsp[-1].si)->get_symbol()+"\n";
			(yyval.si)->code += "\tMOV "+temp+",ax\n";
			(yyval.si)->code += "\tDEC "+(yyvsp[-1].si)->get_symbol()+"\n";

			(yyval.si)->set_symbol(temp);
		}

 	  	print_line();
		fp2<<"factor : variable DECOP\n"<<endl;
		fp2<<(yyval.si)->get_code()<<endl;
		fp2<<(yyval.si)->getName()<<endl<<endl;
 	}
#line 4137 "y.tab.c"
    break;

  case 73:
#line 2484 "./parser.y"
                {
			(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
 		  	print_line();
			//set_code
			(yyval.si)->set_code((yyvsp[0].si)->get_code());
			fp2<<"argument_list : arguments\n"<<endl;
			fp2<<(yyval.si)->getName()<<endl;
 		}
#line 4150 "y.tab.c"
    break;

  case 74:
#line 2493 "./parser.y"
                {
			(yyval.si) = new SymbolInfo("", "NON_TERMINAL");
 		  	print_line();
			fp2<<"argument_list : \n"<<endl;
			fp2<<(yyval.si)->getName()<<endl;
 		}
#line 4161 "y.tab.c"
    break;

  case 75:
#line 2502 "./parser.y"
                        {
				(yyval.si) = new SymbolInfo((yyvsp[-2].si)->getName()+(yyvsp[-1].si)->getName()+(yyvsp[0].si)->getName(), "NON_TERMINAL");
				//set_code
				(yyval.si)->set_code((yyvsp[-2].si)->get_code()+(yyvsp[0].si)->get_code());
				print_line();
				fp2<<"arguments : arguments COMMA logic_expression\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;

				if((yyvsp[0].si)->getType() != "array")
					arg_list.push_back((yyvsp[0].si)->get_data_type());
				else
					arg_list.push_back("array");

				arg_symbol.push_back((yyvsp[0].si)->get_symbol());
 			}
#line 4181 "y.tab.c"
    break;

  case 76:
#line 2518 "./parser.y"
                {
				(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),"NON_TERMINAL");
				//set_code
				(yyval.si)->set_code((yyvsp[0].si)->get_code());
				print_line();
				fp2<<"arguments : logic_expression\n"<<endl;
				fp2<<(yyval.si)->getName()<<endl<<endl;

				if((yyvsp[0].si)->getType() != "array")
					arg_list.push_back((yyvsp[0].si)->get_data_type());
				else
					arg_list.push_back("array");

				arg_symbol.push_back((yyvsp[0].si)->get_symbol());
 			}
#line 4201 "y.tab.c"
    break;

  case 77:
#line 2536 "./parser.y"
        {		
		(yyval.si) = new SymbolInfo((yyvsp[0].si)->getName(),(yyvsp[0].si)->getType());	
		temp_id = (yyvsp[0].si)->getName();
		//print_line();
		//fp2<<"id: ID"<<endl;
	}
#line 4212 "y.tab.c"
    break;


#line 4216 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 2546 "./parser.y"

int main(int argc,char *argv[])
{
	FILE *fp;

	if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	yyin=fp;
	
	fp2.open(argv[2]);
	fp3.open(argv[3]);
	cout<<argv[2]<<endl;

	yylineno = 1;

	fp4.open("code.asm");
	fp5.open("optimized_code.asm");
	
	yyparse();	

	fp3<<"Total Errors: "<<error_cnt<<endl;

	st.Print_all(fp2);

	fp2<<endl;

	fp2<<"Total lines: "<<yylineno<<endl;
	fp2<<"Total errors: "<<error_cnt<<endl;

	fclose(yyin);
	
	return 0;
}
