#include "cnode.hpp"
#include "cgate.hpp"
using namespace std;


void print_fault_list(set<fault_list> L){
    set<fault_list>::iterator it;
    for(it=L.begin();it!=L.end();it++){
        cout<<(*it).nid<<"\t"<<(*it).s_a_v<<endl;
    }
    cout<<endl;   
}

bool operator<(const fault_list& l1, const fault_list& l2)
{
    return l1.nid < l2.nid;
}


cnode::cnode(int id){
    value = false;
    ready = false;
    nid = id;
    sat0=0;
    sat1=0;

    v5=3;
}
    
bool cnode::ifready(){
    return ready;
}
    
void cnode::setready(bool r){
    ready = r;
}
    
void cnode::setvalue(bool v){
    value = v;
}
    
bool cnode::getvalue(){
    return value;
}
    
void cnode::inputgate(int gid){
    asinput.push_back(gid);
}
    
void cnode::outputgate(int gid){
    asoutput = gid;
}

int cnode::getnid(){
    return nid;
}

vector<int> cnode::getinputgid(){
    return asinput;
}

int cnode::getoutputgid(){
    return asoutput;
}



void cnode::add_fault_list(fault_list f){
    L.insert(f);
}

void cnode::add_fault_list(set<fault_list> f){
    set<fault_list>::iterator it;
    for(it = f.begin();it!=f.end();it++){
        L.insert(*it);
    }
}

set<fault_list> cnode::get_fault_list(){
    return L;
}

void cnode::clear_fault_list(){
    L.clear();
}

void cnode::setsat0(){
    sat0=1;
}

void cnode::setsat1(){
    sat1=1;
}

int cnode::getsat0(){
    return sat0;
}
int cnode::getsat1(){
    return sat1;
}
void cnode::clearsat(){
    sat0=0;
    sat1=0;
}

void cnode::setv5(int v){
    v5 = v;
}

int cnode::getv5(){
    return v5;
}

bool cnode::getv5ready(){
    return v5_ready;
}
    
void cnode::setv5ready(bool r){
    v5_ready = r;
}
