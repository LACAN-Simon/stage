#   Copyright 2021 FOUBERT Brandon
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

class Node:
    """
    Represent a node in the log file
    
    Attributes
    ----------
    name : string
        the node's name
    list_rx : list
        list of received messages
    list_tx : list
        list of transmitted messages
    pdr : int
        the packet delivery ratio of this node
    latency : float
        the average transmission latency of this node
    success : int 
        total of the received msg that have been send by this node

    Notes
    -----
    Packet delivery ratio = ∑ Number of packet received / ∑ Number of packet send

    """

    def __init__(self, name):
        self.name = name
        self.list_rx = [] 
        self.list_tx = []
        self.pdr = 0
        self.latency = 0
        self.success = 0

    def __str__(self):
        return "Name : {0},Nb rx : {1},Nb tx : {2},Pdr : {3},Average transmission latency : {4},Send success : {5}".format(self.name, len(self.list_rx), len(self.list_tx), self.pdr, self.latency, self.latency, self.success)

    def getIdsRx(self):
        """Return the id of this node's transmitted messages
        
        Return
        ------
            list
                list of transmitted messages' ids
        """
        return set([rx.msg_id for rx in self.list_rx])

    def getIdsTx(self):
        """Return the id of this node's received messages
        
        Return
        ------
            list
                list of received messages' ids
        """

        return set([tx.msg_id for tx in self.list_tx])

class Message:
    """
    Represent a message in the log file
    
    Attributes
    ----------
    timestamp : int
        the timestamp of the message
    msg_id : int
        this message id
    node_name : string
        the node's name that have received or transmitted this message
    """

    def __init__(self, timestamp, msg_id, node_name):
        self.timestamp = timestamp
        self.msg_id = msg_id
        self.node_name = node_name

class MessageRx(Message):
    """
    Represent a received message in the log file
    
    Attributes
    ----------
    timestamp : int
        the timestamp of the message
    msg_id : int
        this message id
    node_name : string
        the node's name that have received or transmitted this message
    latency : int
        the latency of this received message (the time it has taken to arrive)
    """

    def __init__(self, timestamp, msg_id, node_name):
        Message.__init__(self, timestamp, msg_id, node_name)
        self.latency = 0

    def __str__(self):
        return "received;{0};{1};{2};{3}".format(self.node_name, self.timestamp, self.msg_id, self.latency)

class MessageTx(Message):
    """
    Represent a received message in the log file
    
    Attributes
    ----------
    timestamp : int
        the timestamp of the message
    msg_id : int
        this message id
    node_name : string
        the node's name that have received or transmitted this message
    success : int
        number of nodes that have received this message
    """

    def __init__(self, timestamp, msg_id, node_name):
        Message.__init__(self, timestamp, msg_id, node_name)
        self.success = 0

    def __str__(self):
        return "transmitted;{0};{1};{2};{3}".format(self.node_name, self.timestamp, self.msg_id, self.success)


def packetDeliveryRatio(nodes_list):
    """Add the packet delivery ratio for each nodes in the nodes list

    Parameters
    ----------
    nodes_list
        a list of Node

    Notes
    -----
    Packet delivery ratio = ∑ Number of packet received / ∑ Number of packet send
    
    """
    # Total of received nodes (for all nodes)
    total_tx = sum([len(n.list_tx) for n in nodes_list])
    for node in nodes_list:
        # Node pdr (%): Total received msg for this node / Total transmitted msg by other nodes
        node.pdr = (len(node.list_rx) / ( total_tx - len(node.list_tx)))*100

def averageTransmissionLatency(nodes_list):
    """Add the average transmission latency for each nodes in the nodes list

    Parameters
    ----------
    nodes_list
        a list of Node

    Notes
    -----
    The time is in ms
    The average time a transmitted msg take to be received by an other node
    
    """
    for n in nodes_list:
        n.sucess = 0 # Nb of time a transmitted msg has been successfully received by a node
        latency  = 0 # Total latency of the transmitted message by node n
        n.pdr    = 0 

        # For all transmitted msg of node n
        for tx in n.list_tx:
            # For all the other node n2 
            for n2 in nodes_list:
                # For all the received msg of n2
                for rx in n2.list_rx:
                    # If a received msg of n2 has the same id 
                    # than a transmitted message from n
                    if tx.msg_id == rx.msg_id:
                        n.success += 1
                        tx.success += 1
                        # We compute the latency between the transmission time of the msg
                        # and its reception time
                        rx.latency = rx.timestamp - tx.timestamp
                        latency += rx.timestamp - tx.timestamp
        if n.success:
            # We compute the average transmission latency by nodes
            n.latency = latency / n.success 
            n.pdr     =  len(n.list_tx) / n.success * 100
        


