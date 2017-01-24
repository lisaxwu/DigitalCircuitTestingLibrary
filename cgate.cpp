#include "cnode.hpp"
#include "cgate.hpp"
#include <algorithm>

using namespace std;
    
bool cgate::iffinish(){
    return finish;
}

void cgate::addinnode(int inn){
    innodes.push_back(inn);
}
void cgate::addoutnode(int outn){
    outnode = outn;
}

vector<int> cgate::getinnodes(){
    return innodes;
}
int cgate::getoutnode(){
    return outnode;
}

gatetype cgate::gettype(){
    return gtype;
}

void cgate::setfinish(bool f){
    finish = f;
}

bool cgate::outvalue(vector<bool>& invalue){
    switch(gtype){
        case INV:
            return !invalue[0];
        case AND:
            return invalue[0]&&invalue[1];
        case OR:
            return invalue[0]||invalue[1];
        case BUF:
            return invalue[0];
        case NAND:
            return !(invalue[0]&&invalue[1]);
        case NOR:
            return !(invalue[0]||invalue[1]);
        default:
            return false;
    }
}

set<fault_list> cgate::get_outlinefl(set<fault_list> La,set<fault_list> Lb,bool a,bool b){
    set<fault_list> out;
    fault_list z;
    if(a!=controll_value && b!=controll_value){
        z.nid = outnode;
        z.s_a_v = controll_value ^ inversion;
        union_set(La,Lb,out);
        out.insert(z);
    }else if (a ==controll_value && b!=controll_value){
        z.nid = outnode;
        z.s_a_v = (!controll_value) ^ inversion;
        difference_set(La,Lb,out);
        out.insert(z);
    }else if (a !=controll_value && b==controll_value){
        z.nid = outnode;
        z.s_a_v = (!controll_value) ^ inversion;
        difference_set(Lb,La,out);
        out.insert(z);
    }else if (a ==controll_value && b==controll_value){
        z.nid = outnode;
        z.s_a_v = (!controll_value) ^ inversion;
        set<fault_list> tmp;
        intersection_set(La,Lb,out);
        out.insert(z);
    }
    return out;

}

set<fault_list> cgate::get_outlinefl(set<fault_list> La,bool a){
    set<fault_list> out=La;
    fault_list z;
    z.nid = outnode;
    z.s_a_v = !a;
    out.insert(z);
    return out;
}


void difference_set(set<fault_list>& A,set<fault_list>& B,set<fault_list>& result){
    //cout<<"diff"<<endl;
    std::set_difference(A.begin(), A.end(), B.begin(), B.end(),std::inserter(result, result.end()));
}

void intersection_set(set<fault_list>& A,set<fault_list>& B,set<fault_list>& result){
    //cout<<"intersec"<<endl;
    std::set_intersection(A.begin(), A.end(), B.begin(), B.end(),std::inserter(result, result.end()));
}

void union_set(set<fault_list>& A,set<fault_list>& B,set<fault_list>& result){
    //cout<<"union"<<endl;
    std::set_union(A.begin(), A.end(), B.begin(), B.end(),std::inserter(result, result.end()));
}


bool cgate::getv5finish(){
    return v5finish;
}

void cgate::setv5finish(bool f){
    v5finish = f;
}




