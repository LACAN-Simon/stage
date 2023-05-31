#!/bin/bash
# exp.sh: launch experiment on IoT-lab, log & retrieve results from server

set -e

#---------------------- TEST ARGUMENTS ----------------------#
if [ "$#" -ne 6 ]; then
	echo "Usage: $0 <exp name> <interval (s)> <payload size (B)> <exp duration (m)> <packets per seconds> <list of nodes>"
	exit
fi
#---------------------- TEST ARGUMENTS ----------------------#

#--------------------- DEFINE VARIABLES ---------------------#
LOGIN="yourlogin"
SITE="lille"
IOTLAB="$LOGIN@$SITE.iot-lab.info"
CODEDIR="${HOME}/iot-lab/parts/contiki/examples/ipv6/simple-udp-rpl"
EXPDIR="${HOME}/traces"
#--------------------- DEFINE VARIABLES ---------------------#

#----------------------- CATCH SIGINT -----------------------#
# For a clean exit from the experiment
trap ctrl_c INT
function ctrl_c() {
	echo "Terminating experiment."
	iotlab-experiment stop -i "$EXPID"
	exit 1
}
#----------------------- CATCH SIGINT -----------------------#

#-------------------- CONFIGURE FIRMWARE --------------------#
sed -i "s/#define\ SEND_INTERVAL_SECONDS\ .*/#define\ SEND_INTERVAL_SECONDS\ $2/g" $CODEDIR/broadcast-example.c
sed -i "s/#define\ SEND_BUFFER_SIZE\ .*/#define\ SEND_BUFFER_SIZE\ $3/g" $CODEDIR/broadcast-example.c
sed -i "s/#define\ NB_PACKETS\ .*/#define\ NB_PACKETS\ $5/g" $CODEDIR/broadcast-example.c
#-------------------- CONFIGURE FIRMWARE --------------------#

#--------------------- COMPILE FIRMWARE ---------------------#
cd $CODEDIR
make TARGET=iotlab-m3 -j8 || { echo "Compilation failed."; exit 1; }
#--------------------- COMPILE FIRMWARE ---------------------#

#-------------------- LAUNCH EXPERIMENTS --------------------#
cd $EXPDIR/scripts

# Launch the experiment and obtain its ID
EXPID=$(iotlab-experiment submit -n $1 -d $4 -l $6 | grep id | cut -d' ' -f6)
# Wait for the experiment to began
iotlab-experiment wait -i $EXPID
# Flash nodes
iotlab-node --flash $CODEDIR/broadcast-example.iotlab-m3 -i $EXPID 
# Wait for contiki
sleep 10
# Run a script for logging and seeding
iotlab-experiment script -i $EXPID --run $SITE,script=serial_script.sh
# Wait for experiment termination
iotlab-experiment wait -i $EXPID --state Terminated
#-------------------- LAUNCH EXPERIMENTS --------------------#


#----------------------- RETRIEVE LOG -----------------------#
ssh $IOTLAB "tar -C ~/.iot-lab/${EXPID}/ -cvzf $1.tar.gz serial_output" 
mkdir $EXPDIR/log/$EXPID
scp "$IOTLAB":~/$1.tar.gz $EXPDIR/log/$EXPID/$1.tar.gz
cd $EXPDIR/log/$EXPID/
tar -xvf $1.tar.gz 
#----------------------- RETRIEVE LOG -----------------------#

exit 0
