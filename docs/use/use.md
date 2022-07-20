# Use

## Android

The Android operating system requires applications to ask for device location permission in order to scan for nearby Bluetooth Low Energy sensors. Android 10+ will ask for _ACCESS_FINE_LOCATION_, while Android 6+ will only ask for _ACCESS_COARSE_LOCATION_.  
The location permission is only needed while scanning for new sensors, not for updating data. You can learn more on Android developer [website](https://developer.android.com/guide/topics/connectivity/bluetooth/permissions#declare-android11-or-lower).

Some devices also require the GPS to be turned on while scanning for new sensors. This requirement is not documented and there is not much that can be done about it.  

## iOS

The application will ask you for permission to use Bluetooth. You can learn more on Apple [website](https://support.apple.com/HT210578).

## Linux ('live mode' and 'historical data')

While reading Flower Care and RoPot historical entry count, the sensors usually freeze for up to a second, and the Linux kernel consider that to be a connection timeout.  
To be able to get the historical data working on Linux, you'll need to increase the "supervision timeout" value (while root):

```bash
# echo 100 > /sys/kernel/debug/bluetooth/hci0/supervision_timeout
```

You could also give Theengs binary the _net_raw_ and _net_admin_ capabilities, so that it could be allowed to changes the supervision timeout on its own. But at the moment that's not fully implemented.

```bash
# setcap 'cap_net_raw,cap_net_admin+eip' theengs
```