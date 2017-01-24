#include <vector>
#include <set>
#include <iostream>
using namespace std;

struct fault_list{
    int nid;
    bool s_a_v;
};

bool operator<(const fault_list& l1, const fault_list& l2);

void print_fault_list(set<fault_list> L);



class cgate;

class cnode {
public:
    
    cnode(int id);
    bool ifready();
    void setready(bool r);
    void setvalue(bool v);
    bool getvalue();
    void inputgate(int gid);
    void outputgate(int gid);
    int getnid();
    
    vector<int> getinputgid();
    int getoutputgid();

    void add_fault_list(fault_list f);
    void add_fault_list(set<fault_list> f);
    set<fault_list> get_fault_list();
    void clear_fault_list();
    void setsat0();
    void setsat1();
    int getsat0();
    int getsat1();
    void clearsat();

//----------------------
    void setv5(int v);
    int getv5();
    bool getv5ready();
    void setv5ready(bool r);

private:
    bool value;
    bool ready;
    int nid;
    vector<int> asinput;
    int asoutput;

    set<fault_list> L;
    int sat0;
    int sat1;


    //------PODEM---------

    int v5;    // 0->0  1->1  x->3  D->4  D'->5
    bool v5_ready;
    
};
