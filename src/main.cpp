#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
#include<set>
#include <stdexcept>

using namespace std;
int main(int argc, char* argv[]){
    bool is2way;
    vector<int> cell_size;
    vector<vector<set<int>>> net;
    vector<vector<int>> cell;
    int max_area;
    int num_cell;
    int num_net;
    string _;
    ifstream in_file;
    in_file.open(argv[1]);    //open <.in file>

    in_file >> max_area;
    getline(in_file, _); //new line
    getline(in_file, _); //.cell

    in_file >> num_cell;
    getline(in_file, _); //new line

    int total_area = 0;
    for(int i=0;i<num_cell;i++){
        int _area;
        in_file >> _area;
        in_file >> _area;
        cell_size.push_back(_area);
        getline(in_file, _);
        vector<int> v;
        cell.push_back(v);
        total_area+=_area;
    }
    is2way = (abs(max_area - total_area*0.55) < 1);
    //2-way partition
    if(is2way){
        int group[num_cell];
        int gain[num_cell];
        bool lock[num_cell];
        int cost = 0;
        //partition
        int area[2] = {total_area, 0};
        for(int i=0;i<num_cell;i++){
            group[i]=0;
            lock[i] = false;
        }

        getline(in_file, _); //.net
        in_file>>num_net; 
        getline(in_file, _);
        for(int i=0;i<num_net;i++){
            set<int> _net[2];
            vector<set<int>> v;
            int n;
            in_file>>n;
            getline(in_file, _);
            for(int j=0;j<n;j++){
                int _cell;
                in_file>>_cell;
                cell[_cell].push_back(i);
                _net[group[_cell]].insert(_cell);
            }
            getline(in_file, _);
            v.push_back(_net[0]);
            v.push_back(_net[1]);
            net.push_back(v);
        }

        //compute cost
        cost = 0;
        for(int i=0;i<num_net;i++){
            if(net[i][0].size() && net[i][1].size()){
                cost+=1;
            }
        }
        cout<<"BEFORE MOVE:"<<cost<<endl;

        //init
        vector<int> move;
        vector<vector<set<int>>> tmp_net = net; 
        int tmp_area[2];
        int tmp_group[num_cell];
        for(int i=0;i<num_cell;i++){
            tmp_group[i] = group[i];
        }
        tmp_area[0] = area[0];
        tmp_area[1] = area[1];
        //initialization
        for(int i=0;i<num_cell;i++){
            int score = 0;
            for(auto _net:cell[i]){
                if(tmp_net[_net][tmp_group[i]].size() == 1)
                    score+=1;
                if(tmp_net[_net][1-tmp_group[i]].size() == 0)
                    score-=1;
            }
            gain[i] = score;
        }

        ////update_Gain
        for(int t=0;t<num_cell;t++){
            int max = -1000000;
            int idx = -1;
            int max_cell_area = -1;
            //find max gain
            for(int i=0;i<num_cell;i++){
                if(!lock[i] && gain[i]>=max && tmp_area[0]>=max_area){
                    if(gain[i]==max){
                        if(cell_size[i]>max_cell_area){
                            max_cell_area = cell_size[i];
                            max=gain[i];
                            idx = i;
                        }
                    }
                    else{
                        max_cell_area = cell_size[i];
                        max=gain[i];
                        idx = i;
                    }
                }
            }
            if(idx==-1)
                break;
            for(auto i:cell[idx]){
                if(tmp_net[i][1-tmp_group[idx]].size() == 0){
                    for(auto c:tmp_net[i][tmp_group[idx]])
                        gain[c]++;
                }
                else if(tmp_net[i][1-tmp_group[idx]].size() == 1){
                    for(auto c:tmp_net[i][1-tmp_group[idx]])
                        gain[c]--;
                }
                tmp_net[i][1-tmp_group[idx]].insert(idx);
                tmp_net[i][tmp_group[idx]].erase(idx);
                if(tmp_net[i][tmp_group[idx]].size() == 0){
                    for(auto c:tmp_net[i][1-tmp_group[idx]])
                        gain[c]--;
                }
                else if(tmp_net[i][tmp_group[idx]].size() == 1){
                    for(auto c:tmp_net[i][tmp_group[idx]])
                        gain[c]++;
                }
            }
            tmp_group[idx] = 1 - tmp_group[idx];
            tmp_area[tmp_group[idx]] += cell_size[idx];
            tmp_area[1-tmp_group[idx]] -= cell_size[idx];
            lock[idx] = true;
            move.push_back(idx);
        }
        int max_idx = move.size();

        for(int t=0;t<max_idx;t++){
            int idx = move[t];
            for(auto i:cell[idx]){
                net[i][1-group[idx]].insert(idx);
                net[i][group[idx]].erase(idx);
            }
            group[idx] = 1 - group[idx];
            area[group[idx]] += cell_size[idx];
            area[1-group[idx]] -= cell_size[idx];
        }
        cost = 0;
        for(int i=0;i<num_net;i++){
            if(net[i][0].size() && net[i][1].size()){
                cost+=1;
            }
        }
        cout<<"INIT cost:"<<cost<<endl;
        //cout<<area[0]<<endl<<area[1]<<endl;

        while(1){
            vector<int> G;
            vector<int> move;
            vector<vector<set<int>>> tmp_net = net; 
            int tmp_area[2];
            int tmp_group[num_cell];
            for(int i=0;i<num_cell;i++){
                tmp_group[i] = group[i];
                lock[i] = false;
            }
            tmp_area[0] = area[0];
            tmp_area[1] = area[1];
            //initialization
            for(int i=0;i<num_cell;i++){
                int score = 0;
                for(auto _net:cell[i]){
                    if(tmp_net[_net][tmp_group[i]].size() == 1)
                        score+=1;
                    if(tmp_net[_net][1-tmp_group[i]].size() == 0)
                        score-=1;
                }
                gain[i] = score;
            }

            ////update_Gain
            for(int t=0;t<num_cell;t++){
                int max = -1000000;
                int idx = -1;
                //find max gain
                int max_sum_cell = -1;
                for(int i=0;i<num_cell;i++){
                    if(!lock[i] && gain[i]>=max && cell_size[i]+tmp_area[1-tmp_group[i]]<max_area){
                        int sum_cell = 0;
                        for(auto n:cell[i]){
                            sum_cell += tmp_net[n][1-tmp_group[i]].size();
                        }
                        if((gain[i]==max && sum_cell>max_sum_cell)|| gain[i]>max){
                            max_sum_cell = sum_cell;
                            max=gain[i];
                            idx = i;
                        }
                    }
                }
                if(idx==-1)
                    break;
                for(auto i:cell[idx]){
                    if(tmp_net[i][1-tmp_group[idx]].size() == 0){
                        for(auto c:tmp_net[i][tmp_group[idx]])
                            gain[c]++;
                    }
                    else if(tmp_net[i][1-tmp_group[idx]].size() == 1){
                        for(auto c:tmp_net[i][1-tmp_group[idx]])
                            gain[c]--;
                    }
                    tmp_net[i][1-tmp_group[idx]].insert(idx);
                    tmp_net[i][tmp_group[idx]].erase(idx);
                    if(tmp_net[i][tmp_group[idx]].size() == 0){
                        for(auto c:tmp_net[i][1-tmp_group[idx]])
                            gain[c]--;
                    }
                    else if(tmp_net[i][tmp_group[idx]].size() == 1){
                        for(auto c:tmp_net[i][tmp_group[idx]])
                            gain[c]++;
                    }
                }
                tmp_group[idx] = 1 - tmp_group[idx];
                tmp_area[tmp_group[idx]] += cell_size[idx];
                tmp_area[1-tmp_group[idx]] -= cell_size[idx];
                lock[idx] = true;
                G.push_back(max);
                move.push_back(idx);
            }

            int max = 0;
            int reduce = 0;
            int max_idx = 0;
            for(int i=0;i<G.size();i++){
                reduce += G[i];
                if(reduce>max){
                    max = reduce;
                    max_idx = i;
                }
            }
            if(max==0)
                break;
            //cout<<"reduce:"<<max<<endl;
            for(int t=0;t<=max_idx;t++){
                int idx = move[t];
                for(auto i:cell[idx]){
                    net[i][1-group[idx]].insert(idx);
                    net[i][group[idx]].erase(idx);
                }
                group[idx] = 1 - group[idx];
                area[group[idx]] += cell_size[idx];
                area[1-group[idx]] -= cell_size[idx];
            }
            for(int i=0;i<num_cell;i++)
                lock[i] = false;
            //compute cost
            cost = 0;
            for(int i=0;i<num_net;i++){
                if(net[i][0].size() && net[i][1].size()){
                    cost+=1;
                }
            }
            cout<<"AFTER MOVE:"<<cost<<endl;
            //cout<<area[0]<<endl<<area[1]<<endl;
        }


        //ofstream out_file;
        //out_file.open(argv[2]);    //open <.out file>
        //out_file << cost << endl;
        //out_file << 2 <<endl;
        //for(auto i:group){
            //out_file << i << endl;
        //}
        //out_file.close();
    }
    

    return 0;
}