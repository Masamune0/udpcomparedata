Compare udp data of different packets from tcpdump and find the same bytes which are on all the packets.

It is a work in progress.


To have the same test3.txt I've used, in  the same format, example of a command :
tcpdump -n dst host ipyouwant and src host ipyouwant and port portyouwant -x and greater sizeyouwant > test3.txt
