#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "circuit.hpp"

using namespace std;


gatetype str2gt(string str){
    gatetype gt;
    if (str.compare("INV")==0){
        gt = INV;
        return gt;
    }else if (str.compare("AND")==0){
        gt = AND;
        return gt;
    }else if (str.compare("OR")==0){
        
        gt = OR;
        return gt;
    }else if (str.compare("BUF")==0){
        
        gt = BUF;
        return gt;
    }else if (str.compare("NAND")==0){
        
        gt = NAND;
        return gt;
    }else if (str.compare("NOR")==0){
        
        gt = NOR;
        return gt;
    }else{
        cout<<str<<" -- error: no such gate!!\n";
        gt = INV;
        return gt;
    }
}

vector<bool> str2vec(string str){
    vector<bool> result;
    for(int i=0;i<str.length();i++){
        if(str[i]=='1'){
            result.push_back(true);
        }else if(str[i]=='0'){
            result.push_back(false);
        }
    }
    return result;
}

bool i2b(int i){
    if(i==0){
        return false;
    }else{
        return true;
    }
}



int main(int argc, char* argv[]){
    
    if (argc < 3) {
        cout << "enter circuit file name and test file name !!" << endl;
        return 1;
    }
    
    string filename(argv[1]);
    string line;
    ifstream myfile(filename);
    
    circuit csim = circuit();
    
    if (myfile.is_open())
    {
        
        while (getline(myfile,line))
        {
            
            istringstream iss(line);
            string g;
            iss>>g;
            
            if(g.compare("INPUT")==0){
                
                int n;
                while(iss>>n){
                    csim.addinput(n);
                }
                continue;
            }
            
            if(g.compare("OUTPUT")==0){
                
                int n;
                while(iss>>n){
                    csim.addoutput(n);
                }
                break;
            }
            
            gatetype thegate = str2gt(g);
            
            int n;
            vector<int> nid;
            while(iss>>n){
                nid.push_back(n);
            }
            
            csim.add_gate_nodes(thegate,nid);
            
        }
        myfile.close();
    }else{
        cout << "Unable to open file";
        return 1;
    }
    
    csim.clear();
    //csim.printbygates();
    
    string testfile(argv[2]);
    ifstream mytest(testfile);
    if(mytest.is_open()){
        while (getline(mytest,line)){
            if(line.empty()){
                break;
            }
            vector<bool> invalue = str2vec(line);
            csim.run(invalue);
            csim.printoutput();
            
            csim.run_fault_list();
            csim.printoutput_fault_list();
            csim.print_fault_coverage();
            cout<<"----------------------------------"<<endl;
            csim.clear();
        }
        mytest.close();
    }

    
    nid_v5 f;
    f.nid = atoi(argv[3]);
    f.v5 = atoi(argv[4]);
    csim.set_podem_fault(f);
    csim.PODEM();

       
    return 0;
    
}

