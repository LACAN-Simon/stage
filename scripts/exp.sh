#!/bin/bash
# exp.sh: launch experiment on IoT-lab, log & retrieve results from server

set -e

#---------------------- TEST ARGUMENTS ----------------------#
if [ "$#" -ne 5 ]; then
	echo "Usage: $0 <exp name> <exp duration (m)> <interval (s)> <packets per seconds> <list of nodes>"
	exit
fi  
#---------------------- TEST ARGUMENTS ----------------------#

#--------------------- DEFINE VARIABLES ---------------------#
LOGIN="lacan"
SITE="lille"
LU=",m3,"  
L="$SITE$LU$5"
IOTLAB="$LOGIN@$SITE.iot-lab.info"
CODEDIR="${HOME}/path/to/iot-lab/parts/contiki/examples/ipv6/simple-udp-rpl"
EXPDIR="${HOME}/stage"
#--------------------- DEFINE VARIABLES ---------------------#

#----------------------- CATCH SIGINT -----------------------#
# For a clean exit from the experiment
trap ctrl_q INT
function ctrl_q() {
	echo "Terminating experiment."
	iotlab-experiment stop -i "$EXPID"
	exit 1
}
#----------------------- CATCH SIGINT -----------------------#

#-------------------- CONFIGURE FIRMWARE --------------------#
sed -i "s/#define\ SEND_INTERVAL_SECONDS\ .*/#define\ SEND_INTERVAL_SECONDS\ $3/g" $CODEDIR/broadcast-example.c
sed -i "s/#define\ NB_PACKETS\ .*/#define\ NB_PACKETS\ $4/g" $CODEDIR/broadcast-example.c
#-------------------- CONFIGURE FIRMWARE --------------------#

#--------------------- COMPILE FIRMWARE ---------------------#
cd $CODEDIR
make TARGET=iotlab-m3 -j8 || { echo "Compilation failed."; exit 1; }
#--------------------- COMPILE FIRMWARE ---------------------#

#-------------------- LAUNCH EXPERIMENTS --------------------#

# Launch the experiment and obtain its ID
cd 
EXPID=$(iotlab-experiment submit -n $1 -d $2 -l $L,gnrc_networking.elf | grep id | cut -d' ' -f6)
# Wait for the experiment to began
iotlab-experiment wait -i $EXPID

serial_aggregator


# Flash nodes
#W="115+117"
#T="$SITE$LU$W"
iotlab-node --flash $CODEDIR/broadcast-example.iotlab-m3 -i $EXPID 
# Wait for contiki
sleep 10
cd $EXPDIR/scripts
# Run a script for logging and seeding
iotlab-experiment script -i $EXPID --run $SITE,script=serial_script.sh
# Wait for experiment termination 
iotlab-experiment wait -i $EXPID --state Terminated  
#-------------------- LAUNCH EXPERIMENTS --------------------#


#----------------------- RETRIEVE LOG -----------------------#
tar -C ~/.iot-lab/${EXPID}/ -cvzf $1.tar.gz serial_output 
mkdir $EXPDIR/log/$EXPID
scp ~/stage/scripts/$1.tar.gz $EXPDIR/log/$EXPID/$1.tar.gz
cd $EXPDIR/scripts
rm -r $1.tar.gz
cd $EXPDIR/log/$EXPID/
tar -xvf $1.tar.gz 
#----------------------- RETRIEVE LOG -----------------------#

exit 0
