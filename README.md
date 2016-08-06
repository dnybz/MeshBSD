MeshBSD (under construction)
============================

Extensions on FreeBSD 11-RELEASE operating system for embedded systems.

<pre><code>
 |
 | This project is at an early stage! 
 | 
 | Please, be patient, there is recent work 
 | in progress to reach the goal to provide
 | an operating system for embedded systems 
 | based on FreeBSD 11-RELEASE. 
 |
</code></pre>

After I've read 

<pre><code>
 | https://www.kernel.org/doc/Documentation/stable_api_nonsense.txt
</code></pre>
 
I was disgusted and I've decided to provide a set 
of components to build an operating system based on
compoments from FreeBSD and OpenBSD operating system. 

This operation is divided into three major stages:
--------------------------------------------------

<pre><code>
 #1 Buildup of code-base as free generating set for base operating system.

 #2 Divide set of given Makefiles and transition into a set of modularized
    components acting as toolkit for generating an operating system targeting  
    embedded systems and mobile stations.
    
 #3 Generate an developement environment for building aplications as
    subset on e. g. X.org based GUI.
</code></pre>

TL-WR1043NDv2 (experimental)
============================

Now, there exists an experimental firmware image 

<pre><code>
   MeshBSD.tl-wr1043ndv2.201601291719.factory.bin
</code></pre>

in releases directory (only) for demonstration purposes. Intentionally, 
this software is not released for use as production system or in context 
of production systems, because this software is not error-free. 

<pre><code>
   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
</code></pre>

Basic requirements for installation of (experimental) firmware image are:

<pre><code>
 * Soldered header on-board, which is used for serial 
   access through interconnected usb cable. 
   
    +-----+-----+-----+-----+
    | VCC   GND   RX  | TX  |
    +-----+-----+-----+-----+
     3.3V

 * Configured tftpd(8) daemon.
 
 * At least cu(1) -- Call Unix.
</code></pre>
   
On host running FreeBSD, access to serial console might 
be performed e. g. through use of priveleged terminal:   

<pre><code> 
 | # cu -l /dev/ttyU0 -s 115200
</code></pre>

Then, appliance (or router board) shall be switched on. 

<pre><code>
 | 
 | U-Boot 1.1.4 (Jun 13 2014 - 15:14:01)
 | 
 | ap135 - Scorpion 1.0DRAM:  
 | sri
 | Scorpion 1.0
 | ath_ddr_initial_config(178): (16bit) ddr2 init
 | tap = 0x00000003
 | Tap (low, high) = (0x0, 0x1c)
 | Tap values = (0xe, 0xe, 0xe, 0xe)
 | 64 MB
 | Flash Manuf Id 0xef, DeviceId0 0x40, DeviceId1 0x17
 | flash size 8MB, sector count = 128
 | Flash:  8 MB
 | Using default environment
 | 
 | *** Warning *** : PCIe WLAN Module not found !!!
 | *** Warning *** : PCIe WLAN Module not found !!!
 | In:    serial
 | Out:   serial
 | Err:   serial
 | Net:   ath_gmac_enet_initialize...
 | athrs_sgmii_res_cal: cal value = 0x6
 | No valid address in Flash. Using fixed address
 | No valid address in Flash. Using fixed address
 | ath_gmac_enet_initialize: reset mask:c02200 
 | Scorpion  ----> S17 PHY *
 | athrs17_reg_init: complete
 | : cfg1 0x80000000 cfg2 0x7114
 | eth0: ba:be:fa:ce:08:41
 | eth0 up
 | athrs17_reg_init_wan done
 | SGMII in forced mode
 | athr_gmac_sgmii_setup SGMII done
 | : cfg1 0x800c0000 cfg2 0x7214
 | eth1: ba:be:fa:ce:08:41
 | eth1 up
 | eth0, eth1
 | Setting 0x18116290 to 0x58b1a14f
 | Autobooting in 1 seconds
</code></pre>

The booting procedure may interrupted by entering 

<pre><code> 
   tpl
</code></pre>

within time intervall, shown as above. Then 

<pre><code>
 | ap135>
</code></pre>

occours as prompt on screen, awaiting input.

<pre><code>
 | ap135> printenv 
</code></pre>
 
shows environment for bootstrapping an operating system.  

<pre><code>
 | bootargs=console=ttyS0,115200 root=31:02 rootfstype=jffs2 init=/sbin/init mtdparts=ath-nor0:256k(u-boot),64k(u-boot-env),6336k(rootfs),1408k(uImage),64k(mib0),64k(ART)
 | bootcmd=bootm 0x9f020000
 | bootdelay=1
 | baudrate=115200
 | ethaddr=0xba:0xbe:0xfa:0xce:0x08:0x41
 | ipaddr=192.168.1.111
 | serverip=192.168.1.100
 | dir=
 | lu=tftp 0x80060000 ${dir}u-boot.bin&&erase 0x9f000000 +$filesize&&cp.b $fileaddr 0x9f000000 $filesize
 | lf=tftp 0x80060000 ${dir}ap135${bc}-jffs2&&erase 0x9f050000 +0x630000&&cp.b $fileaddr 0x9f050000 $filesize
 | lk=tftp 0x80060000 ${dir}vmlinux${bc}.lzma.uImage&&erase 0x9f680000 +$filesize&&cp.b $fileaddr 0x9f680000 $filesize
 | stdin=serial
 | stdout=serial
 | stderr=serial
 | ethact=eth0
 |
 | Environment size: 686/65532 bytes
</code></pre>

It is obvious that host might be configured by

<pre><code>
 | # ifconfig re0 inet 192.168.1.100/24 up 
 | # route add -4 192.168.1.111 192.168.1.100
 | add host 192.168.1.111: gateway 192.168.1.100
</code></pre>

in conjuction with 

<pre><code>
 | # cat >> /etc/inetd.conf << EOF
 | > tftp dgram udp wait root /usr/libexec/tftpd tftpd -l -s /tftpboot
 | > EOF
</code></pre>

then

<pre><code>
 | # service inetd {one}{re}start
</code></pre>

initializes tftpd(8) daemon manually.

<pre><code>
 | ap135> tftpboot 0x80060000 MeshBSD.tl-wr1043ndv2.201601291719.factory.bin
</code></pre>

loads 

<pre><code>
 | dup 1 speed 1000
 | Using eth1 device
 | TFTP from server 192.168.1.100; our IP address is 192.168.1.111
 | Filename 'MeshBSD.tl-wr1043ndv2.201601291719.factory.bin'.
 | Load address: 0x80060000
 | Loading: #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #################################################################
 |          #########
 | done
 | Bytes transferred = 6368768 (612e00 hex)
</code></pre>

the (experimental) firmware image into random access memory starting at 
address 0x80060000 on (this) ap135 (router) board.

<pre><code>
 | ap135> erase 0x9f020000 +0x7c0000
 | Erasing flash... 
</code></pre>

erases contents of flash memory at 0x9f020000 through 0x7c0000 bytes offset:

<pre><code>
 | First 0x2 last 0x7d sector size 0x10000
 | 125
 | Erased 124 sectors  
</code></pre>
 
The flash memory is now ready for copying in the firmware

<pre><code>
 | ap135> cp.b 0x80060000 0x9f020000 0x7c0000
 | Copy to Flash... write addr: 9f020000
 | done 
</code></pre>

then 

<pre><code>
 | ap135> bootm 0x9f020000
</code></pre>
 
starts bootstrapping. 

<pre><code> 
 | ## Booting image at 9f020000 ...
 |    Uncompressing Kernel Image ... OK
 |
 | Starting kernel ...
</code></pre>

When operating system kernel and init(8) process are running, then

<pre><code>
 | FreeBSD/mips (styx.testenv.local) (ttyu0)
 |
 | login: 
</code></pre>

shall occour as login prompt on serial console. After logging in 

<pre><code>
 | login: root
</code></pre>

by use of an empty password. It will be changed by

<pre><code> 
 | # passwd root
 | Changing local password for user
 | New Password:
 | Retype New Password:
</code></pre>

then

<pre><code>
 | # ifconfig
</code></pre>
 
provides a listing 

<pre><code>
 | arge0: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> metric 0 mtu 1500
 |        options=8<VLAN_MTU>
 |        ether c4:6e:1f:b9:19:e1
 |        media: Ethernet 1000baseT <full-duplex>
 |        status: active
 | arge1: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> metric 0 mtu 1492
 |        options=8<VLAN_MTU>
 |        ether c4:6e:1f:b9:19:e2
 |        inet 192.168.1.1 netmask 0xffffff00 broadcast 192.168.1.255 
 |        media: Ethernet 1000baseT <full-duplex>
 |        status: active
 | lo0: flags=8049<UP,LOOPBACK,RUNNING,MULTICAST> metric 0 mtu 16384
 |        options=600003<RXCSUM,TXCSUM,RXCSUM_IPV6,TXCSUM_IPV6>
 |        inet 127.0.0.1 netmask 0xff000000 
 |        groups: lo 
</code></pre>

about configured network interfaces on link-layer. Any persistent configuration 
on network interfaces shall described by

<pre><code> 
    /etc/hostname.if
</code></pre>
 
configuration file. Its contents provides either by dhclient(8) or ifconfig(8) 
accepted argument-vector for processing during execution of /etc/rc2, if and 
only if interface category was enlisted by

<pre><code>
    network_interface_types="arge"
</code></pre>
    
variable in

<pre><code>
    /etc/rc.conf
</code></pre>

configuration file. Implecitely, 

<pre><code>
 | # cat >> /etc/hostname.wlan0 << EOF
 | > create wlandev ath0 wlanmode adhoc
 | > inet 192.168.2.1 netmask 255.255.255.0 ssid tiamat
 | > mtu 1492
 | > up
 | > EOF 
</code></pre>

in conjunction with 

<pre><code>
    network_interface_types="arge wlan"
</code></pre>
  
creates an instance of wlan(4) device automatically. Therefore

<pre><code>
 | # cfg_save 
</code></pre>
    
stores any in 

<pre><code>
    /etc/cfg/manifest 
</code></pre>
    
enlisted and e. g.  by

<pre><code>
 | # cat >> /etc/cfg/manifest << EOF
 | > etc/hostname.wlan0
 | > EOF
</code></pre>

additionally promoted 

<pre><code> 
 | *** Storing configuration files from /etc/cfg/manifest -> /dev/map/cfg..
 | etc/cfg/manifest
 | etc/ppp/ppp.conf
 | etc/dhcpd.conf
 | etc/group
 | etc/hostapd.conf
 | etc/hostname.arge0
 | etc/hostname.arge1
 | etc/ipsec.conf
 | etc/login.conf
 | etc/master.passwd
 | etc/myname
 | etc/nsswitch.conf
 | etc/ntp.conf
 | etc/passwd
 | etc/pf.conf
 | etc/rc.conf
 | etc/services
 | etc/ttys
 | etc/wpa_supplicant.conf
 | etc/hostname.wlan0
 | 36 blocks
 | 0+1 records in
 | 1+0 records out
 | 65536 bytes transferred in 0.497583 secs (131709 bytes/sec)
 | *** Completed.
</code></pre>

configuration file as compressed image into NVRAM. When

<pre><code>
 | # reboot
</code></pre>
 
takes place then

<pre><code>
 | # ifconfig
</code></pre>
 
shows 

<pre><code>
 | arge0: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> metric 0 mtu 1500
 |         options=8<VLAN_MTU>
 |         ether c4:6e:1f:b9:19:e1
 |         media: Ethernet 1000baseT <full-duplex>
 |         status: active
 | arge1: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> metric 0 mtu 1492
 |         options=8<VLAN_MTU>
 |         ether c4:6e:1f:b9:19:e2
 |         inet 192.168.1.1 netmask 0xffffff00 broadcast 255.255.255.0 
 |         media: Ethernet 1000baseT <full-duplex>
 |         status: active
 | lo0: flags=8049<UP,LOOPBACK,RUNNING,MULTICAST> metric 0 mtu 16384
 |         options=600003<RXCSUM,TXCSUM,RXCSUM_IPV6,TXCSUM_IPV6>
 |         inet 127.0.0.1 netmask 0xff000000 
 |         groups: lo 
 | wlan0: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> metric 0 mtu 1492
 |         ether c4:6e:1f:b9:19:e0
 |         inet 192.168.2.1 netmask 0xffffff00 broadcast 192.168.2.255 
 |         media: IEEE 802.11 Wireless Ethernet autoselect mode 11ng <adhoc>
 |         status: running
 |         ssid tiamat channel 10 (2457 MHz 11g ht/40-) bssid 86:18:5a:e0:41:d5
 |         country US ecm authmode OPEN privacy OFF txpower 27 scanvalid 60
 |         protmode CTS ampdulimit 8k ampdudensity 8 shortgi wme burst ff
 |         groups: wlan
</code></pre>
 
enabled wlan(4) device. If e. g.

<pre><code>
    arge0
</code></pre>

may configured by DHCP, then

<pre><code>
 | # cat > /etc/hostname.arge0 << EOF
 | > dhcp
 | > EOF 
</code></pre>

overwrites contents of interface-specific configuration file and enables DHCP.

Some additional information:
----------------------------

<pre><code>
 * Easy editor ee(1) is useable for editing ascii encoded files.
 
 * It is possible to perform NAT by pf(4) in conjunction with ppp(8).

 * Handling of incoming PPPoE connections by pppoed(8) is possible.
 
 * Manually manipulation of SAD / SPD for ipsec(4) by setkey(8).  

 * Utilizing gif(4), if_vether(4) and wlan(4) in conjunction with 
   if_bridge(4) is possible, e. g. in context with rfc-3378.

 * Optional if_gre(4) and if_vlan(4) support.
</code></pre>

Additional information about contacting
=======================================
      
If someone wants to contact me by electronic mail, please use encryption.

<pre><code>
-----BEGIN PGP PUBLIC KEY BLOCK-----
Version: GnuPG v2

mQENBFYMezIBCACo8X47yor6hI3Rwd2vYr+R2f35ZJw1Zq6qzQXYhWhn2CNf4gYJ
5+hEBi5LJcSFhSvujo/xy3OZzL8a4YN/vFWGTZhuyk20MOx96yjzLLbXD9lxHd+a
AoSPuPe78QSTAw7azv7PtUSTnH0KzLCC2Rh1yODYmU4bBw5Aeso/mmWNebh6hd7r
Azp3ruLji1YorWTUHWWDbq+EsB3bSvNq6hmGiOnTsWlhhdOre4ny0OD0Tig6OgFR
S3fkzofnroJN21MdAgofksaeClzdEgSDor1Yk/tcdCHRu4/kHEdEljD6YdpzWbKx
f6BsqMFLHKrksEF8H7oH+Cq3izXOeziy9TsVABEBAAG0Okhlbm5pbmcgTWF0eXNj
aG9rIDxoZW5uaW5nLm1hdHlzY2hva0BzdHVkLmZoLWZsZW5zYnVyZy5kZT6JATcE
EwEIACEFAlYMezICGwMFCwkIBwIGFQgJCgsCBBYCAwECHgECF4AACgkQzcSBpLKQ
n3Xocgf8Dcp8MoACABJbUDMHGzFOScLhSugj6zcWZVJ96Uyj1B4yrshk1GiSOid5
OkY+g0BLZDsZ6L/ikY55jh4FMRw6Ox6sh2NX1rT4TVVkJJwiG6KLTwvLpqknaRXX
SoSKRt+U2JYhVLX8UY5TGlqtz5jtUm6jB8i2W64EFXYGl161rELEYmpienHvrFH7
rDMIHdBNlc4bJRiJU/qN5/28+BPjnFmG2/xVv7NlnH01GTPIXx2WfmkcgqNnleZS
d74iTejqFtB3jMws9zSCgLK5G684YeFJbN0mYdnZ+JonwaGti4oV91Ey/1NN0dHH
dgiA/njv+Sf17fwDxHLcj7RMesjZ7bkBDQRWDHsyAQgAyZyyysgBBysI0UqYL/27
1mNWABM3Ok6MinkrCy/oeqvp0zj4xocfzvjqpNEC9R2tzIxCtni+c1T2a4eoLSvu
G2TRrncPxHSxvGCClwQxlkS5INp4Y2NCEq4s+Fo0OyTawXGTTTgNEPK8yviK+0nh
jcpEcCNhGMArkNR6G0W6M2k6v3k0A2fMJ0ARFFj85kpbPv1IMGLs8HbWUe2D/1KQ
rJsCGU5tjiOXYL1/KfXDBhfw+fwC5AM+Ndxlhpla3Z+0RaxCjvQT7Z501U311aMh
kbfmS+Llvq3cZNDleNkWkHsWYgYL4wZqnrVQDfeqzL+moFjBHtLn+ZZbLn6OYc1f
qQARAQABiQEfBBgBCAAJBQJWDHsyAhsMAAoJEM3EgaSykJ91zKoH/jzxQSy7pUZx
Pe4ktFRJwil8g6CGUncVaV+Sxe0A+52dlk85W/4F+wMROvg5tc98uZeLH8Ye0BSI
EDwJD5Iel9qI+qQegqzvGkjuJzZx86XhFWBa8dmIzRgqeAZrblmpv9k5V1cyMSUO
2v/GaJOu3P9Jb9RPR0YVsiZTs3+R1Z6V05pBdbMG5bUVHvjEIFahmHKc+cvxwjPV
wY7U3JsZk7bYZG05pUstLIuNJ//UMVjC/dM6ofKTyLknbKDYKvJvcmPBSSeC7VXg
u5E2GRq1FgrjmjTS8r+/zZfV31iFkIdc5gC9ipwBsA7H6Bx8lPP5M0l1MXS/wAWU
8GZ4NRztiBU=
=lsmx
-----END PGP PUBLIC KEY BLOCK-----
</code></pre>


