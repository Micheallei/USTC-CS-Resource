#include<iostream>
#include<vector>
using namespace std;

//基本要求及选做要求的代码实现

int T, n;
int a[100];							//n件物品的体积
int count1 = 0;						//记录方案数
int V = 0;							//不能装满情况下，能占用的最大空间
vector<vector<int>> result;			//装所有可行方案
vector<int> temp;					//在递归过程中临时装一组方案


void search(int pos, int rest) {	//pos为还未尝试放置的第一个物品位置(按输入先后顺序),rest为背包剩余容积
	if (rest < 0) return;			//如果背包剩余容积为负，返回

	if (rest == 0) {				//如果背包容积为0，说明此时temp里装的数据为一组答案
		result.push_back(temp);
		count1++;
		return;
	}
	for (int j = pos; j < n; j++) { //思路：将未尝试放置物品依次放入temp尝试
		temp.push_back(a[j]);
		if (((T - V) > (rest - a[j]))&&(rest-a[j])>=0) V = 10 - rest + a[j];
		search(j + 1, rest - a[j]);
		temp.pop_back();			//回溯
	}
	return;
}


int main()
{

	cin >> T >> n;   //背包体积T,物品数n
	for (int i = 0; i < n; i++) {
		cin >> a[i];  //n件物品的体积
	}
	search(0, T);
	if (count1 == 0) {
		search(0, V);
	}
	cout << count1 << endl;
	for (auto it = result.begin(); it != result.end(); it++) {  //输出
		for (auto iter = it->begin(); iter != it->end(); iter++) {
			cout << *iter << " ";
		}
		cout << endl;
	}
	return 0;
}
