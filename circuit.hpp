#include <vector>
#include <set>
#include <iostream>
#include "cgate.hpp"
#include "cnode.hpp"

using namespace std;

struct fault_list;
void print_fault_list(set<fault_list> L);

struct nid_v5{
    int nid;
    int v5;
};

class circuit{
public:
    
    void add_gate_nodes(gatetype gt,vector<int>& nid);
    void printbygates();
    
    void printoutput();
    
    void addinput(int nid);
    void addoutput(int nid);
    
    void run(vector<bool>& invalue);
    void clear();
    
    bool rungate(int gid);
    void setinputvalue(vector<bool>& invalue);
    
    void furtherstuck(vector<int>& nid);

    void setinput_fault_list();
    bool detective_test_gate(int gid);
    void run_fault_list();
    void printoutput_fault_list();
    void printall_fault_list();
    void printstuckgate();

    int sum_faults();
    void print_fault_coverage();

    void node_fc_clear();

    vector<int> getinputnid();

    //-------------PODEM----------------------

    void set_podem_fault(nid_v5 f);
    nid_v5 Objective();
    nid_v5 Backtrace(nid_v5 nvk);
    bool isPI(int nid);
    int index_inputnid(int nid);

    void v5clear();
    void setinputv5(vector<int>& inv5);
    bool runv5gate(int gid);
    void imply(int nid,int v);
    void print_PI();
    bool success();
    bool failure();
    bool PODEM();
    
private:
    
    vector<cnode> allnodes;     //index by node id
    vector<cgate> allgates;     //index : no specific meaning, identified by its relative nodes.
    vector<int> inputnid;
    vector<int> outputnid;
    
    vector<int> stuckgid;
    set<fault_list> allfl;

    //---------PODEM-----------

    nid_v5 podem_fault;
    vector<int> D_frontier_gid;
    vector<int> PI_assign; //correspoding to inputnid


    
};