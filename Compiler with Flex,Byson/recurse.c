int f(int a){
    return 2*a;
    a=9;
}

int g(int a, int b){
    int x;
    x=f(a)+a+b;
    return x;
}

int h(int a)
{
    if(a<=1)
    {
        return 1;
    }

    else
    {
        return a*h(a-1);
    }
}

int main(){
    int a,b;
    a=1;
    b=2;
    a=g(a,b);
    b=h(a);

    println(a);
    println(b);
    return 0;
}
