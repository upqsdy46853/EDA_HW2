#include<iostream>
#include<fstream>
#include<vector>
#include<set>

using namespace std;
#define MINUS_INF -100000000
int main(int argc, char* argv[]){
    bool is2way;
    int max_area;
    int num_cell;
    int num_net;
    int cost;
    string _;
    ifstream in_file;
    vector<int> cell_size;
    vector<vector<int>> cell;

    in_file.open(argv[1]);

    in_file >> max_area;
    in_file >> _;
    in_file >> num_cell;

    int gain[num_cell];
    int group[num_cell];
    bool lock[num_cell];
    for(int i=0;i<num_cell;i++){
        group[i]=0;
        lock[i] = false;
    }
    int total_area = 0;
    for(int i=0;i<num_cell;i++){
        int _area;
        in_file >> _;
        in_file >> _area;
        cell_size.push_back(_area);
        vector<int> v;
        cell.push_back(v);
        total_area+=_area;
    }
    is2way = (abs(max_area - total_area*0.55) < 1);

    //2-way partition
    if(is2way){
        //partition
        vector<vector<set<int>>> net;
        int area[2] = {total_area, 0};
        in_file>>_;
        in_file>>num_net; 
        for(int i=0;i<num_net;i++){
            set<int> _net[2];
            vector<set<int>> v;
            int n;
            in_file>>n;
            for(int j=0;j<n;j++){
                int _cell;
                in_file>>_cell;
                cell[_cell].push_back(i);
                _net[0].insert(_cell);
            }
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
        //cout<<"BEFORE MOVE:"<<cost<<endl;

        //partition to two parts
        vector<int> move;
        vector<vector<set<int>>> tmp_net = net; 
        int tmp_area[2]={area[0], area[1]};
        int tmp_group[num_cell];

        for(int i=0;i<num_cell;i++){
            tmp_group[i] = group[i];
        }

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
        while(tmp_area[0]>=max_area){
            int max = MINUS_INF;
            int idx = -1;
            int max_cell_area = -1;
            //find max gain
            for(int i=0;i<num_cell;i++){
                if(!lock[i] && gain[i]>=max){
                    if((gain[i]==max && cell_size[i]>max_cell_area) || gain[i]>max){
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

        for(int t=0;t<move.size();t++){
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
        //cout<<"INIT cost:"<<cost<<endl;

        //FM algo
        while(1){
            vector<int> G;
            move.clear();
            tmp_net = net; 
            tmp_area[0] = area[0];
            tmp_area[1] = area[1];
            for(int i=0;i<num_cell;i++){
                tmp_group[i] = group[i];
                lock[i] = false;
            }
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
                int max = MINUS_INF;
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
            //cout<<"AFTER MOVE:"<<cost<<endl;
        }


        ofstream out_file;
        out_file.open(argv[2]);
        out_file << cost << endl;
        out_file << 2 <<endl;
        for(auto i:group){
            out_file << i << endl;
        }
        out_file.close();
    }
    else{
        int group_id = 0;
        int sum = 0;
        int group[num_cell];
        int cost = 0;
        for(int i=0;i<num_cell;i++){
            if(sum + cell_size[i] >= max_area){
                group_id += 1;
                sum = 0;
            }
            sum += cell_size[i];
            group[i] = group_id;
        }

        in_file>>_;
        in_file>>num_net; 
        for(int i=0;i<num_net;i++){
            int n;
            int span[group_id+1];
            for(int i=0;i<group_id+1;i++)
                span[i] = 0;
            int cnt = 0;
            in_file>>n;
            for(int j=0;j<n;j++){
                int _cell;
                in_file>>_cell;
                if(!span[group[_cell]]){
                    cnt+=1;
                    span[group[_cell]] = 1;
                }
            }
            cost+=(cnt-1)*(cnt-1);
        }

        ofstream out_file;
        out_file.open(argv[2]);    //open <.out file>
        out_file << cost << endl;
        out_file << group_id+1 <<endl;
        for(auto i:group){
            out_file << i << endl;
        }
        out_file.close();
    }
    

    return 0;
}
