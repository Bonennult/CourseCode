# -*-coding:utf-8 -*-
####################################################
# DVrouter.py
# Names:
# NetIds:
#####################################################

import sys
from collections import defaultdict
from router import Router
from packet import Packet
from json import dumps, loads

COST_MAX = 16
class DVrouter(Router):
    """Distance vector routing protocol implementation."""

    def __init__(self, addr, heartbeatTime):
        """class fields and initialization code here"""
        Router.__init__(self, addr)  # initialize superclass - don't remove
        self.routersNext = {}  # key》当前节点可到达的所有节点；value》以key作为目标根节点时最短路径中上一个节点
        self.routersCost = {}  # key》当前节点可到达的所有节点；value》以key作为目标根节点时最短路径的总权重
        self.routersPort = {}  # key》当前节点的所有邻居节点的名称（字母）；value》key所表示的节点的编号（整数），编号值是根据节点名称ascii码值排序依次设为1,2,...
        self.routersAddr = {}  # 把 self.routersPort 的key和value交换一下
        self.heartbeat = heartbeatTime
        self.lasttime = None

    def handlePacket(self, port, packet):
        """process incoming packet"""
        # 此函数的目的是当从某个端口接收到新的包时进行分析，以决定是否需要发送路由包来更新路由信息
        # port是接收包的端口号（整数）
        # deal with traceroute packet
        if packet.isTraceroute():
            if packet.dstAddr in self.routersNext:  # 判断当前节点是否可达包中指定的目标节点
                next_nb = self.routersNext[packet.dstAddr]
                self.send(self.routersPort[next_nb], packet)  # 发往到达目标节点的最短路径上下一节点

        # deal with routing packet
        if packet.isRouting():
            # 首先判断是否需要更新路由信息
            rtn = self.updateNode(packet.content)
            # if rtn=None, irrelevant Routing Information
            # 需要更新路由信息
            if rtn != None:
                # routing table updated, need to be broadcast
                # 遍历当前 router 的所有邻居节点进行更新
                for port in self.routersAddr:  # this port is diff from input port
                    # 生成需要更新的路由信息，以路由包的形式发送给邻居节点
                    # 下面这几句可以放在循环外边
                    content = {}
                    content["src"] = self.addr
                    content["dst"] = rtn[1]  # packet dst，名称（字母）
                    content["cost"] = rtn[2] # cost
                    # 排除当前router到当前dst（目标根节点）的路径中的上一节点，不发送路由包
                    if self.routersAddr[port] != self.routersNext[rtn[1]]:   # split horizon 
                        content_str = dumps(content)
                        # send the update route tb to neighbour node (ending point of port)
                        # 包中的内容对于src和dst都用名称表示
                        packet = Packet(Packet.ROUTING, self.addr, self.routersAddr[port], content_str)
                        # send函数的参数对于目标节点用整数编号表示
                        self.send(port, packet)

    def updateNode(self, content):
        """update node with routing packet"""
        data = loads(content)  # 解码json编码（dumps）的内容
        src = data["src"]
        dst = data["dst"]
        cost = data["cost"]
        # build the src-dst link
        # 如果当前节点还不能到达包中指定的dst，建立新的连接
        if dst not in self.routersCost and dst != self.addr:
            # 实际上由于这个包就是从src传过来的因此下面的if语句一定成立，只是为了保险起见？
            if src in self.routersCost:
                self.routersCost[dst] = self.routersCost[src] + cost
                self.routersNext[dst] = src
                return True, dst, self.routersCost[dst]

        # choose the less cost or new info
        if dst in self.routersCost:
            if src in self.routersCost:
                # 第二个判断条件：当前路由节点到达目的根节点的最短路径时需要经过src节点，则一定更新
                # 1.有可能是src到dst的距离权重减小了，则当前节点也需要跟着减小
                # 2.有可能是有链路被断开了导致src到dst的距离权重变为COST_MAX，则当前节点到dst的路由
                #   需要清除，在之后的迭代过程中重新生成
                if (self.routersCost[dst] > self.routersCost[src] + cost) or (self.routersNext[dst] == src and src != dst):
                    self.routersCost[dst] = self.routersCost[src] + cost
                    self.routersNext[dst] = src
                    # set COST_MAX as infinity
                    if self.routersCost[dst] >  COST_MAX:
                        self.routersCost[dst] =  COST_MAX
                    return True, dst, self.routersCost[dst]

        return None


    def handleNewLink(self, port, endpoint, cost):
        """handle new link"""
        # 此函数的目的是在新的链路建立时在当前路由节点进行一定处理，以更新路由信息
        # 需要更新的内容：
        # 1.当前节点的routersPort，routersAddr：增加端口
        # 2.当前节点的routersCost[endpoint]，routersNext[endpoint]：不必是最短路径的信息，之后可以更新
        # 3.

        # 这个函数是在Router基类中被定义，并被函数addLink调用，但是addLink中只是更新了当前路由节点的
        # links（与邻居节点的链路）信息，却没有更新当前子类DVrouter节点的信息如self.routersPort、self.routersAddr等
        # port：新邻居节点的端口号（整数），这个port可能原来已经存在，此时就直接将原来port对应的link替换为当前的新link
        # 下面循环中的内容不是必要的，但是可以使网络的路由信息尽快更新
        for router in self.routersPort:
            # endpoint maybe connect to router via other nodes, now the new link connects them directly.
            if (endpoint not in self.routersCost) or (self.routersCost[endpoint] > cost):
                # 将当前节点与endpoint节点之间的信息发送给当前节点之前所有的邻居节点
                content = {}
                content["src"] = self.addr
                content["dst"] = endpoint
                content["cost"] = cost
                content_str = dumps(content)
                packet = Packet(Packet.ROUTING, self.addr, router, content_str)
                # send the new router info to other routers
                self.send(self.routersPort[router], packet)

                # 将当前节点与之前邻居节点之间的距离发送给新的邻居节点endpoint
                content1 = {}
                content1["src"] = self.addr
                content1["dst"] = router
                content1["cost"] = self.routersCost[router]
                content1_str = dumps(content1)
                packet1 = Packet(Packet.ROUTING, self.addr,
                                 endpoint, content1_str)
                # send other router infos to the new router
                self.send(port, packet1)
        # update the routing table at self
        self.routersPort[endpoint] = port
        self.routersNext[endpoint] = endpoint  # 不一定？
        self.routersAddr[port] = endpoint
        if (endpoint not in self.routersCost) or (self.routersCost[endpoint] > cost):
            self.routersCost[endpoint] = cost


    def handleRemoveLink(self, port):
        """handle removed link"""
        # 删除当前路由器的端口port
        addr = self.routersAddr[port]
        self.routersCost[addr] =  COST_MAX
        # 还需要删除所有间接通过该port对应的链路的最短路径，置为COST_MAX
        for addr1 in self.routersNext:
            if self.routersNext[addr1] == addr:
                self.routersCost[addr1] =  COST_MAX
        if addr == self.addr:
            self.routersCost[addr] = 0
            self.routersNext[addr] = addr


    def handleTime(self, timeMillisecs):
        """handle current time"""
        # 此函数的目的是定时更新路由信息：通过向邻居节点发送路由包实现
        if (self.lasttime == None) or (timeMillisecs - self.lasttime > self.heartbeat):
            self.lasttime = timeMillisecs

            # 遍历邻居节点，依次发送当前节点可到达的各个目标节点的cost信息
            for addr1 in self.routersPort:
                for dst1 in self.routersCost:
                    content1 = {}
                    content1["src"] = self.addr
                    content1["dst"] = dst1
                    content1["cost"] = self.routersCost[dst1]
                    content1_str = dumps(content1)
                    packet1 = Packet(Packet.ROUTING, self.addr,
                                     addr1, content1_str)
                    # send DV to neighbor nodes
                    self.send(self.routersPort[addr1], packet1)


    def debugString(self):
        """generate a string for debugging in network visualizer"""
        out = str(self.routersNext) + "\n" + str(self.routersCost)
        return out
