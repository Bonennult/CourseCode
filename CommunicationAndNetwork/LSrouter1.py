# -*-coding:utf-8 -*-
####################################################
# LSrouter.py
# Names:
# NetIds:
#####################################################

import sys
from collections import defaultdict
from router import Router
from packet import Packet
from json import dumps, loads
from LSP import LSP
from Queue import PriorityQueue

COST_MAX = 16
class LSrouter(Router):
    """Link state routing protocol implementation."""

    def __init__(self, addr, heartbeatTime):
        """class fields and initialization code here"""
        Router.__init__(self, addr)  # initialize superclass - don't remove
        self.routersLSP = {}  ### 
        self.routersAddr = {} ### key》端口号；value》邻居节点名称
        self.routersPort = {} ### key》邻居节点名称；value》端口号
        self.routersNext = {} ### 
        self.routersCost = {} ### 
        self.seqnum = 0 ###  
        self.routersLSP[self.addr] = LSP(self.addr, 0, {}) # 以self.addr初始化作为LSP的第一个元素
        # 每个路由节点针对以每一个节点作为源根节点生成的LSP单独存放一个LSP

        self.lasttime = None
        self.heartbeat = heartbeatTime

    def handlePacket(self, port, packet):
        """process incoming packet"""
        # deal with traceroute packet
        # 接收数据包，如有需要则进行转发（发送给到达目的节点的路径上下一节点）
        if packet.isTraceroute():
            if packet.dstAddr in self.routersNext:
                next_nb = self.routersNext[packet.dstAddr]
                self.send(self.routersPort[next_nb], packet)

        # deal with routing packet
        transfer = False  # 是否转发
        if packet.isRouting():
            # 这是什么情况？
            if packet.dstAddr == packet.srcAddr:
                return

            packetIn = loads(packet.content)  # 包的内容
            addr = packetIn["addr"]
            seqnum = packetIn["seqnum"]
            nbcost = packetIn["nbcost"]
            # 当前路由节点还未存放以新收到的包中addr作为源根节点的LSP
            if addr not in self.routersLSP:
                self.routersLSP[addr] = LSP(addr, seqnum, nbcost)
                transfer = True
            # 更新以addr作为源根节点的LSP的信息
            if self.routersLSP[addr].updateLSP(packetIn):
                transfer = True

            # 当前节点收到的包导致以addr为源根节点的LSP有更新，于是当前节点进行包的转发
            if transfer:
                # 对所有邻居节点转发收到的包，但是把src改为当前节点，dst改为邻居节点
                # 目的是只将更新信息传递给邻居节点而不发给其他节点，由邻居节点决定是否需要继续进行转发
                # 注意packetIn对应的信息（也即packet.content）并未修改
                for portNext in self.routersAddr:
                    if portNext != port:
                        packet.srcAddr = self.addr
                        packet.dstAddr = self.routersAddr[portNext]
                        self.send(portNext, packet)


    def handleNewLink(self, port, endpoint, cost):
        """handle new link"""
        # 当前路由器新开一个端口port，连接到endpoint
        # 此时不更新routersNext与routersCost
        self.routersAddr[port] = endpoint
        self.routersPort[endpoint] = port
        self.routersLSP[self.addr].nbcost[endpoint] = cost

        content = {}
        content["addr"] = self.addr
        content["seqnum"] = self.seqnum
        content["nbcost"] = self.routersLSP[self.addr].nbcost
        # 当前路由节点需要更新序列号的时刻：
        # 1.当前路由节点开设新的端口，以当前节点作为源根节点的LSP需要更新
        # 2.当前节点作为根节点的LSP增加新的节点
        self.seqnum += 1 # update the sequence number
        # 向所有端口发送新的路由包信息，包括新开设的端口
        for port in self.routersAddr:
            packet = Packet(Packet.ROUTING, self.addr, self.routersAddr[port], dumps(content))
            self.send(port, packet)

    
    def calPath(self):
        # Dijkstra Algorithm for LS routing
        self.setCostMax()
        # put LSP info into a queue for operations
        Q = PriorityQueue()
        for addr, nbcost in self.routersLSP[self.addr].nbcost.items():
            Q.put((nbcost, addr, addr))
        self.info = ""   # 调试语句
        while not Q.empty():
            Cost, Addr, Next = Q.get(False)
            """TODO: write your codes here to build the routing table"""
            if 'S' not in vars():
                S = {}
            if Addr not in self.routersCost or Cost < self.routersCost[Addr]:
                self.routersCost[Addr] = Cost
                self.routersNext[Addr] = Next
                S[Addr] = Cost

                if Addr in self.routersLSP:
                    for addr,nbcost in self.routersLSP[Addr].nbcost.items():
                        if addr not in S:
                            Q.put((self.routersCost[Addr]+nbcost, addr, self.routersNext[Addr]))



    def setCostMax(self):
        # intializtion for routing table 
        for addr in self.routersCost:
            self.routersCost[addr] = COST_MAX
        self.routersCost[self.addr] = 0
        self.routersNext[self.addr] = self.addr


    def handleRemoveLink(self, port):
        """handle removed link"""
        addr = self.routersAddr[port]
        self.routersLSP[self.addr].nbcost[addr] = COST_MAX
        self.calPath()

        content = {}
        content["addr"] = self.addr
        content["seqnum"] = self.seqnum + 1 
        content["nbcost"] = self.routersLSP[self.addr].nbcost
        self.seqnum += 1
        # 对所有的端口发送更新后的以当前节点作为源根节点的LSP信息，除了刚关闭的端口
        for port1 in self.routersAddr:
            if port1 != port:
                packet = Packet(Packet.ROUTING, self.addr, self.routersAddr[port1], dumps(content))
                self.send(port1, packet)
        pass

    def handleTime(self, timeMillisecs):
        """handle current time"""
        # 此函数的目的是定时更新路由信息：通过计算最短路径实现
        if (self.lasttime == None) or (timeMillisecs - self.lasttime > self.heartbeat):
            self.lasttime = timeMillisecs
            self.calPath()
      

    def debugString(self):
        """TODO: generate a string for debugging in network visualizer"""
        out = str(self.routersNext) + "\n" + str(self.routersCost) + "\n"
        for item in self.routersLSP.keys():
            out = out + str(self.routersLSP[item].addr) + " | " + str(self.routersLSP[item].seqnum) + " | " + str(self.routersLSP[item].nbcost) + "\n"
        out = out + self.info
        return out
