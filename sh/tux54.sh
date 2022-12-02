#set eth0 up
ifconfig eth0 172.16.50.254/24

#set eth1 up
ifconfig eth1 172.16.51.253/24

#enable ip forwarding and ignore broadcasts
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

#add a route to RC
route add -net 172.16.1.0/24 gw 172.16.51.254

#add default route
route add default gw 172.16.51.254