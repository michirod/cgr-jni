#!/bin/bash
#
# invoke with router directory as first parameter (epidemic, ocgr, prophetV2)
# then indicate the simulation mode as second parameter (all, buffer, message,
# ttl)
# the third parameter must be "-b N" OR "N" with N indicating the parameter of 
# the -b option as indicated in the ONE README file, OR the number of the batch
# test to run if batch mode is disabled (if unsure put 1)
# the last parameter must be "redirect" if the output has to be redirected to a 
# log file in the output directory. If "redirect" is not indicated, the output
# will be printed to stdout.
# the output directory is named with the timestamp of the beginning of the 
# simulation and stored into the router folder.
# the settings file are read in the following order:
#	./global_setting_test.txt
#	./global_setting_test_<mode>.txt
#	./<router>/<router>_settings.txt
#
if ! test -f $ONE_DIR/one.sh
then
	echo "can't find one.sh script into $dir"
	exit
fi

dir=$1
batchoption=""
redirect=""
for i in $@
do
case $i in
	"all"*)
	all=all
	;;
	"buffer"*)
	buffer=buffer
	;;
	"message"*)
	mesage=message
	;;
	"ttl"*)
	ttl=ttl
	;;
	"-b"*)
	batchoption="-b"
	;;
	"redirect"*)
	redirect="redirect"
	;;
esac
done
if [ "$batchoption" = "-b" ]
then
number=$4
else
number=$3
fi
simdir=`pwd`/
results=$(date '+%F-%R')
bufferdir=buffer
messagedir=message
ttldir=ttl
global_settings=global_settings_test.txt
buffer_settings=global_settings_test_buffer.txt
message_settings=global_settings_test_message.txt
ttl_settings=global_settings_test_ttl.txt
settingsFilePattern=_settings.txt
conf=$dir/$dir$settingsFilePattern
if ! test -d $dir
then
	echo "can't find directory $dir"
	exit
fi
if ! test -f $conf
then
	echo "can't find router specific configuration file into $dir"
	exit
fi
if ! test -f $global_settings
then
	echo "can't find settings file $global_settings"
	exit
fi
if ! test -f $buffer_settings
then
	echo "can't find settings file $buffer_settings"
	exit
fi
if ! test -f $message_settings
then
	echo "can't find settings file $message_settings"
	exit
fi
if ! test -f $ttl_settings
then
	echo "can't find settings file $ttl_settings"
	exit
fi

cd $ONE_DIR

if [ "$buffer" ] || [ "$all" ]
then
	#first test: Variable buffer size

	arguments="$batchoption $number $simdir$global_settings $simdir$buffer_settings $simdir$conf"
	argumentsFilename="$batchoption $number $(basename "$simdir$global_settings") $(basename "$simdir$buffer_settings") $(basename "$simdir$conf")" 
	outputdir="$simdir$dir/$results/$bufferdir/"
	mkdir -p $outputdir
	cp $simdir$global_settings $simdir$buffer_settings $simdir$conf $outputdir
	if [ "$redirect" ] 
	then 
		redirect="$outputdir/logfile"
		echo "redirect to $redirect"
		echo "" > "$redirect"
		echo "Performing ./one.sh $argumentsFilename" >> "$redirect"
		./one.sh "$arguments" >> "$redirect"
	else
		echo "Performing ./one.sh $argumentsFilename"
		./one.sh $arguments
	fi
	echo "moving reports to $outputdir"
	mv BufferMSR* BufferELG* $outputdir
fi
if [ "$message" ] || [ "$all" ]
then
	#second test: Variable messages size

	arguments="$batchoption $number $simdir$global_settings $simdir$message_settings $simdir$conf"
	argumentsFilename="$batchoption $number $(basename "$simdir$global_settings") $(basename "$simdir$message_settings") $(basename "$simdir$conf")" 
	outputdir="$simdir$dir/$results/$messagedir/"
	mkdir -p $outputdir
	cp $simdir$global_settings $simdir$message_settings $simdir$conf $outputdir
	if [ "$redirect" ] 
	then 
		redirect="$outputdir/logfile"
		echo "redirect to $redirect"
		echo "" > "$redirect"
		echo "Performing ./one.sh $argumentsFilename" >> "$redirect"
		./one.sh "$arguments" >> "$redirect"
	else
		echo "Performing ./one.sh $argumentsFilename"
		./one.sh $arguments
	fi
	echo "moving reports to $outputdir"
	mv MessageMSR* MessageELG* $outputdir
fi
if [ "$ttl" ] || [ "$all" ]
then
	#third test: Variable ttl length

	arguments="$batchoption $number $simdir$global_settings $simdir$ttl_settings $simdir$conf"
	argumentsFilename="$batchoption $number $(basename "$simdir$global_settings") $(basename "$simdir$ttl_settings") $(basename "$simdir$conf")" 
	outputdir="$simdir$dir/$results/$ttldir/"
	mkdir -p $outputdir
	cp $simdir$global_settings $simdir$ttl_settings $simdir$conf $outputdir
	if [ "$redirect" ] 
	then 
		redirect="$outputdir/logfile"
		echo "redirect to $redirect"
		echo "" > "$redirect"
		echo "Performing ./one.sh $argumentsFilename" >> "$redirect"
		./one.sh "$arguments" >> "$redirect"
	else
		echo "Performing ./one.sh $argumentsFilename"
		./one.sh $arguments
	fi
	echo "moving reports to $outputdir"
	mv TTLMSR* TTLELG* $outputdir
fi
