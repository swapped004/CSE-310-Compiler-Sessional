#include<bits/stdc++.h>
#include<cstdlib>
#include<fstream>

using namespace std;

#define INF 999999


class SymbolInfo{
    string name;
    string type;
    SymbolInfo *next;
    int index;
    int pos;

public:

    SymbolInfo()
    {

    }

    SymbolInfo(string name, string type)
    {
        //next = new SymbolInfo();
        next = NULL;
        this->name = name;
        this->type = type;
        index = -1;
        pos = -1;
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

    }
};


int hash_func(string k, int N)
{
    int sum  = 0;
    for(char c:k)
    {
        sum = (sum+c)%N;
    }

    return sum;
}

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



    void Print()
    {
        cout<<"ScopeTable # "<<get_id()<<endl;
        //out<<"ScopeTable # "<<get_id()<<endl;

        for(int i=0;i<total_buckets;i++)
        {
            
            SymbolInfo* x = table[i];
            
            if(x == NULL)
            	continue;
            
            cout<<" "<<i<<" --> ";
            //out<<" "<<i<<" --> ";

            while(x != NULL)
            {
                cout<<"< "<<x->getName()<<" : "<<x->getType()<<"> ";
                //out<<"< "<<x->getName()<<" : "<<x->getType()<<"> ";

           
                x = x->getNext();
            }

            cout<<" "<<endl;
            //out<<endl;
        }
        
        //out<<endl;

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

    void Print_curr()
    {
        curr->Print();
    }

    void Print_all()
    {
        ScopeTable *x = curr;
        while(x != NULL)
        {
            x->Print();
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


