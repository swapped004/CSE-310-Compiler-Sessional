
#include<bits/stdc++.h>
#include<cstdlib>
#include<fstream>




#ifndef SYMBOL
#define SYMBOL


using namespace std;

#define INF 999999



class func_param{
    int number_of_param;
    string return_type;
    vector<pair<string,string>> param_list;
    int dec_or_def;
    vector<string> param_symbol;
    vector<string> stack_symbol;

public:
    func_param()
    {

    }

    func_param(int n, string rt,vector<pair<string,string>> pl,int flag)
    {
        number_of_param = n;
        return_type = rt;
        param_list = pl;

        //flag = 0 -> declared
        // flag = 1 -> defined
        //flag = -1 -> neither declared or defined

        dec_or_def = flag; 
    }

    void set_flag(int flag)
    {
        dec_or_def = flag;
    }

    int get_flag()
    {
        return dec_or_def;
    }

    int getNumber_of_param()
    {
        return number_of_param;
    }

    string getReturn_type()
    {
        return return_type;
    }

    void set_param_symbol(vector<string> p)
    {
        param_symbol = p;
    }

    vector<string> get_param_symbol()
    {
        return param_symbol;
    }

    void set_stack_symbol(vector<string> p)
    {
        stack_symbol = p;
    }

    vector<string> get_stack_symbol()
    {
        return stack_symbol;
    }

    vector<pair<string,string>> getParam_list()
    {
        return param_list;
    }
};


class SymbolInfo{
    //name
    string name;
    //type-> array or not
    string type;
    //next pointer
    SymbolInfo *next;
    //index and position
    int index;
    int pos;
    bool is_array;
    //for array size
    int size;
    //for array index symbol
    string index_symbol;
    
    //data type
    string data_type;
    //function parameters if it is a function
    func_param* func;

    //symbol
    string symbol;
    
public:
    //store code
    string code;

    SymbolInfo()
    {
        code = "";
    }

    SymbolInfo(string name, string type)
    {
        //next = new SymbolInfo();
        next = NULL;
        this->name = name;
        this->type = type;
        index = -1;
        pos = -1;
        size = -1;
        code = "";

        //default data type set to int
        data_type = "int";
        func = NULL;
        is_array = false;

        //
        symbol = "";
        index_symbol = "";
    }

    void set_symbol(string symbol)
    {
        this->symbol = symbol;
    }

    string get_symbol()
    {
        return symbol;
    }

    void set_index_symbol(string index_symbol)
    {
        this->index_symbol = index_symbol;
    }

    string get_index_symbol()
    {
        return index_symbol;
    }

    void set_size(int size)
    {
        this->size = size;
    }

    int get_size()
    {
        return size;
    }

    void set_code(string code)
    {
        this->code = code;
    }

    string get_code()
    {
        return code;
    }

    void set_data_type(string dt)
    {
        data_type = dt;
    }

    string get_data_type()
    {
        return data_type;
    }

    void set_array(bool val)
    {
        is_array = val;
    }

    bool get_array()
    {
        return is_array;
    }

    void set_func(int num_of_param, string return_type, vector<pair<string,string>> param_list,int flag)
    {
        func = new func_param(num_of_param, return_type, param_list, flag);
    }

    func_param* get_func()
    {
        return func;
    }

    string getName()
    {
        return name;
    }

    string getType()
    {
        return type;
    }

    void setName(string name)
    {
        this->name = name;
    }

    void setType(string type)
    {
        this->type = type;
    }

    SymbolInfo* getNext()
    {
        return next;
    }

    void setNext(SymbolInfo* s)
    {
        next = s;
    }

    void setPos(int n)
    {
        pos = n;
    }

    void setIndex(int n)
    {
        index = n;
    }

    int getPos()
    {
        return pos;
    }

    int getIndex()
    {
        return index;
    }

    


    ~SymbolInfo()
    {
        delete func;
    }
};




class ScopeTable{

    vector<SymbolInfo*> table;
    int total_buckets;
    ScopeTable* parentScope;
    string current_id;
    int num_of_children;

public:

    ScopeTable()
    {

    }
    

    ScopeTable(int n)
    {
        total_buckets = n;
        table.resize(total_buckets);

        //initialize all the pointers to null
        for(int i=0;i<total_buckets;i++)
        {
            table[i] = NULL;
        }

        //resize parent to null
        parentScope = new ScopeTable();
        parentScope = NULL;

        //initialize id & number of children
        current_id = "1";
        num_of_children = 0;
    }
    
    int hash_func(string k, int N)
	{
		int sum  = 0;
		for(char c:k)
		{
		    sum = (sum+c)%N;
		}

		return sum;
	}


    void set_parent_scope(ScopeTable* ps)
    {
        parentScope = ps;
        current_id = ps->get_id()+"."+to_string(ps->getNum_of_children());
    }

    ScopeTable* get_parent_scope()
    {
        return parentScope;
    }

    string get_id()
    {
        return current_id;
    }

    int getNum_of_children()
    {
        return num_of_children;
    }

    void setNum_of_children(int n)
    {
        num_of_children = n;
    }

    
    SymbolInfo* Look_up(string name)
    {
        int index = hash_func(name,total_buckets);
        //cout<<index<<endl;

        SymbolInfo* x = table[index];
        int pos = 0;

        while(x != NULL)
        {
            if(x->getName() == name)
            {
                cout<<"Found in ScopeTable# "+current_id+" at position "+to_string(index)+", "+to_string(pos)<<endl;
                //out<<"Found in ScopeTable# "+current_id+" at position "+to_string(index)+", "+to_string(pos)<<endl;

                return x;
            }
            else
                x = x->getNext();

            pos++;
        }

        return NULL;
    }


    bool Insert(string name, string type)
    {
        SymbolInfo* si = Look_up(name);

        if(si == NULL)
        {
            SymbolInfo* s = new SymbolInfo(name,type);
            int index = hash_func(s->getName(),total_buckets);
            //cout<<index<<endl;

            SymbolInfo* x = table[index];

            int pos = 0;

            if(x == NULL)
            {
                s->setIndex(index);
                s->setPos(pos);
                x = s;
                table[index] = x;
            }

            else
            {
                while(true)
                {
                    pos++;
                    if(x->getNext() == NULL)
                    {
                        s->setIndex(index);
                        s->setPos(pos);
                        x->setNext(s);
                        break;
                    }

                    else
                        x = x->getNext();
                }
            }


            cout<<"Inserted in ScopeTable# "+current_id+" at position "+to_string(index)+", "+to_string(pos)<<endl;
            //out<<"Inserted in ScopeTable# "+current_id+" at position "+to_string(index)+", "+to_string(pos)<<endl;
            return true;

        }

        else
        {
            cout<<"<<"<<si->getName()<<", "<<si->getType()<<">> already exists in current ScopeTable"<<endl;
            //out<<si->getName()<<" already exists in current ScopeTable"<<endl<<endl;
            return false;
        }

    }

    bool Delete(string name)
    {

        int index = hash_func(name,total_buckets);
        //cout<<index<<endl;

        int pos = 0;

        SymbolInfo* x = table[index];

        if(x == NULL)
        {
            return false;
        }

        else if(x->getName() == name)
        {
            table[index] = x->getNext();
            delete x;

            cout<<"Found in ScopeTable# "<<current_id<<" at position "<<x->getIndex()<<", "<<x->getPos()<<endl;
            cout<<"Deleted Entry "<<x->getIndex()<<", "<<x->getPos()<<" from current ScopeTable"<<endl;
            //out<<"Found in ScopeTable# "<<current_id<<" at position "<<x->getIndex()<<", "<<x->getPos()<<endl;
            //out<<"Deleted Entry "<<x->getIndex()<<", "<<x->getPos()<<" from current ScopeTable"<<endl;

            return true;
        }

        else
        {
            while(true)
            {
                if(x->getNext() == NULL)
                {
                    return false;
                }

                else if((x->getNext())->getName() == name)
                {
                    cout<<"Found in ScopeTable# "<<current_id<<" at position "<<x->getNext()->getIndex()<<", "<<x->getNext()->getPos()<<endl;
                    //out<<"Found in ScopeTable# "<<current_id<<" at position "<<x->getNext()->getIndex()<<", "<<x->getNext()->getPos()<<endl;
                    cout<<"Deleted Entry "<<x->getNext()->getIndex()<<", "<<x->getNext()->getPos()<<" from current ScopeTable"<<endl;
                    //out<<"Deleted Entry "<<x->getNext()->getIndex()<<", "<<x->getNext()->getPos()<<" from current ScopeTable"<<endl;

                    SymbolInfo* temp = x->getNext()->getNext();
                    delete x->getNext();
                    x->setNext(temp);
                    return true;
                }

                else
                {
                    x = x->getNext();
                }

            }

        }

    }



    void Print(ofstream &out) const
    {
        cout<<"ScopeTable # "<<current_id<<endl;
        out<<"ScopeTable # "<<current_id<<endl;

        for(int i=0;i<total_buckets;i++)
        {
            
            SymbolInfo* x = table[i];
            
            if(x == NULL)
            	continue;
            
            cout<<" "<<i<<" --> ";
            out<<" "<<i<<" --> ";

            while(x != NULL)
            {
                cout<<"< "<<x->getName()<<" , "<<x->getType()<<" > ";
                out<<"< "<<x->getName()<<" , "<<x->getType()<<" > ";

           
                x = x->getNext();
            }

            cout<<" "<<endl;
            out<<endl;
        }
        
        out<<endl;

    }



    ~ScopeTable()
    {
        for(int i=0;i<table.size();i++)
        {
            SymbolInfo *curr_p = table[i];

            while(curr_p != NULL)
            {
                SymbolInfo *next_p = curr_p->getNext();
                free(curr_p);
                curr_p = next_p;
            }

            table[i] = NULL;
        }

        table.clear();
    }

};


class SymbolTable
{
    ScopeTable* curr;
    int total_buckets;
    
public:
    SymbolTable()
    {

    }
    SymbolTable(int n)
    {
        total_buckets = n;
        curr = new ScopeTable(total_buckets);
    }

    ScopeTable* get_curr()
    {
        return curr;
    }


    void enter_scope()
    {
        curr->setNum_of_children(curr->getNum_of_children() + 1);
        ScopeTable* new_scope = new ScopeTable(total_buckets);
        new_scope->set_parent_scope(curr);
        curr = new_scope;

        cout<<"New ScopeTable with id "<<curr->get_id()<<" created"<<endl;
        //out<<"New ScopeTable with id "<<curr->get_id()<<" created"<<endl;
    }

    void exit_scope()
    {
        cout<<"ScopeTable with id "<<curr->get_id()<<" removed"<<endl;
        //out<<"ScopeTable with id "<<curr->get_id()<<" removed"<<endl;

        ScopeTable *ps = curr->get_parent_scope();
        delete curr;
        curr = ps;
    }

    bool Insert(string name, string type)
    {
        bool ok = curr->Insert(name,type);
        return ok;
    }

    bool Remove(string name)
    {
        bool ok = curr->Delete(name);

        if(!ok)
        {
            cout<<"Not found"<<endl;
            //out<<"Not found"<<endl;
        }

        return ok;
    }

    SymbolInfo* Look_up_current(string name)
    {
        SymbolInfo* temp = curr->Look_up(name);

        return temp;
    }


    SymbolInfo* Look_up(string name)
    {
        ScopeTable *x = curr;
        while(x != NULL)
        {
            SymbolInfo* temp = x->Look_up(name);

            if(temp != NULL)
            {
                return temp;
            }

            x = x->get_parent_scope();
        }

        cout<<"Not found"<<endl;
        //out<<"Not found"<<endl;

        return NULL;

    }

    void Print_curr(ofstream &out) const
    {
        curr->Print(out);
    }

    void Print_all(ofstream &out) const
    {
        ScopeTable *x = curr;
        while(x != NULL)
        {
            x->Print(out);
            x = x->get_parent_scope();
        }
    }

    ~SymbolTable()
    {
        ScopeTable* st = curr;
        while(st != NULL)
        {
            ScopeTable* temp = st->get_parent_scope();
            delete st;
            st = temp;
        }
    }
};

#endif