#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;
class Utility_chain_prefix_node{
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
class Utility_chain_sid_node{
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

void init_UMatrix_RMatrix(vector<vector<vector<int>>> &Utility_Matrix_set,vector<vector<vector<int>>> &Utility_Ru_Matrix_set,int &item_quantity,map<int,int> &single_item_iu,map<int,int> &single_item_swu){
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
        single_item_iu[item]=0;
        single_item_swu[item]=0;
    }
    if(Utility_table.empty()){
        exit(1);
    }
    in.close();

    in.open("uspandata.txt");
    if(in.fail()){
        cout << "input file opening failed";
        exit(1);
    }

    item_quantity = Utility_table.size();

    vector<bool> bitmap_item_exist(item_quantity,false);//這要拿來做SWU和IU
    vector<int> max_iu_item(item_quantity,0);

    vector<vector<int>> Utility_Matrix;
    vector<vector<int>> Utility_Ru_Matrix;

    vector<int> itemset_ru;


    int sequence_total_utility=0;
    vector<int> Itemset(item_quantity,0);
    int sid,itemset_id,quantity;
    int pre_sid=0 , pre_itemset_id=0;
    int run_times=0;
    while(!in.eof()) {
        in >> sid >> itemset_id >> item >> quantity;
        item--;

        if(run_times>0 && pre_sid!=sid){
            Utility_Matrix.push_back(Itemset);
            fill(Itemset.begin(),Itemset.end(),0);

            for(int i=0;i<item_quantity;i++){
                if(bitmap_item_exist[i]){
                    single_item_swu[i]+=sequence_total_utility;
                    single_item_iu[i]+=max_iu_item[i];
                }
            }
            fill(bitmap_item_exist.begin(),bitmap_item_exist.end(),0);
            fill(max_iu_item.begin(),max_iu_item.end(),0);

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

        }else if(run_times>0 && pre_itemset_id!=itemset_id){
            Utility_Matrix.push_back(Itemset);
            fill(Itemset.begin(),Itemset.end(),0);

        }

        //cout<<item;
        bitmap_item_exist[item]=true;
        if(max_iu_item[item]<Utility_table[item] * quantity){
            max_iu_item[item]=Utility_table[item] * quantity;
        }

        pre_sid = sid;
        pre_itemset_id = itemset_id;
        run_times++;
        Itemset[item] = Utility_table[item] * quantity;
        sequence_total_utility += Utility_table[item] * quantity;

    }
    if(run_times>0){ // 最後一個sid要加進matrix
        Utility_Matrix.push_back(Itemset);
        fill(Itemset.begin(),Itemset.end(),0);

        for(int i=0;i<item_quantity;i++){
            if(bitmap_item_exist[i]){
                single_item_swu[i]+=sequence_total_utility;
                single_item_iu[i]+=max_iu_item[i];
            }
        }
        fill(bitmap_item_exist.begin(),bitmap_item_exist.end(),0);
        fill(max_iu_item.begin(),max_iu_item.end(),0);

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
void init_level1(const vector<vector<vector<int>>> &Utility_Matrix_set,const vector<vector<vector<int>>> &Utility_Ru_Matrix_set,const int &item_quantity,const int &threshold,const map<int,int> &single_item_swu,map<int,Seq_table*> &single_item_Utility_Chain){
    Seq_table * Seq_Table;
    int sid_peu,iu_add_ru,iu;
    int Pue,Iu;
    for(int item=0;item<item_quantity;item++){
        //cout<<single_item_swu.at(item)<<"\n\n";
        if(single_item_swu.at(item)<threshold){
            continue;
        }
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
//            cout<<Seq_Table->seq<<"\n";
//            cout<<Seq_Table->Peu<<"\n";
//            cout<<Seq_Table->U_t<<"\n\n";
        }
        single_item_Utility_Chain[item]=Seq_Table;
        //delete Seq_Table;

    }
}


int main() {
    int item_quantity,threshold=100;//item-> a=0,b=1,...
    vector<vector<vector<int>>> Utility_Matrix_set;
    vector<vector<vector<int>>> Utility_Ru_Matrix_set;
    map<int,int> single_item_iu,single_item_swu;
    init_UMatrix_RMatrix(Utility_Matrix_set,Utility_Ru_Matrix_set,item_quantity,single_item_iu,single_item_swu);
    map<int,Seq_table*> single_item_Utility_Chain;
    init_level1(Utility_Matrix_set,Utility_Ru_Matrix_set,item_quantity,threshold,single_item_swu,single_item_Utility_Chain);


    //cout<<single_item_iu[5]<<" "<<single_item_swu[5];
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
