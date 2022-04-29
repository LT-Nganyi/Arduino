#include <iostream>
#include <map>
#include <string>

using namespace std;
int main()
{
    map<int,string>names;
    names[1] = "Trevor";
    names[2] = "Nganyi";
    names[3] = "Likunda";

    int x = 2;
    cout<<"Hello World"<<endl;
    cout<<names[2]<<endl;
    return 0;
}

