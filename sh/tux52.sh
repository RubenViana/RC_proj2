#set eth0 up
ifconfig eth0 172.16.51.1/24

#add a route to tux53
route add -net 172.16.50.0/24 gw 172.16.51.253

#add a route to RC
route add -net 172.16.1.0/24 gw 172.16.51.254

#add default route
route add default gw 172.16.51.254