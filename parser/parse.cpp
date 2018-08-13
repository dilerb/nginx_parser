#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <math.h>
#include <cstring>
#include <list>
#include <string>
#include <map>
#include <vector>
using namespace std;

class user
{
public:
	string ip; // user ip
	string eventt; // event time
	string reqm; // request method
	string address; // içerik adresi
	string id; // içerik id
	string br; // içerik bitrate i
	string rt,ut;
	long long br_tip_long; // bitrate tipi
	bool type;
	int status;
	string browser; // user agent
	long long data; // how many bytes
	string cache;
	user() {
		ip = "-";
		eventt = "-";
		reqm = "-";
		address = "-";
		id = "-";
		br = "-";
		browser = "-";
		status = 0;
		type = false;
		br_tip_long = 0;
		data = 0;
		cache = "-";
		rt = "-";
		ut = "-";
	}
};

void parse(string, string[]);
user meaning(vector<string>);
void mapping(user);
void stat();
list<user>A;
map<string, int> wordCount; //ip
map<string, int> wordCount2; // içerik
map<long long, int> wordCount3; // bitrate tip
map<string, int> wordCount4; // browser
map<int, int> wordCount5; // status
long long total = 0;

int main()
{
	setlocale(LC_ALL, "Turkish"); // türkçe karakter
	string rules[3] = { "[]","\"\""," " }; // öncelik önemli
	string buffer;
	ifstream input;
	input.open("nginx.access.log");
	while (getline(input, buffer)) // while (getline(input,buffer)) // for(int i=0;i<25;i++)
	{
		if ((buffer == "") || (buffer == " ")) break;
		parse(buffer, rules);
	}
	stat();
	input.close();
	getchar();
	return 0;
}

void parse(string buffer, string rules[]) {
	string subs;
	vector<string>parts;
	int loc = 0, loc2 = 0;
	for (size_t i = 0; i < 3; i++) {
		while (buffer.find(rules[i][0]) != -1) {
			if (rules[i].size() == 1) {
				subs = buffer.substr(0, buffer.find(rules[i]));
				parts.push_back(subs);
				buffer = buffer.erase(buffer.find(subs), subs.size() + 1);
			}
			else
			{
				if (buffer[buffer.find(rules[i][0]) - 1] != ' ') {
					string subs3 = buffer;
					loc = subs3.find(rules[i][0]);
					int k = loc;
					subs.clear();
					while (1) {
						k--;
						if (subs3[k] == ' ') break;
						else subs = subs3[k] + subs;
					}
					subs3.erase(loc, 1);
					loc2 = subs3.find(rules[i][1]);
					string subs2 = subs3.substr(loc, loc2 - loc);
					subs = subs + rules[i][0] + subs2 + rules[i][1];
					parts.push_back(subs);
					buffer.erase(buffer.find(subs), subs.size() + 1);
				}
				else {
					loc = buffer.find(rules[i][0]);
					buffer = buffer.erase(loc, 1);
					loc2 = buffer.find(rules[i][1]);
					subs = buffer.substr(loc, loc2 - loc);
					parts.push_back(subs);
					buffer = buffer.erase(loc, subs.size() + 2);
				}
			}
		}
	}
	if (buffer != "") {
		parts.push_back(buffer);
		buffer.clear();
	}
	A.push_back(meaning(parts));
}

user meaning(vector<string>parts) {
	//counter++;
	//cout << counter << endl;
	/*if (counter == 1732) {

		cout << "";
	}*/
	user new_user;
	vector<string>::iterator it;
	vector<string>temp;
	string subs, subs2, subs3;
	int i = 0;
	int j = parts.size() - 1;
	for (it = parts.begin(); it != parts.end(); it++)
	{
		if ((it[0] != "-") && (it[0] != "")) temp.push_back(it[0]);
	}

	if (temp.back().find("cs") != -1) {
		new_user.cache = temp.back();
		temp.pop_back();
	}
	//cout << "Cache status: " << subs;
	//if (subs == "HIT") cout << " " << "(Cache Kullanýlmýþ)" << endl;
	//else cout << " " << "(Cache Kullanýlmamýþ)" << endl;	
	if (temp.back().find("rt") != -1) {
		new_user.rt = temp.back();
		temp.pop_back();
	}

	new_user.data = stoll(temp.back());
	total += new_user.data;
	temp.pop_back();

	new_user.status = stoi(temp.back());
	temp.pop_back();

	new_user.ip = temp.back();
	temp.pop_back();

	if (temp.back().find("ut") != -1) {
		new_user.ut = temp.back();
		temp.pop_back();
	}

	new_user.eventt = temp.front();
	temp.erase(temp.begin());

	new_user.address = temp[0];
	new_user.id = temp[0].substr(0, temp[0].find('.'));
	new_user.id = new_user.id.substr(new_user.id.find_last_of('/') + 1);

	int loc = temp[0].find("QualityLevels");
	if (loc != -1) {
		new_user.reqm = temp[0].substr(0, temp[0].find(" "));
		temp[0] = temp[0].substr(loc);
		subs = temp[0].substr(0, temp[0].find('/'));
		loc = subs.find('(');
		long long bitrate = stoll(subs.substr(loc + 1, subs.find(')') - loc - 1));
		bitrate = (bitrate / 100000) * 100000;
		subs2 = "QualityLevels(";
		subs2.append(to_string(bitrate));
		subs2 += ')';
		new_user.br = subs2;
		loc = temp[0].find('/');
		subs = temp[0].substr(loc + 1, temp[0].find_last_of(')') - loc);
		new_user.br_tip_long = bitrate; // içerik tipi
		if (subs.find("video") == -1) new_user.type = 1; // 1 audio 0 video
		else new_user.type = 0;
	}
	temp.erase(temp.begin());
	if (temp.size() != 0) {
		new_user.browser = temp.back();
	}
	mapping(new_user);
	return new_user;
}

void mapping(user new_user) {
	++wordCount[new_user.ip];
	++wordCount2[new_user.id];
	++wordCount3[new_user.br_tip_long];
	++wordCount4[new_user.browser];
	++wordCount5[new_user.status];
}

void stat() {
	map<string, int>::iterator it;
	map<string, int>::iterator it2;
	map<long long, int>::iterator it3;
	map<string, int>::iterator it4;
	map<int, int>::iterator it5;
	multimap <int, string > freq_rev; // ordered small to large
	multimap <int, string > freq_rev2; // ordered small to large
	int counter = A.size();
	for (it = wordCount.begin(); it != wordCount.end(); it++)
	{
		freq_rev.insert(make_pair(it->second, it->first));
	}
	for (it2 = wordCount2.begin(); it2 != wordCount2.end(); it2++)
	{
		freq_rev2.insert(make_pair(it2->second, it2->first));
	}
	cout << "Kaç adet unique ip den izleme yapýlmýþ? " << wordCount.size() << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "Kaç unique içerik izlenmiþ? " << wordCount2.size() << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "Toplam ne kadar veri gönderilmiþ ? ";
	if (total < 1000) cout << total << " bytes" << endl;
	else if (total < 1000000) cout << total / 1000 << " KB" << endl;
	else if (total < 1000000000) cout << total / 1000000 << " MB" << endl;
	else if (total < 1000000000000) cout << total / 1000000000 << " GB" << endl;
	else if (total < 1000000000000000) cout << total / 1000000000000 << " TB" << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "En çok izleme yapan kullanýcý? " << freq_rev.rbegin()->second << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "En çok izlenen içerik ? " << freq_rev2.rbegin()->second << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "Ýzlenen bitratelerin oraný (video). ";

	for (it3 = wordCount3.begin(); it3 != wordCount3.end(); ++it3)
	{
		cout << it3->first << "= %" << (it3->second * 100) / counter << endl;
	}
	cout << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "Ýzleme yapýlan browserlarýn oraný. ";
	for (it4 = wordCount4.begin(); it4 != wordCount4.end(); ++it4)
	{
		cout << it4->first << "= %" << (it4->second * 100) / counter << endl;
	}
	cout << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "HTTP status oraný ";
	for (it5 = wordCount5.begin(); it5 != wordCount5.end(); ++it5)
	{
		cout << it5->first << "= %" << (it5->second * 100) / counter << endl;
	}
	cout << endl;
}