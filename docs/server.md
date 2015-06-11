# Server

A WheatIS Server exposes Services to be used by both Clients and also other WheatIS Servers. Grouping Servers together gives the ability to offer multiple Services without having to physically install each Service on every Server. Inside the WheatIS code, there are currently two Server instances. The first is the Apache httd module, mod_wheatis, which is the recommended solution and can be compiled and installed on at least httpd 2.2.x and higher. If for some reason you cannot run httpd there is also a standalone Server, which should be considered far less robust and should only be used by developers for experimental purposes and definitely never in a production environment.

## Mod_WheatIS

All of the configuration details for this module are added to the appropriate conf folder within an httpd installation and is of the form:

~~~
<LocationMatch "/wheatis">
LoadModule wheatis_module modules/mod_wheatis.so
SetHandler wheatis-handler
WheatISRoot /opt/wheatis
</LocationMatch>

Alias /wheatis/docs /opt/wheatis/docs
~~~


## Connecting Multiple Servers

TODO