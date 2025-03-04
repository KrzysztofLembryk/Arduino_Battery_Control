# How to allow connection from other device to local server
- 1) run server on --host 0.0.0.0 and some specified port e.g. 8000 <br>
```uvicorn server:app --host 0.0.0.0 --port 8000```
- 2) check ip addr of device that hosts the server
- 3) on your phone or other device connected to the same WiFi type in web browser
ip_addr:port and connecto to the server
- 4) If no connection, check your other device ip and check if ip values from 
mask are the same on server device and your device e.g. mask: 255.255.255.0 
ip1: 192.168.0.24 and ip2: 192.168.0.222, first three values must be the same on
both devices with given mask for them to be in the same subnet, if they are not
you need to change some settings in router.
- 5) if both devices are in the same subnet and still there is no connection
firewall blocks the connection. To check it, it is advisable to download **TCPView**
on Windows to see connections on chosen port, and what .exe file is running 
your server
- 6) then go to firewall settings, check firewall rules for this programme and
change them to allow any connections for private networks
- 7) Now everything should work and you should be able to connect to your server
from other devices