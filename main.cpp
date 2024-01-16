#include <iostream>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

void init_UMatrix_RMatrix(vector<vector<vector<int>>> &Utility_Matrix_set,vector<vector<vector<int>>> &Utility_Ru_Matrix_set){
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

    int item_quantity = Utility_table.size()+1;
    int sequence_total_utility=0;
    vector<int> Itemset(item_quantity,0);
    int sid,itemset_id,quantity;
    int pre_sid=0 , pre_itemset_id=0;
    int run_times=0;
    while(!in.eof()) {
        in >> sid >> itemset_id >> item >> quantity;
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


    //    for(int i=0;i<Utility_table.size();i++){
    //        cout<<Utility_Matrix_Secquence[i]<<" ";
    //
    //    }
    //    for(pair<int,int> i:Utility_table){
    //        cout<<i.first<<" "<<i.second<<endl;
    //    }
    //    cout<<Utility_table[2];


}
int main() {
    vector<vector<vector<int>>> Utility_Matrix_set;
    vector<vector<vector<int>>> Utility_Ru_Matrix_set;
    init_UMatrix_RMatrix(Utility_Matrix_set,Utility_Ru_Matrix_set);

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
