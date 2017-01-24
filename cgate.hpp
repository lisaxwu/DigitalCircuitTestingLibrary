#include <vector>
#include <set>
using namespace std;

struct fault_list;

enum gatetype {INV = 1, AND = 2, OR = 3, BUF = 4, NAND = 5, NOR = 6};

class cnode;

class cgate{ //hhh
public:
    cgate(gatetype gt,int id){
        gtype = gt;
        finish = false;
        gid = id;
        switch(gtype){
            case INV:
                controll_value = false;
                inversion = true;
                break;
            case AND:
                controll_value = false;
                inversion = false;
                break;
            case OR:
                controll_value = true;
                inversion = false;
                break;
            case BUF:
                controll_value = false;
                inversion = false;
                break;
            case NAND:
                controll_value = false;
                inversion = true;
                break;
            case NOR:
                controll_value = true;
                inversion = true;
                break;
            default:
                break;
        }
    }
    
    void addinnode(int nid);
    void addoutnode(int nid);
    
    vector<int> getinnodes();
    int getoutnode();
    
    gatetype gettype();
    
    bool iffinish();
    void setfinish(bool f);
    
    bool outvalue(vector<bool>& invalue);

    set<fault_list> get_outlinefl(set<fault_list> La, set<fault_list> Lb,bool a,bool b);
    set<fault_list> get_outlinefl(set<fault_list> La,bool a);
    
    bool controll_value;
    bool inversion;

    //-----------------------------
    bool getv5finish();
    void setv5finish(bool f);

private:
    gatetype gtype;
    int gid;
    vector<int> innodes;
    int outnode;
    bool finish;


    //--------PODEM-----------
    bool v5finish;


};

void difference_set(set<fault_list>& A,set<fault_list>& B,set<fault_list>& result);
void intersection_set(set<fault_list>& A,set<fault_list>& B,set<fault_list>& result);
void union_set(set<fault_list>& A,set<fault_list>& B,set<fault_list>& result);








