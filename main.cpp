#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;
class Utility_chain_prefix_node {
public:
    int tid_prefix;
    int acu;
    int ru;
    Utility_chain_prefix_node *next;
    Utility_chain_prefix_node(){
        tid_prefix=0;
        acu=0;
        ru=0;
        next = nullptr;
    }
};
class Utility_chain_sid_node {
public:
    int sid;
    int peu_sid;
    int iu;
    Utility_chain_prefix_node *prefix_node_head;
    Utility_chain_sid_node *next;
    Utility_chain_sid_node(){
        sid=0;
        peu_sid = 0;
        iu=0;
        prefix_node_head =nullptr;
        next =nullptr;

    }
};
class Seq_table{
public:
    string seq;
    int Peu;
    int U_t;
    Utility_chain_sid_node *sid_node_head;
    Seq_table(){
        seq = "";
        Peu=0;
        U_t=0;
        sid_node_head=nullptr;
    }
};

void init_UMatrix_RMatrix(vector<vector<vector<int>>> &Utility_Matrix_set,vector<vector<vector<int>>> &Utility_Ru_Matrix_set,int &item_quantity){
    ifstream in;
    in.open("uspanutb.txt");
    if(in.fail()){
        cout << "input file opening failed";
        exit(1);
    }
    map<int,int> Utility_table;
    int item,utility;
    while(!in.eof()) {
        in >> item >> utility;
        item--;
        Utility_table[item]=utility;
    }
    if(Utility_table.empty()){
        exit(1);
    }
    in.close();


    in.open("uspandata.txt");


    vector<vector<int>> Utility_Matrix;
    vector<vector<int>> Utility_Ru_Matrix;

    vector<int> itemset_ru;

    item_quantity = Utility_table.size();
    int sequence_total_utility=0;
    vector<int> Itemset(item_quantity,0);
    int sid,itemset_id,quantity;
    int pre_sid=0 , pre_itemset_id=0;
    int run_times=0;
    while(!in.eof()) {
        in >> sid >> itemset_id >> item >> quantity;
        item--;
        if(run_times>0 && pre_itemset_id!=itemset_id){
            Utility_Matrix.push_back(Itemset);
            fill(Itemset.begin(),Itemset.end(),0);
        }
        if(run_times>0 && pre_sid!=sid){

            for(int i=0;i<Utility_Matrix.size();i++){
                for(int j=0;j<Utility_Matrix[i].size();j++){
                    sequence_total_utility-=Utility_Matrix[i][j];
                    itemset_ru.push_back(sequence_total_utility);
                }
                Utility_Ru_Matrix.push_back(itemset_ru);
                itemset_ru.clear();
            }

            Utility_Matrix_set.push_back(Utility_Matrix);
            Utility_Matrix.clear();
            Utility_Ru_Matrix_set.push_back(Utility_Ru_Matrix);
            Utility_Ru_Matrix.clear();
            sequence_total_utility = 0 ;

        }
        pre_sid = sid;
        pre_itemset_id = itemset_id;
        run_times++;

        Itemset[item] = Utility_table[item] * quantity;
        sequence_total_utility += Utility_table[item] * quantity;
    }
    if(run_times>0){ // 最後一個sid要加進matrix
        Utility_Matrix.push_back(Itemset);

        for(int i=0;i<Utility_Matrix.size();i++){
            for(int j=0;j<Utility_Matrix[i].size();j++){
                sequence_total_utility-=Utility_Matrix[i][j];
                itemset_ru.push_back(sequence_total_utility);
            }
            Utility_Ru_Matrix.push_back(itemset_ru);
            itemset_ru.clear();
        }

        Utility_Matrix_set.push_back(Utility_Matrix);
        Utility_Matrix.clear();
        Utility_Ru_Matrix_set.push_back(Utility_Ru_Matrix);
        Utility_Ru_Matrix.clear();
        sequence_total_utility = 0 ;
    }
    in.close();

}
void init_level1(const vector<vector<vector<int>>> &Utility_Matrix_set,const vector<vector<vector<int>>> &Utility_Ru_Matrix_set,const int &item_quantity){
    Seq_table * Seq_Table;
    int sid_peu,iu_add_ru,iu;
    int Pue,Iu;
    for(int item=0;item<item_quantity;item++){//test
        Seq_Table = new Seq_table;
        Seq_Table->seq += to_string(item);
        Seq_Table->sid_node_head = new Utility_chain_sid_node;

        Utility_chain_sid_node *sid_node_it;
        sid_node_it = Seq_Table->sid_node_head;

        sid_node_it->prefix_node_head = new Utility_chain_prefix_node;
        Utility_chain_prefix_node *prefix_node_it = sid_node_it->prefix_node_head;

        for(int i=0;i<Utility_Matrix_set.size();i++){
            sid_peu=0;
            iu=0;
            for(int j=0;j<Utility_Matrix_set[i].size();j++){
                if(Utility_Matrix_set[i][j][item]>0){
                    iu_add_ru=Utility_Matrix_set[i][j][item]+Utility_Ru_Matrix_set[i][j][item];
                    if(iu_add_ru>sid_peu && Utility_Ru_Matrix_set[i][j][item]>0){
                        sid_peu=iu_add_ru;
                    }

                    if(Utility_Matrix_set[i][j][item]>iu){
                        iu=Utility_Matrix_set[i][j][item];
                    }

                    prefix_node_it->tid_prefix = j;
                    prefix_node_it->acu = Utility_Matrix_set[i][j][item];
                    prefix_node_it->ru = Utility_Ru_Matrix_set[i][j][item];
                    prefix_node_it->next = new Utility_chain_prefix_node;
                    prefix_node_it = prefix_node_it->next;
                }
            }
            if(sid_node_it->prefix_node_head != prefix_node_it){
                sid_node_it->peu_sid=sid_peu;
                sid_node_it->sid = i;
                sid_node_it->iu = iu;

                sid_node_it->next = new Utility_chain_sid_node;
                sid_node_it = sid_node_it->next;

                sid_node_it->prefix_node_head = new Utility_chain_prefix_node;
                prefix_node_it = sid_node_it->prefix_node_head;
            }
        }

        if(Seq_Table->sid_node_head != sid_node_it){
            Pue=Seq_Table->sid_node_head->peu_sid;
            Iu = Seq_Table->sid_node_head->iu;
            sid_node_it = Seq_Table->sid_node_head;
            while(sid_node_it->next){
                sid_node_it=sid_node_it->next;
                Pue+=sid_node_it->peu_sid;
                Iu+=sid_node_it->iu;
            }
            Seq_Table->Peu = Pue;
            Seq_Table->U_t = Iu;
            cout<<Seq_Table->Peu<<"\n";
            cout<<Seq_Table->U_t<<"\n\n";
        }

    }
}

int main() {
    int item_quantity;//item-> a=0,b=1,...
    vector<vector<vector<int>>> Utility_Matrix_set;
    vector<vector<vector<int>>> Utility_Ru_Matrix_set;
    init_UMatrix_RMatrix(Utility_Matrix_set,Utility_Ru_Matrix_set,item_quantity);
    init_level1(Utility_Matrix_set,Utility_Ru_Matrix_set,item_quantity);

    //cout<<item_quantity;
//    for(int i=0;i<Utility_Matrix_set.size();i++){
//        for(int j=0;j<Utility_Matrix_set[i].size();j++){
//            for(int k=0;k<Utility_Matrix_set[i][j].size();k++){
//                cout<<Utility_Matrix_set[i][j][k]<<" ";
//            }
//            cout<<endl;
//        }
//        cout<<"--------------------\n";
//    }
//    cout<<"**************************\n";
//    for(int i=0;i<Utility_Ru_Matrix_set.size();i++){
//        for(int j=0;j<Utility_Ru_Matrix_set[i].size();j++){
//            for(int k=0;k<Utility_Ru_Matrix_set[i][j].size();k++){
//                cout<<Utility_Ru_Matrix_set[i][j][k]<<" ";
//            }
//            cout<<endl;
//        }
//        cout<<"--------------------\n";
//    }
    return 0;
}
