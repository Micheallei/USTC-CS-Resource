#include<iostream>
#include<vector>
using namespace std;

//����Ҫ��ѡ��Ҫ��Ĵ���ʵ��

int T, n;
int a[100];							//n����Ʒ�����
int count1 = 0;						//��¼������
int V = 0;							//����װ������£���ռ�õ����ռ�
vector<vector<int>> result;			//װ���п��з���
vector<int> temp;					//�ڵݹ��������ʱװһ�鷽��


void search(int pos, int rest) {	//posΪ��δ���Է��õĵ�һ����Ʒλ��(�������Ⱥ�˳��),restΪ����ʣ���ݻ�
	if (rest < 0) return;			//�������ʣ���ݻ�Ϊ��������

	if (rest == 0) {				//��������ݻ�Ϊ0��˵����ʱtemp��װ������Ϊһ���
		result.push_back(temp);
		count1++;
		return;
	}
	for (int j = pos; j < n; j++) { //˼·����δ���Է�����Ʒ���η���temp����
		temp.push_back(a[j]);
		if (((T - V) > (rest - a[j]))&&(rest-a[j])>=0) V = 10 - rest + a[j];
		search(j + 1, rest - a[j]);
		temp.pop_back();			//����
	}
	return;
}


int main()
{

	cin >> T >> n;   //�������T,��Ʒ��n
	for (int i = 0; i < n; i++) {
		cin >> a[i];  //n����Ʒ�����
	}
	search(0, T);
	if (count1 == 0) {
		search(0, V);
	}
	cout << count1 << endl;
	for (auto it = result.begin(); it != result.end(); it++) {  //���
		for (auto iter = it->begin(); iter != it->end(); iter++) {
			cout << *iter << " ";
		}
		cout << endl;
	}
	return 0;
}
