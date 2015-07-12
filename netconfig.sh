#!/bin/bash
inode_id=2

#调试钩子,执行export DEBUG=true命令打开调试钩子
export DEBUG=true
DEBUG()
{
	if [ "$DEBUG" = "true" ]; then
		$@　　
	fi
}
DEBUG echo "debug opened!!"

set -x
echo "the value of DEBUG: $DEBUG"
set +x

echo "config ip of eth0/usb1"
ifconfig usb1 "192.168.$inode_id.240/24" up

echo "config mac&ip of tap0.."
#ifconfig tap0 down
ifconfig tap0 hw ether "00:11:22:33:44:$inode_id"
ifconfig tap0 "192.168.0.$inode_id" netmask 255.255.255.0 up

#使用route 命令添加的路由，机器重启或者网卡重启后路由就失效了
echo "config arp.."
counter=1
while [ $counter -lt 33 ]
do
	if [ $counter != $inode_id ]
	then
		echo "config arp of addr : $counter"
		arp -s "192.168.0.$counter" "00:11:22:33:44:$counter"
	fi
	counter=`expr $counter + 1`
done

#添加到网络的路由：发往192.168.3.0网段的数据包全部要经过网关192.168.0.3
echo "config route.."
counter=1
while [ $counter -lt 33 ]
do
	if [ $counter != $inode_id ]
	then
		echo "config route of net : $counter"
		route add -net "192.168.$counter.0/24" gw "192.168.0.$counter"
	fi
	counter=`expr $counter + 1`
done

echo "run route forward"
echo 1 > /proc/sys/net/ipv4/ip_forward


