# 实验六 图算法的应用

## 问题描述

设想有一个含有m个用户的社交网络，可以认为这是一个含有m个节点的无向图，每一个节点代表一个用户，如果两个用户认识，则他们之间有边相连，我们希望设计一个
推荐系统，促进这个社交网络的运转，这个推荐系统能完成以下两个功能：

（1）系统能够推荐社交网络中可能认识的两个用户相互认识，这里可能认识的两个用户的判断条件为：这两个用户有至少认识3个共同的人，并且这两个用户原来并不认识。

（2）随机挑选社交网络中的两个用户，判断这两个用户之间是否存在联系，以及这个联系是如何建立的，这里两个用户存在联系的判断条件为：两个用户之间可以通过他们已经存在的认识的人建立连接，即两个用户之间存在一个连通的路径。

## 输入格式

输入的第一行是一个整数L（2<=L<=1000），代表社交网络中的节点（用户）个数，接下来的L行代表一个邻接矩阵A，A中元素的值仅为0或者1，A中每一行有L个元素，
代表一个节点和其他点的连接关系，如果两个节点相连，即对应的两个用户认识，两个则对应元素取值为1，否则取值为0，如A[0][1] = 1说明第0个节点和第1个节点相连，节点自身不与自身连接，最后输入两个数i，j，代表序号为i和j的节点，需要判断这两个节点是否是连通的，参见题目的输入样例。

## 输出格式

首先输出一个矩阵R，R的行数和列数均为L（与A矩阵相同），R中的元素取值仅为0或1，每一行代表一个人与其他人的推荐关系，如果两个人需要被推荐认识则相应的元素
位置为1，否则相应的元素位置为0，如A[0][1] = 1说明推荐系统认为需要推荐第0个人和第1个人互相认识，之后程序输出给定两个节点i，j之间的连通路径（任给一条
即可，路径上包含i，j节点），如果这两个节点不连通，则直接输出-1，参见题目的输出样例。

## 输入样例

5         解释：表示社交网络中一共有5个人

0 1 1 1 0 解释：表示第0个人和第1,2,3个人认识，和第4个人不认识

1 0 0 0 1 解释：表示第1个人和第0,4个人认识，和第2,3个人不认识

1 0 0 0 1

1 0 0 0 1

0 1 1 1 0

0 4       解释：需要判断第0个节点和第4个节点是否连通

## 输出样例

0 0 0 0 1 解释：表示推荐系统推荐第0个人和第4个人相互认识

0 0 0 0 0 解释：表示推荐系统不推荐第1个人和任何人认识

0 0 0 0 0

0 0 0 0 0

1 0 0 0 0

0 1 4     解释：表示第0个人与第4个人的连接方式为通过第0,1,4个人

## 提示

由输入输出的定义可以知道A和R都是对称矩阵，并且A和R主对角线上的元素都是0.
