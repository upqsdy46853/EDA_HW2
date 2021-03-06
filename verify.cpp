#include<iostream>
#include<fstream>
#include<vector>
using namespace std;
int main(int argc, char* argv[]){
	int cost;
	int golden = 0;
	int k;
	vector<int> group;
	ifstream in_file;
	in_file.open(argv[1]);
	in_file >> cost;
	in_file >> k;
	while(!in_file.eof()){
		int group_id;
		in_file >> group_id;
		group.push_back(group_id);
	}
	in_file.close();

	in_file.open(argv[2]);
	int area_cons;
	int num_cell;
	int num_net;
	vector<int> area;
	string _;
	in_file >> area_cons;
	in_file >> _;	
	in_file >> num_cell;
	for(int i=0;i<num_cell;i++){
		int cell_size;
		in_file >> _;
		in_file >> cell_size;
		area.push_back(cell_size);
	}
	in_file >> _;	
	in_file >> num_net;
	
	for(int i=0;i<num_net;i++){
		int n;
		in_file >> n;
		bool in[k];
		int span = 0;
		for(int i=0;i<k;i++){
			in[i] = false;
		}
		for(int j=0;j<n;j++){
			int cell_idx;
			in_file >> cell_idx;
			if(!in[group[cell_idx]]){
				in[group[cell_idx]] = true;
				span++;
			}
		}
		golden += (span-1)*(span-1);		
	}
	int total_area[k];
	for(int i=0;i<k;i++){
		total_area[i] = 0;
	}
	for(int i=0;i<area.size();i++){
		total_area[group[i]] += area[i];
		if(total_area[group[i]]>area_cons){
			cout<<"FAIL"<<endl;
			return 0;
		}
	}
	if (cost == golden){
		cout<<"PASS"<<endl;
	}
	else{
		cout<<"FAIL"<<endl;
	}
}