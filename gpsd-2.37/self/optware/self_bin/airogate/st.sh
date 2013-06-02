#comgt -d /dev/usb/tts/2

#pppd file /tmp/ppp/options.pppoe

#mount -o bind /jffs/opt /opt

/opt/sbin/lighttpd -f /opt/etc/lighttpd/lighttpd.conf -m /opt/lib/lighttpd

mknod -m 660 /dev/airogate c 247 0


#insmod /jffs/airogate_module/ip_queue.ko
#insmod /jffs/airogate_module/kshm.ko
#insmod /jffs/airogate_module/iptable_airogate.ko
#insmod /jffs/airogate_module/ipt_airogate_target.ko
#insmod /jffs/airogate_module/uk_ser.ko

insmod /opt/self_module/airogate/ip_queue.ko
insmod /opt/self_module/airogate/kshm.ko
insmod /opt/self_module/airogate/iptable_airogate.ko
insmod /opt/self_module/airogate/ipt_airogate_target.ko
insmod /opt/self_module/airogate/uk_ser.ko

ln -sf /jffs/www /www/user
ln -sf /jffs/cgi-bin /www/user/cgi-bin


/opt/self_bin/airogate/start &
/opt/self_bin/airogate/freeaccess  &


iptables -I INPUT -p tcp --dport 6868 -j ACCEPT
iptables -I INPUT -p tcp --sport 6868 -j ACCEPT
sleep 1

/opt/self_bin/airogate/iptables -t airogate -A INPUT -m multiport -p tcp -s 192.168.1.1 --source-port 80,6868 -j QUEUE
rm *.log
rm *.cap
/opt/self_bin/airogate/usfwd -d > u.log &
/opt/self_bin/airogate//portald -d > p.log &

#/jffs/airo_bin/usfwd &
#/jffs/airo_bin/portald &

