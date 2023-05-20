%{
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



%}


%union{int ival;SymbolInfo* si;}

%token <si> CONST_INT CONST_FLOAT CONST_CHAR ID
%token <si> INT FLOAT VOID CHAR DOUBLE IF ELSE FOR WHILE RETURN PRINTLN CONTINUE DO
%token <si> ASSIGNOP NOT INCOP DECOP LOGICOP RELOP ADDOP MULOP
%token <si> LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON 

%type <si> type_specifier declaration_list var_declaration variable
%type <si> id
%type <si> factor unary_expression term simple_expression rel_expression logic_expression expression expression_statement
%type <si> arguments argument_list
%type <si> statement statements compound_statement
%type <si> parameter_list func_definition func_declaration
%type <si> start program unit
%type <si> dummy_token_begin func_begin

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE



%%

start: program
	{
		$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
		print_line();
		fp2<<"start : program\n"<<endl;
		//fp2<<$$->getName()<<endl<<endl;

		if(error_cnt == 0) // generate code
		{	
			init();
			fp4<<$1->code<<endl;
			optimize_code($1->code);
		}
		// else blank code.asm

		fp2<<$$->get_code()<<endl;
	}
	;

program: program unit 
		{
			$$ = new SymbolInfo($1->getName()+$2->getName(), "NON_TERMINAL");
			//add two code segments
			$$->set_code($1->get_code()+$2->get_code());
			print_line();
			fp2<<"program : program unit\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
		}
	| unit
		{
			$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
			//set_code
			$$->set_code($1->get_code());
			print_line();
			fp2<<"program : unit\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
		}
	;
	
unit: var_declaration
		{
			$$ = new SymbolInfo($1->getName()+"\n", "NON_TERMINAL");
			print_line();
			fp2<<"unit : var_declaration\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
		}
     | func_declaration
     	{	
			$$ = new SymbolInfo($1->getName()+"\n", "NON_TERMINAL");
			print_line();
			fp2<<"unit : func_declaration\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
		}
     | func_definition
     	{
			$$ = new SymbolInfo($1->getName()+"\n", "NON_TERMINAL");
			//set code
			$$->set_code($1->get_code());
		 	print_line();
			fp2<<"unit : func_definition\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
		}

     ;
     
func_declaration: type_specifier id func_begin LPAREN parameter_list RPAREN SEMICOLON
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$4->getName()+$5->getName()+$6->getName()+$7->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON\n"<<endl;
			

			SymbolInfo* s = st.Look_up($2->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == 1)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				}

				else if(f->get_flag() == 0)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				}

				else if(f->get_flag() == -1)
				{
					//set declaration
					s->set_func((int)param_list.size(),$1->getName(),param_list,0);
				}
			}

			else
			{
				error_cnt++;
				fp3<<"Error at line "<<temp_line<<": ";
				fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;

			}
			
			//clear the parameter list
			param_list.clear();

			fp2<<$$->getName()<<endl<<endl;
		}
		| type_specifier id func_begin LPAREN RPAREN SEMICOLON
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$4->getName()+$5->getName()+$6->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON\n"<<endl;
			

			SymbolInfo* s = st.Look_up($2->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == 1)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				}

				else if(f->get_flag() == 0)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
					fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				}

				else if(f->get_flag() == -1)
				{
					//set declaration
					s->set_func((int)param_list.size(),$1->getName(),param_list,0);
					f->set_flag(0);
				}
			}

			else
			{
				error_cnt++;
				fp3<<"Error at line "<<temp_line<<": ";
				fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
			}
			
			
			//clear the parameter list
			param_list.clear();

			fp2<<$$->getName()<<endl<<endl;
			
		}
		;
		 
func_definition: type_specifier id func_begin LPAREN parameter_list RPAREN compound_statement
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$4->getName()+$5->getName()+$6->getName()+$7->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement\n"<<endl;
			

			SymbolInfo* s = st.Look_up($2->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == -1)
				{
					s->set_func((int)param_list.size(),$1->getName(),param_list,1);
					f->set_flag(1);
				}

				else if(f->get_flag() == 1)
				{
					error_cnt++;
					fp3<<"Error at line "<<temp_line<<": ";
					fp3<<"Multiple definition of " <<$2->getName()<<endl<<endl;
					fp2<<"Multiple definition of " <<$2->getName()<<endl<<endl;
				}

				else if(f->get_flag() == 0)// declared
				{
					f->set_flag(1);
					if($1->getName() != f->getReturn_type())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp2<<"Error at line "<<temp_line<<": ";
						fp3<<"Return type mismatch with function declaration in function "<<$2->getName()<<endl<<endl;
						fp2<<"Return type mismatch with function declaration in function "<<$2->getName()<<endl<<endl;
					}

					else if((int)param_list.size() != f->getNumber_of_param())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp2<<"Error at line "<<temp_line<<": ";
						fp3<<"Total number of arguments mismatch with declaration in function "<<$2->getName()<<endl<<endl;
						fp2<<"Total number of arguments mismatch with declaration in function "<<$2->getName()<<endl<<endl;
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
								fp3<<i+1<<"th argument mismatch in function "<<$2->getName()<<endl<<endl;
								fp2<<"Error at line "<<temp_line<<": ";
								fp2<<i+1<<"th argument mismatch in function "<<$2->getName()<<endl<<endl;
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
				fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
			}	

			//code
			if(f!=NULL)
			{
				f->set_param_symbol(param_symbol);

				$$->code +=$2->getName()+" PROC\n";
				$$->code +="\tPUSH ax\n";
				$$->code +="\tPUSH bx\n";
				$$->code +="\tPUSH cx\n";
				$$->code +="\tPUSH dx\n";
				$$->code +="\tPUSH bp\n";
				$$->code +="\tMOV bp,sp\n";
				//function body start
				$$->code += $7->get_code();
				//end
				$$->code +="\tPOP bp\n";
				$$->code +="\tPOP dx\n";
				$$->code +="\tPOP cx\n";
				$$->code +="\tPOP bx\n";
				$$->code +="\tPOP ax\n";

				/*
				if($1->getName() != "void")
				{
					$$->code +="\tPUSH ret_temp\n";
				}
				*/
				$$->code +="\tRET 2\n";
				$$->code +=$2->getName()+" endp\n";
			}	
		
				
			param_list.clear();
			param_symbol.clear();

			fp2<<$$->getName()<<endl<<endl;

		}
		| type_specifier id func_begin LPAREN RPAREN compound_statement
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$4->getName()+$5->getName()+$6->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"func_definition : type_specifier ID LPAREN RPAREN compound_statement\n"<<endl;
			

			SymbolInfo* s = st.Look_up($2->getName());
			func_param* f = s->get_func();

			if(f != NULL)
			{
				if(f->get_flag() == -1)
				{
					s->set_func((int)param_list.size(),$1->getName(),param_list,1);
				}

				else if(f->get_flag() == 0)// declared
				{
					f->set_flag(1);
					if($1->getName() != f->getReturn_type())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp3<<"Return type mismatch with function declaration in function "<<$2->getName()<<endl<<endl;
						fp2<<"Error at line "<<temp_line<<": ";
						fp2<<"Return type mismatch with function declaration in function "<<$2->getName()<<endl<<endl;
					}

					else if((int)param_list.size() != f->getNumber_of_param())
					{
						error_cnt++;
						fp3<<"Error at line "<<temp_line<<": ";
						fp3<<"Total number of arguments mismatch with declaration in function "<<$2->getName()<<endl<<endl;
						fp2<<"Error at line "<<temp_line<<": ";
						fp2<<"Total number of arguments mismatch with declaration in function "<<$2->getName()<<endl<<endl;
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
								fp3<<i+1<<"th argument mismatch in function "<<$2->getName()<<endl<<endl;
								fp2<<"Error at line "<<temp_line<<": ";
								fp2<<i+1<<"th argument mismatch in function "<<$2->getName()<<endl<<endl;
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
				fp3<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
				fp2<<"Multiple declaration of " <<$2->getName()<<endl<<endl;
			}

			vector<string> empty_list;
			f->set_param_symbol(empty_list);	

			//code
			if($2->getName() == "main")
			{
				$$->code +="main proc\n";
				$$->code+="\tmov ax,@data\n";
				$$->code+="\tmov ds,ax\n\n\n";
				$$->code+=$6->get_code();
				$$->code+="\tMOV AH,4CH\n";
				$$->code+="\tINT 21H\n";
				$$->code+="ENDP main\n";
				$$->code+="END main";
			}

			

			//code
			if(f!=NULL && $2->getName() != "main")
			{
				f->set_param_symbol(param_symbol);

				$$->code +=$2->getName()+" PROC\n";
				$$->code +="\tPUSH ax\n";
				$$->code +="\tPUSH bx\n";
				$$->code +="\tPUSH cx\n";
				$$->code +="\tPUSH dx\n";
				$$->code +="\tPUSH bp\n";
				$$->code +="\tMOV bp,sp\n";
				//function body start
				$$->code += $6->get_code();
				//end
				$$->code +="\tPOP bp\n";
				$$->code +="\tPOP dx\n";
				$$->code +="\tPOP cx\n";
				$$->code +="\tPOP bx\n";
				$$->code +="\tPOP ax\n";
				/*if($1->getName() != "void")
				{
					$$->code +="\tPUSH ret_temp\n";
				}*/
				$$->code +="\tRET 2\n";
				$$->code +="ENDP "+$2->getName()+"\n";
			}	
			
					
			param_list.clear();
			param_symbol.clear();

			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

		}

		
 		;	
		

func_begin:
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
		;

parameter_list: parameter_list COMMA type_specifier id
		{
			if($1->getName() != "")
				$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName()+" "+$4->getName(), "NON_TERMINAL");
			else
				$$ = new SymbolInfo($3->getName()+" "+$4->getName(), "NON_TERMINAL");


			print_line();
			fp2<<"parameter_list : parameter_list COMMA type_specifier ID\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

			//add to param list
			param_list.push_back(make_pair($4->getName(),$3->getName()));
			temp_line = yylineno;

			
		}
		| parameter_list COMMA type_specifier
		{
			if($1->getName() != "")
				$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "NON_TERMINAL");
			else
				$$ = new SymbolInfo($3->getName(), "NON_TERMINAL");

			print_line();
			fp2<<"parameter_list : parameter_list COMMA type_specifier\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

			//add to param list
			param_list.push_back(make_pair("",$3->getName()));
			temp_line = yylineno;
		}
 		| type_specifier id
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"parameter_list : type_specifier ID\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

			//add to param_list
			param_list.push_back(make_pair($2->getName(),$1->getName()));
			temp_line = yylineno;
		}
		| type_specifier
		{
			$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"parameter_list : type_specifier\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

			//add to param list
			param_list.push_back(make_pair("",$1->getName()));
			temp_line = yylineno;
		}

		| parameter_list COMMA error

		{
			$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
			error_cnt++;

		}

		| parameter_list error
		{
			$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
			error_cnt++;

		}

		|error
		{
			$$ = new SymbolInfo("", "NON_TERMINAL");
			error_cnt++;

		}
		
 		;


 		
compound_statement: LCURL dummy_token_begin statements RCURL
			{
				$$ = new SymbolInfo($1->getName()+"\n"+$3->getName()+$4->getName(), "NON_TERMINAL");
				//set_code
				$$->set_code($3->get_code());
				print_line();
				fp2<<"compound_statement : LCURL statements RCURL\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;
				exitScope_parser();

				fp2<<$$->get_code()<<endl;
			}
 		    | LCURL dummy_token_begin RCURL
			{
				$$ = new SymbolInfo($1->getName()+" "+$3->getName(), "NON_TERMINAL");
				print_line();
				fp2<<"compound_statement : LCURL RCURL\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;
				exitScope_parser();

				fp2<<$$->get_code()<<endl;
			}
 		    ;

dummy_token_begin:
				 {
					 //fp2<<"here in dummy token begin"<<endl;
					 enterScope_parser();
					 //add_function_param();

				 }
				 ;
 		    
var_declaration: type_specifier declaration_list SEMICOLON
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$3->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"var_declaration : type_specifier declaration_list SEMICOLON\n"<<endl;
			
			if($1->getName() == "void")
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
					
					bool val = insert_ID(x.first,$1->getName(),x.second);

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

			fp2<<$$->getName()<<endl<<endl;

		}

		| type_specifier declaration_list error SEMICOLON
		{
			$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$4->getName(), "NON_TERMINAL");
			print_line();
			fp2<<"var_declaration : type_specifier declaration_list SEMICOLON\n"<<endl;
			
			if($1->getName() == "void")
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
					
					bool val = insert_ID(x.first,$1->getName(),x.second);

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

			fp2<<$$->getName()<<endl<<endl;

		}

		| type_specifier error SEMICOLON
		{
			$$ = new SymbolInfo("","NON_TERMINAL");
			error_cnt++;

		}
 		;
 		 
type_specifier: INT
			{
				$$ = new SymbolInfo("int","NON_TERMINAL");
				print_line();
				fp2<<"type_specifier : "<<"INT\n"<<endl;
				fp2<<"int\n"<<endl;
			}
 		| FLOAT
 		{
			$$ = new SymbolInfo("float","NON_TERMINAL");
			print_line();
			fp2<<"type_specifier : "<<"FLOAT\n"<<endl;
			fp2<<"float\n"<<endl;
		}
 		| VOID
 		{
			$$ = new SymbolInfo("void","NON_TERMINAL");
			print_line();
			fp2<<"type_specifier : "<<"VOID\n"<<endl;
			fp2<<"void\n"<<endl;
		}
 		;
 		
declaration_list: declaration_list COMMA id
			{
				$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "NON_TERMINAL");
				print_line();
				fp2<<"declaration_list : declaration_list COMMA ID\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;

				decl_list.push_back(make_pair($3->getName(),0));
			}
 		  | declaration_list COMMA id LTHIRD CONST_INT RTHIRD
 		  	{
				$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName()+$4->getName()+$5->getName()+$6->getName(), "NON_TERMINAL");
				print_line();
				fp2<<"declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;


				//size-> convert string to int
				int size = 0;
				stringstream val($5->getName());
				val >> size;

				decl_list.push_back(make_pair($3->getName(),size));
			}
 		  | id
 		  {
 		  		$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
 		  		print_line();
				fp2<<"declaration_list : ID\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;

				//decl_list.clear();
				decl_list.push_back(make_pair($1->getName(),0));
 		  }
 		  | id LTHIRD CONST_INT RTHIRD
 		  {
			    $$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName()+$4->getName(), "NON_TERMINAL");
 		  		print_line();
				fp2<<"declaration_list : ID LTHIRD CONST_INT RTHIRD\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;

				//size-> convert string to int
				int size = 0;
				stringstream val($3->getName());
				val >> size;

				//decl_list.clear();
				decl_list.push_back(make_pair($1->getName(),size));
 		  }

			| declaration_list error COMMA id
			{
				$$ = new SymbolInfo($1->getName()+$3->getName()+$4->getName(), "NON_TERMINAL");
				decl_list.push_back(make_pair($4->getName(),0));
				error_cnt++;
			}

		   
 		  ;


 		  
statements: statement
			{
				$$ = new SymbolInfo($1->getName()+"\n","NON_TERMINAL");
				//set_code
				stringstream ss($1->getName());
				string word;
				string temp ="";
				while (ss >> word) {
					temp+=";"+word+"\n";
				}

				$$->set_code(temp+"\n"+$1->get_code());
 		  		print_line();
				fp2<<"statements : statement\n"<<endl;
				fp2<<$$->getName()<<endl;
				fp2<<$$->get_code()<<endl;

				//free the temporary variables used in the statement
				for(string x:current_temp)
				{
					free_var(x);
				}

				current_temp.clear();
 		  	}
		
	   | statements statement
	   {
		   	$$ = new SymbolInfo($1->getName()+$2->getName()+"\n","NON_TERMINAL");
			//add two code segments
			stringstream ss($2->getName());
			string word;
			string temp ="";
			while (ss >> word) {
				temp+=";"+word+"\n";
			}
			$$->set_code($1->get_code()+temp+"\n"+$2->get_code());
 		  	print_line();
			fp2<<"statements : statements statement\n"<<endl;
			fp2<<$$->getName()<<endl;
			fp2<<$$->get_code()<<endl;

			//free the temporary variables used in the statement
			for(string x:current_temp)
			{
				free_var(x);
			}

			current_temp.clear();

 		}
		
	   ;
	   
statement: var_declaration
		{
			$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
 		  	print_line();
			fp2<<"statement : var_declaration\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

 		}
		
	  | expression_statement
	   {
		   	$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
			//set_code
			$$->set_code($1->get_code());
 		  	print_line();
			fp2<<"statement : expression_statement\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		}
	  | compound_statement
	  {
		  	$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
			//set_code
			$$->set_code($1->get_code());
 		  	print_line();
			fp2<<"statement : compound_statement\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement
	  {
			$$ = new SymbolInfo("for"+$2->getName()+$3->getName()+$4->getName()+$5->getName()+$6->getName()+$7->getName(), "NON_TERMINAL");
 		  	print_line();
			//code
			string label1 = newLabel();
			string label2 = newLabel();

			//initiallize
			$$->set_code($3->get_code());
			//condition check
			$$->code+=label1+":\n";
			$$->code+=$4->get_code();
			$$->code+="\tCMP "+$4->get_symbol()+",0\n";
			$$->code+="\tJE "+label2+"\n";
			//loop body
			$$->code+=$7->get_code();
			//increment
			$$->code+=$5->get_code();
			$$->code+="\tJMP "+label1+"\n";
			//exit_loop
			$$->code+=label2+":\n";


			fp2<<"statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

 		}
	  | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
	   {
		   	$$ = new SymbolInfo("if "+$2->getName()+$3->getName()+$4->getName()+$5->getName(), "NON_TERMINAL");
			//code
			$$->set_code($3->get_code());

			string label1 = newLabel();
			string label2 = newLabel();

			$$->code+="\tCMP "+$3->get_symbol()+",1\n";
			$$->code+="\tJE "+label1+"\n";
			$$->code+="\tJMP "+label2+"\n";
			$$->code+=label1+":\n";
			$$->code +=$5->get_code();
			$$->code+=label2+":\n";


 		  	print_line();
			fp2<<"statement : IF LPAREN expression RPAREN statement\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl<<endl;

 		}
	  | IF LPAREN expression RPAREN statement ELSE statement
	  {
		  	$$ = new SymbolInfo("if "+$2->getName()+$3->getName()+$4->getName()+$5->getName()+"\nelse\n"+$7->getName(), "NON_TERMINAL");

			//code
			$$->set_code($3->get_code());

			string label1 = newLabel();
			string label2 = newLabel();

			$$->code+="\tCMP "+$3->get_symbol()+",1\n";
			$$->code+="\tJE "+label1+"\n";
			$$->code +=$7->get_code();
			$$->code+="\tJMP "+label2+"\n";
			$$->code+=label1+":\n";
			$$->code +=$5->get_code();
			$$->code+=label2+":\n";


 		  	print_line();
			fp2<<"statement : IF LPAREN expression RPAREN statement ELSE statement\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}
	  | WHILE LPAREN expression RPAREN statement
	  {
		  	$$ = new SymbolInfo("while "+$2->getName()+$3->getName()+$4->getName()+$5->getName(), "NON_TERMINAL");
 		  	print_line();

			//code
			string label1 = newLabel();
			string label2 = newLabel();

			//condition check
			$$->code+=label1+":\n";
			$$->code+=$3->get_code();
			$$->code+="\tCMP "+$3->get_symbol()+",0\n";
			$$->code+="\tJE "+label2+"\n";
			//loop body
			$$->code+=$5->get_code();
			$$->code+="\tJMP "+label1+"\n";
			//exit_loop
			$$->code+=label2+":\n";


			fp2<<"statement : WHILE LPAREN expression RPAREN statement\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

 		}
	  | PRINTLN LPAREN id RPAREN SEMICOLON
	   {
		   	
		   	$$ = new SymbolInfo("printf"+$2->getName()+$3->getName()+$4->getName()+$5->getName(), "NON_TERMINAL");
			SymbolInfo *s = st.Look_up($3->getName());
			if(s == NULL)
			{
				error_cnt++;
				error_print_line();
				fp3<<"Undeclared variable "<<$3->getName()<<endl<<endl;
				fp2<<"Undeclared variable "<<$3->getName()<<endl<<endl;
			}
			//code
			$$->code += "\tMOV ax,"+s->get_symbol()+"\n";
			$$->code += "\tMOV print_var,ax\n";
			$$->code += "\tCALL print\n";
 		  	print_line();
			fp2<<"statement : PRINTLN LPAREN ID RPAREN SEMICOLON\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

 		}
	  | RETURN expression SEMICOLON
	  	{
			//fp2<<"RETURN symbol->name: "<<$1->getName()<<endl<<endl;
		  	$$ = new SymbolInfo($1->getName()+" "+$2->getName()+$3->getName(), "NON_TERMINAL");
			//set_code
			$$->set_code($2->get_code());
			//code
			$$->code += "\tMOV ax,"+$2->get_symbol()+"\n";
			$$->code += "\tMOV ret_temp,ax\n";
 		  	print_line();
			fp2<<"statement : RETURN expression SEMICOLON\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;

 		}
	  | func_declaration
		{
			$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
			error_cnt++;
			error_print_line();
			fp2<<"Illegal scoping, function declared inside a function\n"<<endl;
			fp3<<"Illegal scoping, function declared inside a function\n"<<endl;
		}

	  | func_definition
		{
			$$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
			//set_code
			$$->set_code($1->get_code());
			error_cnt++;
			error_print_line();
			fp2<<"Illegal scoping, function defined inside a function\n"<<endl;
			fp3<<"Illegal scoping, function defined inside a function\n"<<endl;
			fp2<<$$->get_code()<<endl;
		}
		
	  ;
	  
expression_statement: SEMICOLON	
		{
			$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
 		  	print_line();
			fp2<<"expression_statement : SEMICOLON\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}		
			| expression SEMICOLON 
			{
				$$ = new SymbolInfo($1->getName()+$2->getName(),"NON_TERMINAL");
				//set_code
				$$->set_code($1->get_code());
				$$->set_data_type($1->get_data_type());
				$$->set_symbol($1->get_symbol());

	 		  	print_line();
				fp2<<"expression_statement : expression SEMICOLON\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;
				fp2<<$$->get_code()<<endl;

 			}

			| error SEMICOLON

			{
				$$ = new SymbolInfo("","NON_TERMINAL");
				error_cnt++;
				//error_print_line();
				//yyerror("syntax error exp");

				//yyerror("invalid expression");
				yyerrok;
				yyclearin;
			}

			| expression error SEMICOLON

			{
				$$ = new SymbolInfo($1->getName()+$3->getName(),"NON_TERMINAL");
				error_cnt++;
			}
			;
	  
variable: id
		{
			$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");

			print_line();
			fp2<<"variable : ID\n"<<endl;

			//check if ID is declared or not
			if(!check_var_declared($1->getName()))
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Undeclared variable "<<$1->getName()<<endl<<endl;
				fp2<<"Undeclared variable "<<$1->getName()<<endl<<endl;
			}

			else
			{
				SymbolInfo *s = st.Look_up($1->getName());
				if(s->get_array())
				{
					mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();
					fp3<<"Type mismatch, "+$1->getName()+" is an array"<<endl<<endl;
					fp2<<"Type mismatch, "+$1->getName()+" is an array"<<endl<<endl;
					$$->setType("array");
				}	
			}
			
			
			//set variable data type and symbol according to id's data type
			SymbolInfo* s = st.Look_up($1->getName());
			if(s != NULL)
			{
				//data_type
				set_data_type($$,s);
				//type
				$$->setType(s->getType());
				//symbol
				$$->set_symbol(s->get_symbol());

			}
			
			
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}		
	 	| id LTHIRD expression RTHIRD
	 	{
			print_line();
			fp2<<"variable : ID LTHIRD expression RTHIRD\n"<<endl;

			//check if ID is declared or not
			if(!check_var_declared($1->getName()))
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"variable "<<$1->getName()<<" not declared"<<endl;
				fp2<<"variable "<<$1->getName()<<" not declared"<<endl;
			}

			else
			{	
				//check is the id actually declared as an array
				SymbolInfo *s = st.Look_up($1->getName());
				if(!s->get_array())
				{
					mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();
					fp3<<$1->getName()+" not an array"<<endl<<endl;
					fp2<<$1->getName()+" not an array"<<endl<<endl;

				}		
			}

			//check array's index is integer type
			//fp2<<$3->get_data_type()<<endl<<endl;
			if($3->get_data_type() != "int")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Expression inside third brackets not an integer"<<endl<<endl;
				fp2<<"Expression inside third brackets not an integer"<<endl<<endl;
			}
			
			$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName()+$4->getName(), "NON_TERMINAL");
			//set variable data type according to id's data type
			SymbolInfo* s = st.Look_up($1->getName());
			if(s != NULL)
			{
				//data_type and symbol
				set_data_type($$,s);
				$$->set_size(s->get_size());
				$$->setType("array");

				//index calculation
				//saving the index in bx
				$$->code += "\tMOV bx,"+$3->get_symbol()+"\n";
				//index = index*2
				$$->code += "\tADD bx,bx\n";

				$$->set_symbol(s->get_symbol());

				/*
				//added code
				string temp = newTemp();
				data_seg.push_back(temp+"dw ?");
				
				$$->code +="\tMOV cx,"+$1->get_symbol()+"\n";
				$$->code +="\tMOV "+temp+",cx[bx]\n";
				$$->set_symbol(temp);
				*/
			}
			
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		} 
	 ;
	 
 expression: logic_expression	
 		{
			$$ = new SymbolInfo($1->getName(),$1->getType());
			//set data type of expression
			$$->set_code($1->get_code());
			$$->set_size($1->get_size());
			set_data_type($$,$1);
			$$->set_symbol($1->get_symbol());

 		  	print_line();
			fp2<<"expression : logic expression\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;


 		} 
	   | variable ASSIGNOP logic_expression
	   {
		   	$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(),"NON_TERMINAL");

			//set data type of logic expression
			set_data_type($$,$1);

			if($3->get_data_type() == "void" | $1->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			else if($3->get_data_type() == "int" && $1->get_data_type() == "float")
			{
				//type convert -> no error
			}

			//find error if operands of assignment operations are not compatible
			else if($1->get_data_type() != $3->get_data_type() && mismatch_map[yylineno] == 0)
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
			$$->set_code($3->get_code()+$1->get_code());

			//code
			if($1->getType() == "array")
			{
				$$->code += "\tMOV ax,"+$3->get_symbol()+"\n";
				$$->code += "\tMOV "+$1->get_symbol()+"[bx],ax\n";
			}

			else
			{
				$$->code += "\tMOV ax,"+$3->get_symbol()+"\n";
				$$->code += "\tMOV "+$1->get_symbol()+",ax\n";
			}
			$$->set_symbol($1->get_symbol());

			print_line();
			fp2<<"expression : variable ASSIGNOP logic_expression\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		} 
		 
	   ;
			
logic_expression: rel_expression 	
 		{
			$$ = new SymbolInfo($1->getName(),$1->getType());
			//set data type of logic expression
			$$->set_code($1->get_code());
			$$->set_size($1->get_size());
			set_data_type($$,$1);
			$$->set_symbol($1->get_symbol());

 		  	print_line();
			fp2<<"logic_expression : rel_expression\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		} 	
		 | rel_expression LOGICOP rel_expression 
		 {
			$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(),"NON_TERMINAL");
			//add two code segments
			$$->set_code($1->get_code()+$3->get_code());
			//set data type of logic expression
			//set_data_type($$,$1);
			$$->set_data_type("int");

			if($1->get_data_type() == "void" | $3->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			if($2->getName() == "||")
			{
				string label1 = newLabel();
				string label2 = newLabel();

				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");

				$$->code+="\tCMP "+$1->get_symbol()+",0\n";
				$$->code+="\tJNE "+label1+"\n";
				$$->code+="\tCMP "+$1->get_symbol()+",0\n";
				$$->code+="\tJNE "+label1+"\n";
				$$->code+="\tMOV "+temp+",0\n";
				$$->code+="\tJMP "+label2+"\n";
				$$->code+=label1+":\n";
				$$->code+="\tMOV "+temp+",1\n";
				$$->code+=label2+":\n";

				$$->set_symbol(temp);
			}

			else if($2->getName() == "&&")
			{
				string label1 = newLabel();
				string label2 = newLabel();

				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");

				$$->code+="\tCMP "+$1->get_symbol()+",0\n";
				$$->code+="\tJE "+label1+"\n";
				$$->code+="\tCMP "+$3->get_symbol()+",0\n";
				$$->code+="\tJE "+label1+"\n";
				$$->code+="\tMOV "+temp+",1\n";
				$$->code+="\tJMP "+label2+"\n";
				$$->code+=label1+":\n";
				$$->code+="\tMOV "+temp+",0\n";
				$$->code+=label2+":\n";

				$$->set_symbol(temp);
			}

			

 		  	print_line();
			fp2<<"logic_expression : rel_expression LOGICOP rel_expression\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		} 		
		 ;
			
rel_expression: simple_expression 
		{
			$$ = new SymbolInfo($1->getName(),$1->getType());
			//set data type of rel expression
			$$->set_code($1->get_code());
			$$->set_size($1->get_size());
			set_data_type($$,$1);
			$$->set_symbol($1->get_symbol());

 		  	print_line();
			fp2<<"rel_expression : simple_expression\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		} 	
		| simple_expression RELOP simple_expression	
		{
			$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(),"NON_TERMINAL");
			//add two code segments
			$$->set_code($1->get_code()+$3->get_code());
			//set data type of rel expression
			//set_data_type($$,$1);
			$$->set_data_type("int");

			if($1->get_data_type() == "void" | $3->get_data_type() == "void")
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

			$$->code+="\tMOV ax,"+$1->get_symbol()+"\n";
			$$->code+="\tCMP ax,"+$3->get_symbol()+"\n";

			if($2->getName() == "==")
			{
				$$->code+="\tJE "+label1+"\n";
			}

			else if($2->getName() == "<")
			{
				$$->code+="\tJL "+label1+"\n";
			}

			else if($2->getName() == ">")
			{
				$$->code+="\tJG "+label1+"\n";
			}

			else if($2->getName() == "<=")
			{
				$$->code+="\tJLE "+label1+"\n";
			}

			else if($2->getName() == ">=")
			{
				$$->code+="\tJGE "+label1+"\n";
			}

			else if($2->getName() == "!=")
			{
				$$->code+="\tJNE "+label1+"\n";
			}

			$$->code+="\tMOV cx,0\n";
			$$->code+="\tJMP "+label2+"\n";
			$$->code+=label1+":\n";
			$$->code+="\tMOV cx,1\n";
			$$->code+=label2+":\n";
			$$->code+="\tMOV "+temp+",cx\n";

			$$->set_symbol(temp);

 		  	print_line();
			fp2<<"rel_expression : simple_expression RELOP simple_expression\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		} 
		;
				
simple_expression: term 
		{
			$$ = new SymbolInfo($1->getName(),$1->getType());
			//set data type of simple expression
			$$->set_code($1->get_code());
			$$->set_size($1->get_size());
			set_data_type($$,$1);
			$$->set_symbol($1->get_symbol());

 		  	print_line();
			fp2<<"simple_expression : term\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		} 
		  | simple_expression ADDOP term 
		{
			$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(),"NON_TERMINAL");
			//add two code segments
			$$->set_code($1->get_code()+$3->get_code());
			//set data type of simple expression

			if($1->get_data_type() == "void" | $3->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				$$->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//type conversion
			else if($1->get_data_type() == "float" | $3->get_data_type() == "float")
			{
				$$->set_data_type("float");
			}

			else
			{
				$$->set_data_type("int");
			}

			//code
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");
			$$->code +="\tMOV ax,"+$1->get_symbol()+"\n";
			
			if($2->getName() == "+")
			{
				$$->code +="\tADD ax,"+$3->get_symbol()+"\n";
			}
			else
			{
				$$->code +="\tSUB ax,"+$3->get_symbol()+"\n";
			}

			$$->code+="\tMOV "+temp+",ax\n";

			$$->set_symbol(temp);


 		  	print_line();
			fp2<<$2->getName()<<endl;
			//fp2<<$$->code<<endl;
			fp2<<"simple_expression : simple_expression ADDOP term\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		} 
		  
		  ;
					
term:	unary_expression
		{
			$$ = new SymbolInfo($1->getName(),$1->getType());
			//set data type of term
			$$->set_code($1->get_code());
			$$->set_size($1->get_size());
			set_data_type($$,$1);
			$$->set_symbol($1->get_symbol());

 		  	print_line();
			fp2<<"term : unary_expression\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		}
     |  term MULOP unary_expression
     	{
		 	$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(),"NON_TERMINAL");

			//adding the code segments together
			$$->set_code($1->get_code()+$3->get_code());

			print_line();
			fp2<<"term : term MULOP unary_expression\n"<<endl;
			
			if($1->get_data_type() == "void" | $3->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				$$->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//type conversion
			else if($1->get_data_type() == "float" | $3->get_data_type() == "float")
			{
				$$->set_data_type("float");
			}

			else
			{
				$$->set_data_type("int");
			}

			//check operands of modulus operator
			if($2->getName() == "/" && $3->getType() == "zero")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Divided by Zero"<<endl<<endl;
				fp2<<"Divided by Zero"<<endl<<endl;
			}

			if($2->getName() == "%" && $3->getType() == "zero")
			{
				mismatch_map[yylineno]++;
				error_cnt++;
				error_print_line();
				fp3<<"Modulus by Zero"<<endl<<endl;
				fp2<<"Modulus by Zero"<<endl<<endl;

				$$->set_data_type("int");
			}

			else if($2->getName() == "%")
			{
				if($1->get_data_type() != "int" | $3->get_data_type() != "int")
				{
					mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();
					fp3<<"Non-Integer operand on modulus operator"<<endl<<endl;
					fp2<<"Non-Integer operand on modulus operator"<<endl<<endl;
				}

				$$->set_data_type("int");
			}

			//code
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");

			if($2->getName() == "*")
			{
				$$->code+="\tMOV ax,"+$1->get_symbol()+"\n";
				$$->code+="\tMOV dx,"+$3->get_symbol()+"\n";
				$$->code+="\tMUL dx\n";
				//result lower half is in ax, upper half in dx
				$$->code+="\tMOV "+temp+",ax\n";
				
			}

			else
			{
				$$->code+="\tMOV ax,"+$1->get_symbol()+"\n";
				$$->code+="\tXOR dx,dx\n";
				$$->code+="\tMOV cx,"+$3->get_symbol()+"\n";
				$$->code+="\tDIV cx\n";

				if($2->getName() == "/")
				{
					$$->code+="\tMOV "+temp+",ax\n";
				}
				else if($2->getName() == "%")
				{
					$$->code+="\tMOV "+temp+",dx\n";
				}
			}
			$$->set_symbol(temp);

			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		}
     ;

unary_expression: ADDOP unary_expression
 		{
			$$ = new SymbolInfo($1->getName()+$2->getName(),$2->getType());
			//set data type of unary expression
			$$->set_code($2->get_code());
			set_data_type($$,$2);		
			$$->set_size($2->get_size());

			if($2->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				$$->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			if($1->getName() == "-")
			{
				$$->code+="\tNEG "+$2->get_symbol()+"\n";	
			}
			//symbol same as unary_expression
			$$->set_symbol($2->get_symbol());

 		  	print_line();
			fp2<<"unary_expression : ADDOP unary expression\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}  
		 | NOT unary_expression
		 {
			$$ = new SymbolInfo($1->getName()+$2->getName(),$2->getType());
			//set data type of unary expression
			$$->set_code($2->get_code());
			set_data_type($$,$2);
			$$->set_size($2->get_size());

			if($2->get_data_type() == "void")
			{
				mismatch_map[yylineno]++;
				$$->set_data_type("int");
				error_cnt++;
				error_print_line();
				fp3<<"Void function used in expression"<<endl<<endl;
				fp2<<"Void function used in expression"<<endl<<endl;
			}

			//code
			$$->code+="\tNOT "+$2->get_symbol()+"\n";
			//symbol same as unary_expression
			$$->set_symbol($2->get_symbol());

 		  	print_line();
			fp2<<"unary_expression : NOT unary expression\n"<<endl;
			fp2<<$$->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}   
		 | factor 
		 {
			$$ = new SymbolInfo($1->getName(),$1->getType());
			//set data type of unary expression
			$$->set_code($1->get_code());
			set_data_type($$,$1);
			$$->set_size($1->get_size());
			$$->set_symbol($1->get_symbol());

 		  	print_line();
			fp2<<"unary_expression : factor\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;

 		}  
		 ;
	
factor: variable 
 		{
			$$ = new SymbolInfo($1->getName(),$1->getType());
			$$->set_code($1->get_code());
			//set data type of factor
			set_data_type($$,$1);
			
			//the symbol is same as variable symbol
			if($1->getType() == "array")
			{
				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");
				$$->set_size($1->get_size());

				$$->code+= "\tMOV ax,"+$1->get_symbol()+"[bx]\n";
				$$->code+= "\tMOV "+temp+",ax\n";
				$$->set_symbol(temp);
			}

			else
			{
				$$->set_symbol($1->get_symbol());
			}

 		  	print_line();
			fp2<<"factor : variable\n"<<endl;
			fp2<<$1->getName()<<endl<<endl;
			fp2<<$$->get_code()<<endl;
 		} 
	| id LPAREN argument_list RPAREN
	{
		$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName()+$4->getName(), "NON_TERMINAL");
		//set_code
		$$->set_code($3->get_code());
		print_line();
		fp2<<"factor : ID LPAREN argument_list RPAREN\n"<<endl;

		
		//check if declared
		SymbolInfo* s = st.Look_up($1->getName());
		if(s == NULL)
		{
			mismatch_map[yylineno]++;
			error_cnt++;
			error_print_line();
			fp3<<"Undeclared function "<<$1->getName()<<endl<<endl;
			fp2<<"Undeclared function "<<$1->getName()<<endl<<endl;
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
				fp3<<$1->getName() + " is not a function"<<endl<<endl;
				fp2<<$1->getName() + " is not a function"<<endl<<endl;
			}

			else //is a function
			{
				//set type according to return type
				$$->set_data_type(f->getReturn_type());

				//check if defined
				if(f->get_flag() == 0)
				{
					//mismatch_map[yylineno]++;
					error_cnt++;
					error_print_line();

					fp3<<$1->getName() + " function declared but not defined"<<endl<<endl;
					fp2<<$1->getName() + " function declared but not defined"<<endl<<endl;

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
						fp3<<"Total number of arguments mismatch in function "+$1->getName()<<endl<<endl;
						fp2<<"Total number of arguments mismatch in function "+$1->getName()<<endl<<endl;
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
									fp3<<i+1<<"th argument mismatch in function " + $1->getName()<<endl<<endl;
									fp2<<i+1<<"th argument mismatch in function " + $1->getName()<<endl<<endl;
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
						fp3<<"Total number of arguments mismatch in function "+$1->getName()<<endl<<endl;
						fp2<<"Total number of arguments mismatch in function "+$1->getName()<<endl<<endl;
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
									fp3<<i+1<<"th argument mismatch in function " + $1->getName()<<endl<<endl;
									fp2<<i+1<<"th argument mismatch in function " + $1->getName()<<endl<<endl;
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
				$$->code+="\tPUSH "+x+"\n";
				
			}
			$$->code+="\tCALL "+$1->getName()+"\n";

			if(f->getReturn_type() != "void")
			{
				//$$->code+="\tPOP ret_temp\n";
				$$->code+="\tMOV ax,ret_temp\n";

				string temp = newTemp();
				current_temp.push_back(temp);
				//data_seg.push_back(temp+" dw ?");
				$$->code+="\tMOV "+temp+",ax\n";

				$$->set_symbol(temp);
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
 	  	
		fp2<<$$->getName()<<endl;
		fp2<<$$->get_code()<<endl;
 	} 
	| LPAREN expression RPAREN
	{
		$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), $2->getType());
		$$->set_code($2->get_code());

		set_data_type($$,$2);
		//set symbol same as the expression symbol
		$$->set_symbol($2->get_symbol());
 		print_line();
		fp2<<"factor : LPAREN expression RPAREN\n"<<endl;
		fp2<<$$->get_code()<<endl;
		fp2<<$$->getName()<<endl<<endl;

	} 
	| CONST_INT 
	{
		$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
		$$->set_data_type("int");

		if($1->getName() == "0")
		{
			$$->setType("zero");
		}

		//the symbol is same as value
		$$->set_symbol($1->getName());

 		print_line();
		fp2<<"factor : CONST_INT\n"<<endl;
		
		fp2<<$1->getName()<<endl<<endl;
	} 
	| CONST_FLOAT
	{
		$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
		$$->set_data_type("float");

		//the symbol is same as value
		$$->set_symbol($1->getName());

	  	print_line();
		fp2<<"factor : CONST_FLOAT\n"<<endl;
		
		fp2<<$1->getName()<<endl<<endl;
 	} 
	| variable INCOP
	{
		$$ = new SymbolInfo($1->getName()+$2->getName(),$1->getType());
		$$->set_code($1->get_code());
		//set data type of factor
		set_data_type($$,$1);

		if($1->getType() == "array")
		{
			string temp = newTemp();
			current_temp.push_back(temp);
			
			//data_seg.push_back(temp+" dw ?");

			$$->code += "\tMOV ax,"+$1->get_symbol()+"[bx]\n";
			$$->code += "\tMOV "+temp+",ax\n";
			$$->code += "\tINC "+$1->get_symbol()+"[bx]\n";

			$$->set_symbol(temp);
		}

		else
		{
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");
			
			$$->code += "\tMOV ax,"+$1->get_symbol()+"\n";
			$$->code += "\tMOV "+temp+",ax\n";
			$$->code += "\tINC "+$1->get_symbol()+"\n";

			$$->set_symbol(temp);
		}

 	  	print_line();
		fp2<<"factor : variable INCOP\n"<<endl;
		fp2<<$$->get_code()<<endl;
		fp2<<$$->getName()<<endl<<endl;
 	}  
	| variable DECOP
	{
		$$ = new SymbolInfo($1->getName()+$2->getName(),"NON_TERMINAL");
		$$->set_code($1->get_code());
		//set data type of factor
		set_data_type($$,$1);

		if($1->getType() == "array")
		{
			string temp = newTemp();
			current_temp.push_back(temp);
			//data_seg.push_back(temp+" dw ?");

			$$->code += "\tMOV ax,"+$1->get_symbol()+"[bx]\n";
			$$->code += "\tMOV "+temp+",ax\n";
			$$->code += "\tDEC "+$1->get_symbol()+"[bx]\n";

			$$->set_symbol(temp);
		}

		else
		{
			string temp = newTemp();
			//data_seg.push_back(temp+" dw ?");
			current_temp.push_back(temp);
			
			$$->code += "\tMOV ax,"+$1->get_symbol()+"\n";
			$$->code += "\tMOV "+temp+",ax\n";
			$$->code += "\tDEC "+$1->get_symbol()+"\n";

			$$->set_symbol(temp);
		}

 	  	print_line();
		fp2<<"factor : variable DECOP\n"<<endl;
		fp2<<$$->get_code()<<endl;
		fp2<<$$->getName()<<endl<<endl;
 	} 
	;
	
argument_list: arguments
		{
			$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
 		  	print_line();
			//set_code
			$$->set_code($1->get_code());
			fp2<<"argument_list : arguments\n"<<endl;
			fp2<<$$->getName()<<endl;
 		} 
		|
		{
			$$ = new SymbolInfo("", "NON_TERMINAL");
 		  	print_line();
			fp2<<"argument_list : \n"<<endl;
			fp2<<$$->getName()<<endl;
 		} 
			  ;
	
arguments: arguments COMMA logic_expression
			{
				$$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "NON_TERMINAL");
				//set_code
				$$->set_code($1->get_code()+$3->get_code());
				print_line();
				fp2<<"arguments : arguments COMMA logic_expression\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;

				if($3->getType() != "array")
					arg_list.push_back($3->get_data_type());
				else
					arg_list.push_back("array");

				arg_symbol.push_back($3->get_symbol());
 			} 
	      | logic_expression
	     	{
				$$ = new SymbolInfo($1->getName(),"NON_TERMINAL");
				//set_code
				$$->set_code($1->get_code());
				print_line();
				fp2<<"arguments : logic_expression\n"<<endl;
				fp2<<$$->getName()<<endl<<endl;

				if($1->getType() != "array")
					arg_list.push_back($1->get_data_type());
				else
					arg_list.push_back("array");

				arg_symbol.push_back($1->get_symbol());
 			} 
	      ;
	      
id: ID
	{		
		$$ = new SymbolInfo($1->getName(),$1->getType());	
		temp_id = $1->getName();
		//print_line();
		//fp2<<"id: ID"<<endl;
	}
;
 


%%
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