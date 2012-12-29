# CISSE Mohamed
# SABER Takfarinas
# M2 ORO

# download all sscplp instances (p?.txt) from Professor Elena Fernandez
function download_sscplp_instances 
{
    echo "Downloading sscplp instances from http://www-eio.upc.es/~elena/sscplp/ ..."
    for i in `seq 1 57`; do 
	wget -c "http://www-eio.upc.es/~elena/sscplp/p"$i".txt" >> /dev/null 2>&1
    done    
    echo "Done"
}

# remove all sscplp instances (p?.txt) downloaded
function delete_sscplp_instances
{
    echo "Removing sscplp instances ..."
    for i in `seq 1 57`; do 
	rm -rf "p"$i".txt"
    done
    echo "Done"
}

# remove all txt files
function delete_txt_files
{
    echo "Cleaning the directory ..."
    rm -rf F*.txt
    rm -rf vector*
    echo "Done"
}

# From sscplp files (http://www-eio.upc.es/~elena/sscplp/), 
# generate file of the following format :
############################################################
#BEGIN OF FILE
# m : nbr of customers
# n : nbr of facilities
#
# matrix of assignation [ m x n ] over the first objective
# matrix of assignation [ m x n ] over the second objective
#
# cost of opening a facility over the first objective
#
# cost of opening a facility over the second objective
#
# demands of customers
#
# capacities of facilities
#END OF FILE
#############################################################
# e.g : create_cflp_instances lb ub m n
# lb, ub, m, n are integers
# [lb, ub] is an interval 
# only the following calls are possible :
# create_cflp_instances 1 6 20 10
# create_cflp_instances 7 17 30 15
# create_cflp_instances 18 25 40 20
# create_cflp_instances 26 33 50 20
# create_cflp_instances 34 41 60 30
# create_cflp_instances 42 49 75 30
# create_cflp_instances 50 57 90 30

function create_cflp_instances
{
    arg1=$1 
    arg2=$2
    m=$3   # nbr of customers
    n=$4   # nbr of facilities
    for i in `seq $arg1 $((arg2-1))`; do
	j=$((i+1))
	while [ $j -le $arg2 ]; do
	    # filename to generate
	    myfilename="F"$i"-"$j".txt"
	    echo "Generating the file $myfilename"
	    # m : nbr of customers
	    echo "$m" >> $myfilename

	    # n : nbr of facilities
	    echo "$n" >> $myfilename

	    echo >> $myfilename

	    # matrix of assignation [ m x n ] over the first objective
	    # matrix of assignation [ m x n ] over the second objective
	    for alpha in $i $j; do 
		tmp1=1
		tmp2=1
		demands=1
		capacities=1
		costs=1
		for word in $(cat "p"$alpha".txt"); do 
		    if [ $tmp1 -gt $(((m*n)+2)) ]; then
			if [ $demands -lt $m ]; then
			    echo -n "${word:0:-3} " >> vector_customer_demands
			    demands=$((demands+1))
			elif [ $demands -eq $m ]; then
			    echo -ne "${word:0:-3}\n\n" >> vector_customer_demands
			    demands=$((demands+1))
			else 
			    if [ $costs -lt $n ]; then
				echo -n "${word:0:-3} " >> vector_costs
				costs=$((costs+1))
			    elif [ $costs -eq $n ]; then
				echo -ne "${word:0:-3}\n\n" >> vector_costs
				costs=$((costs+1))
			    else
				if [ $capacities -lt $n ]; then 
				    echo -n "${word:0:-3} " >> vector_facility_capacities
				    capacities=$((capacities+1))
				else 
				    echo -ne "${word:0:-3}\n\n" >> vector_facility_capacities
				fi
			    fi
			fi
			continue
		    fi

		    if [ $tmp1 -gt 2 ]; then
			echo -n "${word:0:-3}" >> $myfilename
			if [ $tmp2 -eq $n ]; then
			    echo -ne "\n" >> $myfilename
			    tmp2=1
			else
			    echo -ne " " >> $myfilename
			    tmp2=$((tmp2+1))
			fi
		    fi
		    tmp1=$((tmp1+1))
		done
		echo >> $myfilename
	    done

            # cost of opening a facility over the first objective
            # cost of opening a facility over the second objective
	    cat vector_costs >> $myfilename

	    # demands of customers
	    cat vector_customer_demands >> $myfilename

            # capacities of facilities
	    cat vector_facility_capacities >> $myfilename
	    rm -rf vector_facility_capacities vector_customer_demands vector_costs
	    echo "Done"
	    j=$((j+1))
	done
    done
}

delete_txt_files
download_sscplp_instances 

###
# TAKE CARE, there is a bug for F7-8.txt :
# because the file p8.txt does not respect the format
# there is in p8.txt m n ? in the first line
#
# and F42-43.txt :
# because there is a problem on file p43.txt, it does not respect the format too
# we need to remove the last character
###
# some corections :
mv p7.txt tmp.txt
mv p8.txt p7.txt
mv tmp.txt p8.txt

mv p42.txt tmp.txt
mv p43.txt p42.txt
mv tmp.txt p43.txt


create_cflp_instances 1 6 20 10
#create_cflp_instances 7 17 30 15 ?? bug without the corrections
create_cflp_instances 8 17 30 15

create_cflp_instances 18 25 40 20
create_cflp_instances 26 33 50 20
create_cflp_instances 34 41 60 30

#create_cflp_instances 42 49 75 30 ?? bug without the corrections
create_cflp_instances 43 49 75 30

create_cflp_instances 50 57 90 30
#delete_txt_files
