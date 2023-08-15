---
title: Theengs BLE mobile application
---
# Theengs BLE mobile application

**Theengs app** is a multi platforms, multi devices **application** and BLE to MQTT **gateway** that leverages the [Theengs Decoder library](https://github.com/theengs/decoder).
It retrieves data from a wide range of [BLE sensors](prerequisites/devices) from Xiaomi, Inkbird, Govee, ClearGrass, Qingping, Ruuvi, Switchbot, Thermopro; LYWSD03MMC, CGD1, CGP1W, CGDN1, CGG1, H5055, H5072, H5075, H5102, TH1, TH2, BBQ, CGH1, CGDK2, CGPR1, RuuviTag, WS02, WS08, TPMS, MiScale, LYWSD02, LYWSDCGQ, MiFlora... and enables to read the advertised real time data from these sensors. 

It also translates this information into a readable JSON format and pushes those to an MQTT broker if you need. The Home Assistant MQTT convention is followed so that you can see your add your sensors in a few steps into your favorite controller.

The app uses the bluetooth component of your Phone/Tablet to read the sensor data.

![Iot](./img/Theengs-app-home-reduced-1280.png)

**Theengs app** can be used as a standalone solution or as a complementary solution to [OpenMQTTGateway](https://docs.openmqttgateway.com/) and/or [Theengs gateway](https://gateway.theengs.io) if you want a continuously running gateway.

The app will retrieve data from BLE sensors from Govee, Xiaomi, Inkbird, QingPing, ThermoBeacon, ClearGrass, Blue Maestro and many more.

For more information view the [documentation](https://app.theengs.io/)

<center><a href="https://apps.apple.com/us/app/theengs-ble/id6443457651?itsct=apps_box_badge&amp;itscg=30200" style="display: inline-block; overflow: hidden; border-radius: 13px; width: 250px; height: 83px;"><img src="https://tools.applemediaservices.com/api/badges/download-on-the-app-store/black/en-us?size=250x83&amp;releaseDate=1667088000&h=c822551038086181cfac2a32c96cd47e" alt="Download on the App Store" style="border-radius: 13px; width: 250px; height: 83px;"></a></center>

<center><a href='https://play.google.com/store/apps/details?id=com.theengs.app&pcampaignid=pcampaignidMKT-Other-global-all-co-prtnr-py-PartBadge-Mar2515-1'><img alt='Get it on Google Play' src='https://play.google.com/intl/en_us/badges/static/images/badges/en_badge_web_generic.png' width=300px/></a></center>

::: tip Note
Theengs app read data "broadcasted" by the devices and does not connect to them, only the Mi Flora and ThermoBeacon historical data are retrieved through a connection. For the rest of the devices the data is read real time when the device emit it.
:::

Features comparison between OS:
| OS | Real time data | MQTT integration | Running in background | Home Assistant Auto Discovery |
|:-:|:-:|:-:|:-:|:-:|
|iOS|☑️|☑️||☑️|
|Android|☑️|☑️|☑️ *experimental*|☑️|

#### Third party projects used by Theengs app

* [Qt](https://www.qt.io) ([LGPL 3](https://www.gnu.org/licenses/lgpl-3.0.txt))
* [QtMqtt](https://www.qt.io) ([GPL 3](https://www.gnu.org/licenses/gpl-3.0.txt))
* [Arduino Json](https://arduinojson.org/) ([MIT](https://opensource.org/licenses/MIT))
* [Decoder](https://decoder.theengs.io/) ([GPL 3](https://www.gnu.org/licenses/gpl-3.0.txt))
* [StatusBar](https://github.com/jpnurmi/statusbar) ([MIT](https://opensource.org/licenses/MIT))
* [ekkesSHAREexample](https://github.com/ekke/ekkesSHAREexample) ([license](https://github.com/ekke/ekkesSHAREexample/blob/master/LICENSE))
* [SingleApplication](https://github.com/itay-grudev/SingleApplication) ([MIT](https://opensource.org/licenses/MIT))
* RC4 code from Christophe Devine ([GPL 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt))
* Graphical resources: [assets/COPYING](assets/COPYING)

*App Store and Apple logo are registered trademarks of Apple Inc.*

*Google Play and the Google Play logo are trademarks of Google LLC.*

::: warning Note
All product and company names are trademarks or registered trademarks of their respective holders. Use of them does not imply any affiliation with or endorsement by them.
:::

::: slot footer
[GPLv3 Licensed](https://github.com/theengs/home/blob/main/LICENSE) | Copyright © 2022-present Theengs
:::
