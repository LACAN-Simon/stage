# How to generate application traces with FIT IoT-LAB 

This project contain :
* A script to launch an experiment on the FIT IoT-LAB testbed and store the results on your computer
* A Python code base to parse and analyse the obtained data
* Some applications traces obtained with this method

## Experimentation script

This script will lauch an experiment on the FIT IoT-LAB testbed.
The experiment set nodes to send packets on broadcast with IEEE 802.15.4 on the RPL routing protocol.
Each nodes will write on the output the packet send or received. Then all nodes outputs are gathered with the testbed [serial aggregator](https://www.iot-lab.info/docs/tools/serial-aggregator/) and stored on your computer under the *log/<exp_id>/serial_output* file. You will find an example [here](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/log/248426). 

### Prerequisite steps

#### Dependencies 

* gcc:8.3.0

#### Set environment

*You must have FIT IoT lab credentials, you can register [here](https://www.iot-lab.info/testbed/signup).*

Store your credentials on your computer :
```
$ iotlab-auth -u <login>
```

Set the IoT-LAB environment in the directory of your choice :
```
$ git clone https://github.com/iot-lab/iot-lab.git path/to/iot-lab
```

Go to the root of this project and replace the code in Contiki :
```
$ cp src/broadcast-example.c \ 
	path/to/iot-lab/parts/contiki/examples/ipv6/simple-udp-rpl/broadcast-example.c
```
*You can obviously change this code to your convenience, just be sure that it doesn't enter in conflic with the modifications done to it in the script*

Set up Contiki :
```
$ cd path/to/iot-lab
$ make setup-contiki
```

Your environment is set !

#### Adjust the script 

Modify the variables in the script according to your environment :
* LOGIN: you FIT IoT-LAB login
* SITE: the site where you want to experiment, here you can find a description of each site : [grenoble](https://www.iot-lab.info/docs/deployment/grenoble/), [lille](https://www.iot-lab.info/docs/deployment/lille/), [lyon](https://www.iot-lab.info/docs/deployment/grenoble/), [saclay](https://www.iot-lab.info/docs/deployment/saclay/), [strasbourg](https://www.iot-lab.info/docs/deployment/saclay/)
* CODEDIR: the absolute path to the firmware (path/to/iot-lab/parts/contiki/examples/ipv6/simple-udp-rpl/broadcast-example.c)
* EXPDIR: the absolute path of this project's root, used to store log and retrieve scripts 

### Usage 

```
$ ./exp.sh <exp name> <interval (s)> <payload size (b)> <exp duration (m)> <nb packets> <list of nodes>"
```

Parameters :
* exp name: the name of the experience, used to store the results
* interval (s): at which interval nodes will send "nb packets" packet (ex: three packets every 3 seconds)
* payload size (b): the payload size in bytes
* exp duration (m): the experiment duration in minutes
* nb packets: the number of packets send each time, useful when you want to send more than one packet per seconds
* list of nodes: the list of nodes for the experiment, you can find useful examples [here](https://www.iot-lab.info/docs/tools/cli/#experiment-command). **Be aware that the site in your node list and in the script must be the same**

Example : 

```
$ ./exp.sh "MyExp" 6 12 1 1 "3,archi=m3:at86rf231+site=lille"
```
will launch an experience named "MyExp", during 1 minute, where nodes send 1 packet of 12 bytes every 6 seconds, on three [M3 nodes](https://www.iot-lab.info/docs/boards/iot-lab-m3/) at Lille.

## Python sources

We provide some sources to parse and analyse the log you will obtain with the script.
It will produce two files :
* One grouping all received packets, named <exp name>Rx.data with the format : received;<node name>;<timestamp (ms)>;<message id>;<reception delay (ms)> 
* The other grouping all transmitted packets, named <exp name>Tx.data with the format : transmitted;<node name>;<timestamp (ms)>;<message id>;<transmission success (nb of nodes that have received this packet)> 

It will also produce two plots :
* AverageTransmissionLatency.png that show the average transmission latency in ms per node
* PacketDeliveryRatio.png that show the packet delivery ration per node

This code is documented so you can easily use it and modify it to perform your own metrics and plot.

### Dependencies

* Python: 3.6.9
* pip: 9.0.1 
* matplotlib: 3.3.3

### Usage 

```
$ python3 main.py <log filename> <exp name>
```

Example :
```
$ python3 python_src/main.py ../log/248426/serial_output MyExp
```
will produce the directory [MyExp](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/MyExp).


## Applications Traces 

We provide some application traces obtained with the script and the Python code.
Here the experiments parameters we used :

| Scenario                                                                                                | Nb nodes | Packet sending frequency | Payload size (bytes) | Experiment duration (minutes) |
|---------------------------------------------------------------------------------------------------------|----------|--------------------------|----------------------|-------------------------------|
| [Smart HVAC](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/app_traces/HVAC)                     | 100      | 1 packet/4 minutes       | 60                   | 60                            |
| [Smart lighting](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/app_traces/Lighting)             | 100      | 1 packet/8 minutes       | 30                   | 90                            |
| [Emergency Response](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/app_traces/Emergency)        | 40       | 1 packet/30 seconds      | 127                  | 10                            |
| [Surveillance](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/app_traces/VoIP)                   | 30       | 99 packets/seconds       | 127                  | 10                            |
| [Augmented/Virtual reality](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/app_traces/AR)        | 10       | 197 packets/seconds      | 127                  | 10                            |
| [Voice on IP (VoIP)](https://gitlab.irisa.fr/0000H82G/traces/-/tree/master/app_traces/VoIP)             | 10       | 16 packets/seconds       | 127                  | 10                            |
