# FF_MQTT_Sync
This is an Eclipse Paho C library based Sync MQTT client (subscriber and publisher) plugin for Unreal Engine 5.

## THIRD PARTY LIBRARIES
1. Paho C Library: https://github.com/eclipse/paho.mqtt.c
2. OpenSSL 3 Binaries : You can build them by yourself or install pre-built binaries from here. We used v3.6.0. **Don't use any other version !**. or you have to build Paho C library with that version, again.</br>
https://slproweb.com/download/Win64OpenSSL-3_6_0.exe

## INSTALLATION INSTRUCTIONS
1. If you go with pre-built openssl from the web site that we provide you, make sure that you set binaries location to ``/bin`` folder. Install wizard will ask you about this.
2. Open ``FF_MQTT_Sync.uplugin`` and remove ``FF_OpenSSL`` plugin. **(We have centralized and closed source OpenSSL UE5 wrapper that satisfy all of our openssl dependent plugins. To run this plugin, you don't need that. So, just remove it to prevent issues.)**
3. Copy ``libcrypto-3-x64.dll`` and ``libssl-3-x64.dll`` files from OpenSSL v3 installation's ``/bin`` folder to plugin's ``Source/paho_c_sync/Win64/bin`` folder.
4. Make sure that you don't have any other plugin that use different ``OpenSSL v3`` than ``v3.6.0``. If you have, you either other build libraries with ``OpenSSL 3.6.0`` or ``Paho`` with your preferred ``OpenSSL v3`` version. Unreal's own OpenSSL module that use ``OpenSSL 1.1.t`` is not important. You can use it in other plugins if you want. **(unfortunately, UE5 already uses it.)**

## USAGE HINTS
* I assume that you already know how to connect an MQTT broker.
* You can see a sample blueprint in ``Content/BPs`` folder. Just change connection informations based on your broker.
* You need to put on of these protocols before URL ``mqtt://``, ``mqtts://``, ``ws://``, ``wss://``, ``ssl://``
* You need to put port after the URL

## DOCUMENTS
https://blueprintue.com/blueprint/tyjl3y62/

## ASYNC
https://github.com/FF-Plugins-Active/FF_MQTT_Async

## ROADMAP
* Android Platform support

## LICENSE
Our custom license allow you to use this plugin in your commercial projects freely, but you can't sell this plugin as a plugin with or without any modification on any marketplace.