#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <windows.h>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}

using namespace std;

struct pointstruct
{
	int x;
	int y;
};
struct posstorage
{
	pointstruct beforeposition;
	pointstruct nowposition;
	bool startpoint;
	int g;
	int h;
	int f;
};

inline bool operator==(const pointstruct &A, const pointstruct &B)
{
	if (memcmp(&A, &B, sizeof(A)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

pointstruct startpos;
pointstruct endpos;
vector<posstorage> openlist;
vector<posstorage> closelist;
vector<pointstruct> closelistv2;
int mapinfo[10][10] = { 
{ 0,0,0,1,0,0,0,0,0,0 },
{ 0,0,0,1,0,1,0,0,0,0 },
{ 0,0,0,1,0,1,0,0,0,0 },
{ 0,0,0,1,0,1,1,1,1,1 },
{ 0,0,0,1,0,1,0,0,0,0 },
{ 0,0,0,1,0,1,0,0,1,0 },
{ 0,0,0,1,0,0,0,0,1,0 },
{ 0,0,0,0,0,1,0,1,1,0 },
{ 0,0,0,0,0,1,0,1,0,0 },
{ 0,0,0,1,0,1,0,1,0,0 } };

int calc_G(pointstruct target, posstorage lastpos)
{
	int res;
	res = abs(target.x - lastpos.nowposition.x) + abs(target.y - lastpos.nowposition.y) + lastpos.g;
	return res;
}

int calc_H(pointstruct tmp)
{
	int res;
	res = abs(tmp.x - endpos.x) + abs(tmp.y - endpos.y);
	return res;
}

bool checkblock(pointstruct tmp)
{
	if (tmp.x < 0 || tmp.y < 0)
	{
		return false;
	}
	if (tmp.x > 10 || tmp.y > 10)
	{
		return false;
	}
	if (mapinfo[tmp.y][tmp.x] == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool find_point(vector<pointstruct> pointlist, pointstruct target)
{
	bool flag = false;
	for (auto iter = pointlist.begin(); iter != pointlist.end(); iter++)
	{
		if (*iter == target)
		{
			flag = true;
		}
	}
	return flag;
}

int posinopenvector(vector<posstorage> openlist, pointstruct target)
{
	int i = 0;
	int flag = 0;
	bool check2=false;
	for (vector<posstorage>::iterator iter = openlist.begin(); iter != openlist.end(); iter++)
	{
		if (iter->nowposition == target)
		{
			flag = i;
			check2 = true;
		}
		i++;
	}
	if (check2!=false)
	{
		return flag;
	}
	else
	{
		return -1;
	}

}


void proc_findblock()
{
	vector<posstorage> tmp;
	pointstruct check;
	posstorage node;
	int minF = -1;
	int posminF = 0;
	int i = 0;
	for (vector<posstorage>::iterator iter = openlist.begin(); iter != openlist.end(); ++iter)
	{
		if (minF == -1)
		{
			minF = iter->f;
			posminF = i;
		}
		if (iter->f < minF)
		{
			minF = iter->f;
			posminF = i;
		}
		i++;
	}
	check.x = openlist[posminF].nowposition.x - 1;
	check.y = openlist[posminF].nowposition.y;

	if ((checkblock(check)) && !find_point(closelistv2, check))
	{
		node.beforeposition = openlist[posminF].nowposition;
		node.nowposition = check;
		node.g = calc_H(check);
		node.h = calc_G(check, openlist[posminF]);
		node.f = node.g + node.h;
		if (posinopenvector(openlist, check) == -1)
		{
			tmp.push_back(node);
		}
		else
		{
			int pos = posinopenvector(openlist, check);
			if (openlist[pos].g > node.g)
			{
				openlist[pos].beforeposition = node.beforeposition;
				openlist[pos].nowposition = node.nowposition;
				openlist[pos].g = node.g;
				openlist[pos].f = node.f;
				openlist[pos].h = node.h;
			}

		}
	}
	if (openlist[posminF].nowposition.x + 1 < 10)
	{
		check.x = openlist[posminF].nowposition.x + 1;
		check.y = openlist[posminF].nowposition.y;
		if ((checkblock(check)) && !find_point(closelistv2, check))
		{
			node.beforeposition = openlist[posminF].nowposition;
			node.nowposition = check;
			node.g = calc_H(check);
			node.h = calc_G(check, openlist[posminF]);
			node.f = node.g + node.h;
			if (posinopenvector(openlist, check) == -1)
			{
				tmp.push_back(node);
			}
			else
			{
				int pos = posinopenvector(openlist, check);
				if (openlist[pos].g > node.g)
				{
					openlist[pos].beforeposition = node.beforeposition;
					openlist[pos].nowposition = node.nowposition;
					openlist[pos].g = node.g;
					openlist[pos].f = node.f;
					openlist[pos].h = node.h;
				}

			}
		}
	}
	if (openlist[posminF].nowposition.y + 1 < 10)
	{
		check.x = openlist[posminF].nowposition.x;
		check.y = openlist[posminF].nowposition.y + 1;
		if ((checkblock(check)) && !find_point(closelistv2, check))
		{
			node.beforeposition = openlist[posminF].nowposition;
			node.nowposition = check;
			node.g = calc_H(check);
			node.h = calc_G(check, openlist[posminF]);
			node.f = node.g + node.h;
			if (posinopenvector(openlist, check) == -1)
			{
				tmp.push_back(node);
			}
			else
			{
				int pos = posinopenvector(openlist, check);
				if (openlist[pos].g > node.g)
				{
					openlist[pos].beforeposition = node.beforeposition;
					openlist[pos].nowposition = node.nowposition;
					openlist[pos].g = node.g;
					openlist[pos].f = node.f;
					openlist[pos].h = node.h;
				}

			}
		}
	}
	check.x = openlist[posminF].nowposition.x;
	check.y = openlist[posminF].nowposition.y - 1;
	if ((checkblock(check)) && !find_point(closelistv2, check))
	{
		node.beforeposition = openlist[posminF].nowposition;
		node.nowposition = check;
		node.g = calc_H(check);
		node.h = calc_G(check, openlist[posminF]);
		node.f = node.g + node.h;
		if (posinopenvector(openlist, check) == -1)
		{
			tmp.push_back(node);
		}
		else
		{
			int pos = posinopenvector(openlist, check);
			if (openlist[pos].g > node.g)
			{
				openlist[pos].beforeposition = node.beforeposition;
				openlist[pos].nowposition = node.nowposition;
				openlist[pos].g = node.g;
				openlist[pos].f = node.f;
				openlist[pos].h = node.h;
			}

		}
	}



	closelist.push_back(openlist[posminF]);
	closelistv2.push_back(openlist[posminF].nowposition);
	auto delpos = openlist.begin() + posminF;
	openlist.erase(delpos);
	for (auto iter = tmp.begin(); iter != tmp.end(); iter++)
	{
		openlist.push_back(*iter);
	}

}

pointstruct lasttarget;
pointstruct movetarget;
void findpath(pointstruct last)
{
	lasttarget.x = last.x;
	lasttarget.y = last.y;
	if (last==startpos)
	{
		movetarget = last;
		goto Lable1;
	}
	if (closelistv2.size() == 1)
	{
		movetarget = last;
		goto Lable1;
	}
	do
	{
		for (auto iter = closelist.begin(); iter != closelist.end(); iter++)
		{
			if (iter->nowposition == lasttarget)
			{
				//cout << "Point:(" << lasttarget.x << "," << lasttarget.y << ")" << endl;
				if (iter->beforeposition==startpos)
				{
					movetarget.x = iter->nowposition.x;
					movetarget.y = iter->nowposition.y;
					lasttarget = iter->beforeposition;
					break;
				}
				lasttarget = iter->beforeposition;
				break;
			}
		}
	} while (!(lasttarget == startpos)&&closelistv2.size()!=1);
Lable1:
	Sleep(1);
}


extern "C" pointstruct* __stdcall Findpath(int x1, int y1, int x2, int y2)
{
	startpos.x = x1;
	startpos.y = y1;
	endpos.x = x2;
	endpos.y = y2;
	posstorage stpos = { startpos,startpos,true,0,0,0 };
	openlist.push_back(stpos);
	if (startpos==endpos)
	{
		return &startpos;
	}
	do
	{
		proc_findblock();
	} while (posinopenvector(openlist, endpos) == -1||openlist.size()==0);
	//MessageBoxW(0, L"Processing Finish", L"Info", 0);
	if (openlist.size() == 0)
	{
		return 0;
	}
	findpath(openlist[posinopenvector(openlist, endpos)].beforeposition);
	openlist.clear();
	closelist.clear();
	closelistv2.clear();
	return &movetarget;
}

