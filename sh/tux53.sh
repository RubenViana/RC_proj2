#set eth0 up
ifconfig eth0 172.16.50.1/24

#add a route to tux52
route add -net 172.16.51.0/24 gw 172.16.50.254

#add default route
route add default gw 172.16.50.254