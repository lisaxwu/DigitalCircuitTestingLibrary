#include "circuit.hpp"

using namespace std;


void circuit::add_gate_nodes(gatetype gt,vector<int>& nid){
    
    int gid = allgates.size()+1;
    allgates.push_back(cgate(gt,gid));
    
    int ns = nid.size();
    
    for(int i=0;i<ns;i++){
        int id = allnodes.size();
        int count = nid[i] - allnodes.size();
        for(int j=0;j<count;j++){
            allnodes.push_back(cnode(id+j+1));
        }
    }
    
    //add nodes to the gate
    
    vector<cgate>::iterator itg = allgates.end()-1;
    
    for(int i=0;i<(ns-1);i++){
        itg->addinnode(nid[i]);
    }
    itg->addoutnode(nid[ns-1]);    //last one : output node
    
    //add gate to these nodes;
    for(int i=0;i<(ns-1);i++){
        allnodes[nid[i]-1].inputgate(gid);
    }
    allnodes[nid[ns-1]-1].outputgate(gid);
    
    
}

void circuit::printbygates(){
    int n = allgates.size();
    int k = allnodes.size();
    cout<<k<<"nodes "<<n<<"gates"<<endl;
    
    vector<cgate>::iterator itg = allgates.begin();
    
    for(;itg<allgates.end();itg++){
        cout<<"gatetype"<<itg->gettype()<<" ";
        
        vector<int> innodes = itg->getinnodes();
        for(int i=0;i<innodes.size();i++){
            cout<<innodes[i]<<"r"<<allnodes[innodes[i]-1].ifready()<<
            "v"<<allnodes[innodes[i]-1].getvalue()<<" ";
        }
        cout<<"##";
        int outnode = itg->getoutnode();
        cout<<outnode<<"r"<<allnodes[outnode-1].ifready()<<
        "v"<<allnodes[outnode-1].getvalue()<<endl;
    }
    
    vector<int>::iterator it = inputnid.begin();
    cout<<"input ";
    for(;it<inputnid.end();it++){
        cout<<*it<<"v"<<allnodes[*it-1].getvalue()<<" ";
    }
    cout<<endl;
    cout<<"output ";
    for(it=outputnid.begin();it<outputnid.end();it++){
        cout<<*it<<"v"<<allnodes[*it-1].getvalue()<<" ";
    }
    cout<<endl;
    
        
}

void circuit::printoutput(){
    for(int i=0;i<outputnid.size();i++){
        cout<<allnodes[outputnid[i]-1].getvalue();
    }
    cout<<endl;
}


void circuit::addinput(int nid){
    if(nid>0){
        inputnid.push_back(nid);
        PI_assign.push_back(3);
    }
}

void circuit::addoutput(int nid){
    if(nid>0)
        outputnid.push_back(nid);
}

bool circuit::rungate(int gid){
    if(allgates[gid-1].iffinish()){
        return true;
    }
    vector<int> innodes = allgates[gid-1].getinnodes();
    vector<bool> invalue;
    for(int i=0;i<innodes.size();i++){
        if(!allnodes[innodes[i]-1].ifready()){
            return false;
        }
        invalue.push_back(allnodes[innodes[i]-1].getvalue());
    }
    int outnode = allgates[gid-1].getoutnode();
    allnodes[outnode-1].setready(true);
    allnodes[outnode-1].setvalue(allgates[gid-1].outvalue(invalue));
    allgates[gid-1].setfinish(true);
    return true;
}


void circuit::setinputvalue(vector<bool>& invalue){
    for(int i=0;i<inputnid.size();i++){
        allnodes[inputnid[i]-1].setready(true);
        allnodes[inputnid[i]-1].setvalue(invalue[i]);
    }
}

void circuit::furtherstuck(vector<int>& nid){
    for(int i=0;i<nid.size();i++){
        vector<int> tmp = allnodes[nid[i]-1].getinputgid();
        stuckgid.reserve(stuckgid.size()+tmp.size());
        stuckgid.insert(stuckgid.end(),tmp.begin(),tmp.end());
    }
}


void circuit::run(vector<bool>& invalue){
    
    stuckgid.clear();
    setinputvalue(invalue);
    
    furtherstuck(inputnid);
    
    while(stuckgid.size()>0){
        vector<int> newreadynodes;
        
        vector<int>::iterator it=stuckgid.begin();
        while(it<stuckgid.end()){
            if(rungate(*it)){
                newreadynodes.push_back(allgates[(*it)-1].getoutnode());
                stuckgid.erase(it);
            }else{
                it++;
            }
        }
        furtherstuck(newreadynodes);
    }
    
}

void circuit::setinput_fault_list(){
    for(int i=0;i<inputnid.size();i++){
        fault_list L;
        L.nid = inputnid[i];
        bool v = allnodes[inputnid[i]-1].getvalue();
        L.s_a_v = !v;
        allnodes[inputnid[i]-1].add_fault_list(L);
    }
}

bool circuit::detective_test_gate(int gid){
    
    vector<int> innodes = allgates[gid-1].getinnodes();
    int outnode = allgates[gid-1].getoutnode();
    if(!allnodes[outnode-1].get_fault_list().empty()){
        //cout<<gid<<"already"<<endl;
        return true;
    }
    
    if(innodes.size()==2){
        set<fault_list> La = allnodes[innodes[0]-1].get_fault_list();
        set<fault_list> Lb = allnodes[innodes[1]-1].get_fault_list();
        if(La.empty() || Lb.empty()){
            return false;
        }
        set<fault_list> Lz = allgates[gid-1].get_outlinefl(La,Lb,allnodes[innodes[0]-1].getvalue(),allnodes[innodes[1]-1].getvalue());
        allnodes[outnode-1].add_fault_list(Lz);
    }else{
        set<fault_list> La = allnodes[innodes[0]-1].get_fault_list();
        if(La.empty()){
            return false;
        }
        set<fault_list> Lz = allgates[gid-1].get_outlinefl(La,allnodes[outnode-1].getvalue());
        
        allnodes[outnode-1].add_fault_list(Lz);
    }
     
    return true;
}

void circuit::run_fault_list(){
    
    setinput_fault_list();

    stuckgid.clear();
    furtherstuck(inputnid);
    
    while(stuckgid.size()>0){
        vector<int> newreadynodes;

        vector<int>::iterator it=stuckgid.begin();
        while(it<stuckgid.end()){
            if(detective_test_gate(*it)){
                newreadynodes.push_back(allgates[(*it)-1].getoutnode());
                stuckgid.erase(it);
                
            }else{
                it++;
            }
        }

        furtherstuck(newreadynodes);
    }

    for(int i=0;i<outputnid.size();i++){
        set<fault_list> L = allnodes[outputnid[i]-1].get_fault_list();
        union_set(allfl,L,allfl);
    }
    set<fault_list>::iterator fit;
    for(fit = allfl.begin();fit!=allfl.end();fit++){
        if((*fit).s_a_v == 0){
            allnodes[(*fit).nid-1].setsat0();
        }else{
            allnodes[(*fit).nid-1].setsat1();
        }   
    }

}

int circuit::sum_faults(){
    int fault_counts = 0;
    for(int i=0;i<allnodes.size();i++){
        fault_counts = fault_counts + allnodes[i].getsat0()+ allnodes[i].getsat1();
    }
    return fault_counts;
}

void circuit::print_fault_coverage(){
    int fc = sum_faults();
    //cout<<"fault counts: "<< fc << "   coverage: ";
    cout<<(float)(fc+0.0)/(allnodes.size()*2)<<endl;
}

void circuit::printoutput_fault_list(){
    print_fault_list(allfl);
}

void circuit::printall_fault_list(){
    cout<<"all fl:"<<endl;
    for(int i=0;i<allnodes.size();i++){
        cout<<"node"<<i+1<<":";
        set<fault_list> L = allnodes[i].get_fault_list();
        print_fault_list(L);
    }
    cout<<endl;
}

void circuit::printstuckgate(){
    cout<<"stuckgid: ";
    for(int i=0;i<stuckgid.size();i++){
        cout<<stuckgid[i]<<" ";
    }
    cout<<endl;
}

void circuit::clear(){

    vector<cnode>::iterator itn = allnodes.begin();
    for(;itn<allnodes.end();itn++){
        (*itn).setvalue(false);
        (*itn).setready(false);
        (*itn).clear_fault_list();
        (*itn).setv5(3);
    }
    vector<cgate>::iterator itg = allgates.begin();
    for(;itg<allgates.end();itg++){
        (*itg).setfinish(false);
    }
    
    stuckgid.clear();
    allfl.clear();
}

void circuit::node_fc_clear(){
    vector<cnode>::iterator itn = allnodes.begin();
    for(;itn<allnodes.end();itn++){
        (*itn).clearsat();
    }
}

vector<int> circuit::getinputnid(){
    return inputnid;
}

//----------------------PODEM--------------------------

bool v52b(int v5){
    if (v5 == 0){
        return false;
    }
    if (v5 == 1){
        return true;
    }
    return false;
}

int b2v5(bool b){
    if (b){
        return 1;
    }else{
        return 0;
    }
    return 3;
}

int intxor(int v,int i){  //v xor i vi'+v'i
    if (v==i){
        return 0;
    }else{
        return 1;
    }
}

int v5reverse(int v){
    if(v==0){
        return 1;
    }
    if(v==1){
        return 0;
    }
    if(v==3){
        return 3;
    }
    if(v==4){
        return 5;
    }
    if(v==5){
        return 4;
    }
    return 3;
}

int v5logic(gatetype gt,int v,int u){
    
    switch(gt){
        case AND:
            if(v==0 || u==0){
                return 0;
            }
            if(v==1){
                return u;
            }
            if(u==1){
                return v;
            }
            if(u==3 || v==3){
                return 3;
            }
            if(v==u){
                return v;
            }
            return 0;

        case NAND:
            if(v==0 || u==0){
                return 1;
            }
            if(v==1){
                return v5reverse(u);
            }
            if(u==1){
                return v5reverse(v);
            }
            if(u==3 || v==3){
                return 3;
            }
            if(v==u){
                return v5reverse(v);
            }
            return 1;

        case OR:
            if(v==1 || u==1){
                return 1;
            }
            if(v==0){
                return u;
            }
            if(u==0){
                return v;
            }
            if(u==3 || v==3){
                return 3;
            }
            if(v==u){
                return v;
            }
            return 1;

        case NOR:
            if(v==1 || u==1){
                return 0;
            }
            if(v==0){
                return v5reverse(u);
            }
            if(u==0){
                return v5reverse(v);
            }
            if(u==3 || v==3){
                return 3;
            }
            if(v==u){
                return v5reverse(v);
            }
            return 0;       
        default:
            return 3;
    }
}

string v5tostr(int v){
    if(v==0){
        return "0";
    }
    if(v==1){
        return "1";
    }
    if(v==3){
        return "X";
    }
    if(v==4){
        return "D";
    }
    if(v==5){
        return "D'";
    }
    return "$";
}

int toD(int v){
    if(v==0){
        return 5;
    }
    if(v==1){
        return 4;
    }
    return v;
}

void circuit::set_podem_fault(nid_v5 f){
    podem_fault = f;
    //cout<<"PIsize"<<PI_assign.size()<<endl;
    for(int i=0;i<PI_assign.size();i++){
        PI_assign[i] = 3;
    }
    D_frontier_gid.push_back(-1);
    v5clear();
}

bool circuit::isPI(int nid){
    for(int i=0;i<inputnid.size();i++){
        if (nid == inputnid[i]){
            return true;
        }
    }
    return false;
}

nid_v5 circuit::Objective(){
    nid_v5 nv;
    int l = podem_fault.nid;
    if(allnodes[l-1].getv5() == 3){
        nv.nid = l;
        nv.v5 = (podem_fault.v5 + 1)%2;
        return nv;
    }
    
    // select the first gate G from D-frontier
    int gid = D_frontier_gid[0];
    //select an input(=x) j of G
    vector<int>innodes = allgates[gid-1].getinnodes();
    for(int i =0;i<innodes.size();i++){
        if(allnodes[innodes[i]-1].getv5() == 3){
            nv.nid = innodes[i];
            nv.v5 = b2v5(!allgates[gid-1].controll_value);
            return nv;
        }
    }
    
    nv.nid = -1;
    nv.v5 = 3;
    return nv;
}

nid_v5 circuit::Backtrace(nid_v5 nvk){
    int v = nvk.v5;
    int k = nvk.nid;
    while(!isPI(k)){
        int gid = allnodes[k-1].getoutputgid();
        vector<int> innodes = allgates[gid-1].getinnodes();
        for(int i =0;i<innodes.size();i++){
            if(allnodes[innodes[i]-1].getv5() == 3){
                k = innodes[i];
                v = intxor(v,b2v5(allgates[gid-1].inversion));
                break;
            }
        }
    }
    nvk.nid = k;
    nvk.v5 = v;
    return nvk;
}

int circuit::index_inputnid(int nid){
    for (int i=0;i<inputnid.size();i++){
        if(inputnid[i]==nid){
            return i;
        }
    }
    return -1;
}

void circuit::v5clear(){
    vector<cnode>::iterator itn = allnodes.begin();
    for(;itn<allnodes.end();itn++){
        (*itn).setv5ready(false);
        (*itn).setv5(3);
    }
    vector<cgate>::iterator itg = allgates.begin();
    for(;itg<allgates.end();itg++){
        (*itg).setv5finish(false);
    }
}

void circuit::setinputv5(vector<int>& inv5){
    for(int i=0;i<inputnid.size();i++){
        allnodes[inputnid[i]-1].setv5ready(true);
        allnodes[inputnid[i]-1].setv5(inv5[i]);
    }
}


bool circuit::runv5gate(int gid){
    if(allgates[gid-1].getv5finish()){
        return true;
    }
    vector<int> innodes = allgates[gid-1].getinnodes();
    int outnode = allgates[gid-1].getoutnode();
    gatetype gt = allgates[gid-1].gettype();
    int l = podem_fault.nid;
    int fv = podem_fault.v5;

    switch(gt){
        case BUF:
            if (allnodes[innodes[0]-1].getv5ready()){
                int v = allnodes[innodes[0]-1].getv5();
                if (outnode == l && v!=fv){
                    v = toD(v);
                }
                allnodes[outnode-1].setv5(v);
                allnodes[outnode-1].setv5ready(true);
                allgates[gid-1].setv5finish(true);
                return true;
            }
            return false;

        case INV:
            if (allnodes[innodes[0]-1].getv5ready()){
                int v = allnodes[innodes[0]-1].getv5();
                v = v5reverse(v); 
                if (outnode == l && v!=fv){
                    v = toD(v);
                }
                allnodes[outnode-1].setv5(v);
                allnodes[outnode-1].setv5ready(true);
                allgates[gid-1].setv5finish(true);
                return true;
            }
            return false;

        case AND:
            if (allnodes[innodes[0]-1].getv5ready() && allnodes[innodes[1]-1].getv5ready()){
                int v = allnodes[innodes[0]-1].getv5();
                int u = allnodes[innodes[1]-1].getv5();
                v = v5logic(AND,v,u); 
                if (outnode == l && v!=fv){
                    v = toD(v);
                }
                allnodes[outnode-1].setv5(v);
                allnodes[outnode-1].setv5ready(true);
                allgates[gid-1].setv5finish(true);
                return true;
            }
            return false;
            
        case OR:
            if (allnodes[innodes[0]-1].getv5ready() && allnodes[innodes[1]-1].getv5ready()){
                int v = allnodes[innodes[0]-1].getv5();
                int u = allnodes[innodes[1]-1].getv5();
                v = v5logic(OR,v,u); 
                if (outnode == l && v!=fv){
                    v = toD(v);
                }
                allnodes[outnode-1].setv5(v);
                allnodes[outnode-1].setv5ready(true);
                allgates[gid-1].setv5finish(true);
                return true;
            }
            return false;

        case NAND:
            if (allnodes[innodes[0]-1].getv5ready() && allnodes[innodes[1]-1].getv5ready()){
                int v = allnodes[innodes[0]-1].getv5();
                int u = allnodes[innodes[1]-1].getv5();
                v = v5logic(NAND,v,u); 
                if (outnode == l && v!=fv){
                    v = toD(v);
                }
                allnodes[outnode-1].setv5(v);
                allnodes[outnode-1].setv5ready(true);
                allgates[gid-1].setv5finish(true);
                return true;
            }
            return false;
            
        case NOR:
            if (allnodes[innodes[0]-1].getv5ready() && allnodes[innodes[1]-1].getv5ready()){
                int v = allnodes[innodes[0]-1].getv5();
                int u = allnodes[innodes[1]-1].getv5();
                v = v5logic(NOR,v,u); 
                if (outnode == l && v!=fv){
                    v = toD(v);
                }
                allnodes[outnode-1].setv5(v);
                allnodes[outnode-1].setv5ready(true);
                allgates[gid-1].setv5finish(true);
                return true;
            }
            return false;
            
        default:
            return false;
    }


}

void circuit::imply(int nid,int v){

    int l = podem_fault.nid;

    int index = index_inputnid(nid);
    PI_assign[index] = v;

    if (nid == l){
        PI_assign[index] = toD(v);
    }

    v5clear();
    stuckgid.clear();
    setinputv5(PI_assign);
    furtherstuck(inputnid);
    
    while(stuckgid.size()>0){
        vector<int> newreadynodes;
        vector<int>::iterator it=stuckgid.begin();
        while(it<stuckgid.end()){
            if(runv5gate(*it)){
                newreadynodes.push_back(allgates[(*it)-1].getoutnode());
                stuckgid.erase(it);
            }else{
                it++;
            }
        }
        furtherstuck(newreadynodes);
    }

    D_frontier_gid.clear();

    for (int i=0;i<allgates.size();i++){

        vector<int> innodes = allgates[i].getinnodes();
        int outnode = allgates[i].getoutnode();

        int out = allnodes[outnode-1].getv5();
        if (out != 3){
            continue;
        }

        for(int k=0;k<innodes.size();k++){
            int v = allnodes[innodes[k]-1].getv5();
            if(v==4 || v==5){
                D_frontier_gid.push_back(i+1);
                break;
            }
        }
    }

    cout<<"imply "<<nid<<"-"<<v<<" {"<<endl;
    cout<<"l:"<<v5tostr(allnodes[l-1].getv5())<<endl;
    print_PI();
    cout<<"D-frontier: ";
    for(int i=0;i<D_frontier_gid.size();i++){
        cout<<D_frontier_gid[i]<<" ";
    }
    cout<<"}"<<endl;

}


void circuit::print_PI(){
    cout<<"PI: ";
    for(int i=0;i<PI_assign.size();i++){
        cout<<v5tostr(PI_assign[i]);
    }
    cout<<endl;
}

bool circuit::success(){
    for(int i=0;i<outputnid.size();i++){
        int v = allnodes[outputnid[i]-1].getv5();
        if (v==4 || v==5){
            cout<<"success!! ";
            print_PI();
            return true;
        }
    }
    return false;
}

bool circuit::failure(){

    int l = podem_fault.nid;
    int fv = podem_fault.v5;

    if (allnodes[l-1].getv5() == fv){
        cout<<"*failure: l wrong"<<endl<<endl;
        return true;
    }
    if (allnodes[l-1].getv5()!=3 && D_frontier_gid.empty()){
        cout<<"*failure: D-frontier empty"<<endl<<endl;
        return true;
    }

    for(int i=0;i<outputnid.size();i++){
        int v = allnodes[outputnid[i]-1].getv5();
        if (v==3){
            return false;
        }
    }
    cout<<"*failure: outputs are all 0/1"<<endl<<endl;
    return true;
}

bool circuit::PODEM(){
    cout<<"podem"<<endl;
    if(success()){
        return true;
    }
    if(failure()){
        return false;
    }

    cout<<endl;
    nid_v5 obj = Objective();
    nid_v5 pi = Backtrace(obj);
    imply(pi.nid,pi.v5);

    if(PODEM()){
        //cout<<"haha_mid-1"<<endl;
        return true;
    }

    cout<<"reverse!!"<<endl;
    imply(pi.nid,v5reverse(pi.v5));
    if(PODEM()){
        //cout<<"haha_mid-2"<<endl;
        return true;
    }
    imply(pi.nid,3);
    cout<<"NO test vector on this tree path!!"<<endl;
    return false;
}






