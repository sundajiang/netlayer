#!/bin/sh
echo "run ip_interface process!"
inode_id=2
readonly inode_id
./ip_interface $inode_id 

sleep 0

echo "config tap0.."
ifconfig tap0 "192.168.2.$inode_id" netmask 255.255.255.0 up 

echo "config arp.."
arp -s 192.168.2.3 11:22:33:44:55:d3

echo "config route.."
#route add -net 192.168.3.0 netmask 255.255.255.0 dev tap0
route add -net 192.168.3.0 netmask 255.255.255.0 gw 192.168.2.3

echo "open the ip_forward"
echo 1 > /proc/sys/net/ipv4/ip_forward
