%option noyywrap yylineno

%x single_comment
%x multi_comment

%{

#include<cstdlib>
#include<iostream>
#include<fstream>

#include "symbol_table.h"


#include "y.tab.h"


using namespace std;

void yyerror(char*);  // yyerror() function for generating error message
extern YYSTYPE yylval;  
extern SymbolTable st;   // SymbolTable variable referencing the SymbolTable object from  parser

//output to token file
ofstream tok;
//number of errors
extern ofstream fp2;
extern ofstream fp3;


long long int error_cnt = 0;
//tracking string and comments
string temp = "";
long long int line_begin = 0;
extern void enterScope_parser();
extern void exitScope_parser();
extern void error_print_line();



%}

AlphaNumeric [a-zA-Z0-9]
digit [0-9]
letter [A-Za-z]
ws [ \t\n\f\v\r]
others [^A-Za-z0-9]
all ([^A-Z]|[A-Z])

%%
{ws}+	{}

if		{return IF;}
else	{return ELSE;}
for		{return FOR;}
while	{return WHILE;}
do		{return DO;}
int		{
			
			return INT;
		}
char	{return CHAR;}
float	{
			
			return FLOAT;
		}
double	{return DOUBLE;}
void	{
			
			return VOID;
		}
return	{
				SymbolInfo* si = new SymbolInfo(yytext,"RETURN");
			 	yylval.si = si; 
				return RETURN;
		}
continue {return CONTINUE;}
printf {return PRINTLN;}
println {return PRINTLN;}


{digit}+	 {
			 SymbolInfo* si = new SymbolInfo(yytext,"CONST_INT");
			 yylval.si = si; 
			 return CONST_INT;
			 }
{digit}+(\.{digit}+)?([Ee][+-]?{digit}+)?	 {
  											 SymbolInfo* si = new SymbolInfo(yytext,"CONST_FLOAT");
			 								 yylval.si = si; 
			 								 return CONST_FLOAT;
			 								 }
([\'][\\][tnafrbv0\'\"\\][\'])|([\'][^\'\"\\\n][\']) 	 {
														 SymbolInfo* si = new SymbolInfo(yytext,"CONST_CHAR");
			 											 yylval.si = si; 
														 return CONST_CHAR;
														 }


[+-]	 {
		 SymbolInfo* si = new SymbolInfo(yytext,"ADDOP");
		 yylval.si = si; 
    	 return ADDOP;
    	 }
    	 
[*/%]	 {
		 SymbolInfo* si = new SymbolInfo(yytext,"MULOP");
		 yylval.si = si; 
    	 return MULOP;
    	 }
    	 
"++"	    {
			SymbolInfo* si = new SymbolInfo(yytext,"INCOP");
		 	yylval.si = si; 
    	 	return INCOP;
    	 	}
    	 	
"--" 		{
			SymbolInfo* si = new SymbolInfo(yytext,"DECOP");
		 	yylval.si = si; 
    	 	return DECOP;
    	 	}
([<>](=)?)|([=!](=))	{
						SymbolInfo* si = new SymbolInfo(yytext,"RELOP");
					 	yylval.si = si;
					 	return RELOP;
					 	}
"="			{
			SymbolInfo* si = new SymbolInfo(yytext,"ASSIGNOP");
		 	yylval.si = si;
    	 	return ASSIGNOP;
    	 	}
("&&"|"||")	{
			SymbolInfo* si = new SymbolInfo(yytext,"LOGICOP");
		 	yylval.si = si;
    	 	return LOGICOP;
    	 	}
"!"			{
			SymbolInfo* si = new SymbolInfo(yytext,"NOT");
		 	yylval.si = si;
    	 	return NOT;
    	 	}
"(" 		{
			SymbolInfo* si = new SymbolInfo(yytext,"LPAREN");
		 	yylval.si = si;
    	 	return LPAREN;
    	 	}
")" 		{
			SymbolInfo* si = new SymbolInfo(yytext,"RPAREN");
		 	yylval.si = si; 
    	 	return RPAREN;
    	 	}
"{"			{
			SymbolInfo* si = new SymbolInfo(yytext,"LCURL");
		 	yylval.si = si;
		 	//enter scope
			//enterScope_parser();
    	 	return LCURL;
    	 	}
	 
"}"         {
			SymbolInfo* si = new SymbolInfo(yytext,"RCURL");
		 	yylval.si = si;
		 	//st.exit_scope();
			//exitScope_parser();
    	 	return RCURL;
		    } 
"[" 		{
			SymbolInfo* si = new SymbolInfo(yytext,"LTHIRD");
		 	yylval.si = si;
    	 	return LTHIRD;
    	 	}
"]" 		{
			SymbolInfo* si = new SymbolInfo(yytext,"RTHIRD");
		 	yylval.si = si;
    	 	return RTHIRD;
    	 	}
","			{
			SymbolInfo* si = new SymbolInfo(yytext,"COMMA");
		 	yylval.si = si;
    	 	return COMMA;
    	 	}
";"			{
			SymbolInfo* si = new SymbolInfo(yytext,"SEMICOLON");
		 	yylval.si = si;
    	 	return SEMICOLON;
    	 	}



({letter}|(_))({AlphaNumeric}|(_))*	{
									cout<<"here in lex ID"<<endl;
									SymbolInfo* si = new SymbolInfo(yytext,"ID");
		 							yylval.si = si; 
    	 							return ID;
    	 							}

{digit}+[.]{digit}*([.]{digit}*)+([Ee]{digit}+)?		{error_print_line();error_cnt++;
										fp3<<"Too many decimal points "<<yytext<<endl<<endl;
										fp2<<"Too many decimal points "<<yytext<<endl<<endl;
										SymbolInfo* si = new SymbolInfo(yytext,"CONST_FLOAT");
			 							yylval.si = si; 
										return CONST_FLOAT;
										}
({digit}+([.]{digit}*)*[Ee]({digit})*([.]{digit}*)+)|({digit}+[.])	{error_print_line();error_cnt++;
										fp3<<"Ill formed number "<<yytext<<endl<<endl;
										fp2<<"Ill formed number "<<yytext<<endl<<endl;
										SymbolInfo* si = new SymbolInfo(yytext,"CONST_FLOAT");
			 							yylval.si = si; 
										return CONST_FLOAT;}
{digit}+([.]{digit}+)?({letter}|(_))({AlphaNumeric}|(_))*			{error_print_line();error_cnt++;
											fp3<<
											"Invalid prefix on ID or invalid suffix on Number ";
									 		fp3<<yytext<<endl<<endl;
											fp2<<
											"Invalid prefix on ID or invalid suffix on Number ";
									 		fp2<<yytext<<endl<<endl;
											SymbolInfo* si = new SymbolInfo(yytext,"CONST_FLOAT");
			 								yylval.si = si; 
											return ID;}	



([/][/])			{BEGIN single_comment;yymore();line_begin = yylineno;}	

<single_comment>[^\\\n(\r\n)] {yymore();}
<single_comment>[\\][\n]	{yymore();}
<single_comment>[\\][\r][\n]	{yymore();}
<single_comment>[\\]	{yymore();}
<single_comment>[\n]	{BEGIN INITIAL;}	
<single_comment>([\r][\n])	{BEGIN INITIAL;}



([/][*])			{yymore();BEGIN multi_comment;line_begin = yylineno;temp=yytext;}

<multi_comment>[*][/] {yymore();BEGIN INITIAL;temp=yytext;}
<multi_comment>[\n]	{yymore();temp=yytext;}
<multi_comment>([\r][\n])	{yymore();temp=yytext;}
<multi_comment><<EOF>>	{error_cnt++;
						 string s = temp;
						 s.erase(s.end()-1);
						 temp="";
						 BEGIN INITIAL;
						 }
<multi_comment>.	{yymore();}

						
. 		{error_print_line();error_cnt++;
		fp3<<"Unrecognized character "<<yytext<<endl<<endl;
		fp2<<"Unrecognized character "<<yytext<<endl<<endl;
		SymbolInfo* si = new SymbolInfo(yytext,"SEMICOLON");
		yylval.si = si;
     	return SEMICOLON;}
									 


%%

