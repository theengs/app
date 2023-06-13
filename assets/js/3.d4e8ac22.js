(window.webpackJsonp=window.webpackJsonp||[]).push([[3],{255:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-background-updates.5db55d76.png"},277:function(t,e,s){t.exports=s.p+"assets/img/Theengs-set-mac.0b596ac9.png"},278:function(t,e,s){t.exports=s.p+"assets/img/Theengs_app_auto_discovery_BLE_Sensor_Home_Assistant.e80531f8.gif"},279:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-location-permission-8.2b20daf0.png"},280:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-battery-excluded.d1996bd2.png"},281:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-location-permission.64c286d9.png"},282:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-nearby-permission.b2b60dd2.png"},283:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-unused-permission.9ced3c1f.png"},284:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-battery.3f30bc55.png"},285:function(t,e,s){t.exports=s.p+"assets/img/Theengs-app-battery-unrestricted.4ee75950.png"},302:function(t,e,s){"use strict";s.r(e);var a=s(14),i=Object(a.a)({},(function(){var t=this,e=t._self._c;return e("ContentSlotsDistributor",{attrs:{"slot-key":t.$parent.slotKey}},[e("h1",{attrs:{id:"use"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#use"}},[t._v("#")]),t._v(" Use")]),t._v(" "),e("h2",{attrs:{id:"features-comparison-between-os"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#features-comparison-between-os"}},[t._v("#")]),t._v(" Features comparison between OS")]),t._v(" "),e("table",[e("thead",[e("tr",[e("th",{staticStyle:{"text-align":"center"}},[t._v("OS")]),t._v(" "),e("th",{staticStyle:{"text-align":"center"}},[t._v("Real time data")]),t._v(" "),e("th",{staticStyle:{"text-align":"center"}},[t._v("MQTT integration")]),t._v(" "),e("th",{staticStyle:{"text-align":"center"}},[t._v("Running in background")]),t._v(" "),e("th",{staticStyle:{"text-align":"center"}},[t._v("Home Assistant Auto Discovery")])])]),t._v(" "),e("tbody",[e("tr",[e("td",{staticStyle:{"text-align":"center"}},[t._v("iOS")]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️")]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️")]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}}),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️")])]),t._v(" "),e("tr",[e("td",{staticStyle:{"text-align":"center"}},[t._v("Android")]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️")]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️")]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️ "),e("em",[t._v("experimental")])]),t._v(" "),e("td",{staticStyle:{"text-align":"center"}},[t._v("☑️")])])])]),t._v(" "),e("h2",{attrs:{id:"overview"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#overview"}},[t._v("#")]),t._v(" Overview")]),t._v(" "),e("video",{attrs:{width:"353",height:"628",controls:""}},[e("source",{attrs:{src:"/videos/Theengs-app-demo.mp4",type:"video/mp4"}}),t._v("\n  Your browser does not support the video tag.\n")]),t._v(" "),e("h2",{attrs:{id:"real-time-data"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#real-time-data"}},[t._v("#")]),t._v(" Real time data")]),t._v(" "),e("p",[t._v('The app enables to retrieve BLE sensor real time data, to add new sensors tap "Search for new sensors" in the hamburger menu.')]),t._v(" "),e("p",[t._v('To refresh the sensor data tap "Refresh sensor data" in the hamburger menu.')]),t._v(" "),e("div",{staticClass:"custom-block tip"},[e("p",{staticClass:"custom-block-title"},[t._v("Note")]),t._v(" "),e("p",[t._v("Why does Theengs App needs the location permission?")]),t._v(" "),e("p",[t._v("Theengs App requires the location permission to access your device Bluetooth and retrieves sensor data. Theengs is not accessing your location with GPS/GLONASS/BAIDU.")])]),t._v(" "),e("h2",{attrs:{id:"mqtt-integration"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#mqtt-integration"}},[t._v("#")]),t._v(" MQTT integration")]),t._v(" "),e("p",[t._v("Prerequisites:")]),t._v(" "),e("ul",[e("li",[t._v("MQTT broker setup in your local network (Mosquitto is a well known broker)")])]),t._v(" "),e("p",[t._v("Theengs app can publish the data to an MQTT broker, to do so go to the hamburger menu, select Integration.\nEnter the following informations:")]),t._v(" "),e("ul",[e("li",[t._v("Broker host IP (compulsory)")]),t._v(" "),e("li",[t._v("Broker port (compulsory, default: "),e("code",[t._v("1883")]),t._v(")")]),t._v(" "),e("li",[t._v("Username (optional)")]),t._v(" "),e("li",[t._v("Password (optional)")]),t._v(" "),e("li",[t._v("Topic (compulsory, default: "),e("code",[t._v("home")]),t._v(")")]),t._v(" "),e("li",[t._v("SubTopic (compulsory, default: "),e("code",[t._v("TheengsApp")]),t._v(")")])]),t._v(" "),e("p",[t._v('Once done click on the MQTT switch to activate the integration, if the app can connect to the broker, it will display "Connected" in the top right.')]),t._v(" "),e("video",{attrs:{width:"353",height:"767",controls:""}},[e("source",{attrs:{src:"/videos/Theengs-App-mqtt-integration.mp4",type:"video/mp4"}}),t._v("\n  Your browser does not support the video tag.\n")]),t._v(" "),e("h3",{attrs:{id:"ios-specificity"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#ios-specificity"}},[t._v("#")]),t._v(" iOS specificity")]),t._v(" "),e("p",[t._v("If you want to push data to an MQTT broker you will need to manually enter a MAC address for the sensor, this is done from the sensor page see below:")]),t._v(" "),e("p",[e("img",{attrs:{src:s(277),alt:"add-mac"}})]),t._v(" "),e("h3",{attrs:{id:"home-assistant-auto-discovery"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#home-assistant-auto-discovery"}},[t._v("#")]),t._v(" Home Assistant Auto Discovery")]),t._v(" "),e("p",[t._v("Theengs app can publish your sensor definition following Home Assistant MQTT convention, so as to do this you need to activate “Enable discovery” into your MQTT Options and follow the steps below:")]),t._v(" "),e("p",[e("img",{attrs:{src:s(278),alt:"location"}})]),t._v(" "),e("p",[t._v("The sensor definition will be publish when the app connect to the broker. You can retrigger the publication by deactivating and reactivating the MQTT integration from the application.")]),t._v(" "),e("h2",{attrs:{id:"ios-permissions"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#ios-permissions"}},[t._v("#")]),t._v(" iOS permissions")]),t._v(" "),e("p",[t._v("The application will ask you for permission to use Bluetooth. You can learn more on Apple "),e("a",{attrs:{href:"https://support.apple.com/HT210578",target:"_blank",rel:"noopener noreferrer"}},[t._v("website"),e("OutboundLink")],1),t._v(".")]),t._v(" "),e("h2",{attrs:{id:"android-permissions"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#android-permissions"}},[t._v("#")]),t._v(" Android permissions")]),t._v(" "),e("p",[t._v("The Android operating system requires applications to ask for device location permission in order to scan for nearby Bluetooth Low Energy sensors.\nYou would need to go into the app information (long press on the icon -> (i)) so as to configure the app permissions properly.")]),t._v(" "),e("h3",{attrs:{id:"android-8"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#android-8"}},[t._v("#")]),t._v(" Android 8+")]),t._v(" "),e("ul",[e("li",[t._v("Click on Permissions")]),t._v(" "),e("li",[t._v('Activate "your location"')])]),t._v(" "),e("p",[e("img",{attrs:{src:s(279),alt:"location"}})]),t._v(" "),e("p",[t._v("You can now use the application. If you want to run the application in the background follow the process below:")]),t._v(" "),e("ul",[e("li",[t._v("Go back")]),t._v(" "),e("li",[t._v('Click on "Battery"')]),t._v(" "),e("li",[t._v('Click on "Power saving exclusions"')]),t._v(" "),e("li",[t._v("Activate the exclusion for Theengs")])]),t._v(" "),e("p",[e("img",{attrs:{src:s(280),alt:"location"}})]),t._v(" "),e("ul",[e("li",[t._v('And finally go into the app settings and toggle "Enable background updates"')])]),t._v(" "),e("p",[e("img",{attrs:{src:s(255),alt:"background"}})]),t._v(" "),e("h3",{attrs:{id:"android-10"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#android-10"}},[t._v("#")]),t._v(" Android 10+")]),t._v(" "),e("h4",{attrs:{id:"running-when-displayed-only"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#running-when-displayed-only"}},[t._v("#")]),t._v(" Running when displayed only")]),t._v(" "),e("ul",[e("li",[t._v("Click on Permissions")]),t._v(" "),e("li",[t._v("Click on Location permission")]),t._v(" "),e("li",[t._v('Select "Allow only while using the app", if you want the app to update sensors data only when the app is running at the front')]),t._v(" "),e("li",[t._v('Enable "Use precise location" if you have this option')])]),t._v(" "),e("h4",{attrs:{id:"running-in-the-background-experimental"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#running-in-the-background-experimental"}},[t._v("#")]),t._v(" Running in the background (experimental)")]),t._v(" "),e("p",[t._v("If you want the app to retrieve data in the background you will need to do the following steps:")]),t._v(" "),e("ul",[e("li",[t._v('Click on "Permissions"')]),t._v(" "),e("li",[t._v('Click on "Location"')]),t._v(" "),e("li",[t._v('Select "Allow all the time" if you want to update the sensors data in the background')]),t._v(" "),e("li",[t._v('Enable "Use precise location" if you have this option')])]),t._v(" "),e("p",[e("img",{attrs:{src:s(281),alt:"location"}})]),t._v(" "),e("ul",[e("li",[t._v("Go back")]),t._v(" "),e("li",[t._v('Click on "Nearby devices"')]),t._v(" "),e("li",[t._v('Click on "Allow"')])]),t._v(" "),e("p",[e("img",{attrs:{src:s(282),alt:"nearby"}})]),t._v(" "),e("ul",[e("li",[t._v("Go back 2 times")]),t._v(" "),e("li",[t._v('Deactivate "Remove permissions if app is unused"')])]),t._v(" "),e("p",[e("img",{attrs:{src:s(283),alt:"unused"}})]),t._v(" "),e("ul",[e("li",[t._v("Click on battery")])]),t._v(" "),e("p",[e("img",{attrs:{src:s(284),alt:"battery"}})]),t._v(" "),e("ul",[e("li",[t._v("Select unrestricted")])]),t._v(" "),e("p",[e("img",{attrs:{src:s(285),alt:"unrestricted"}})]),t._v(" "),e("ul",[e("li",[t._v('And finally go into the app settings and toggle "Enable background updates"')])]),t._v(" "),e("p",[e("img",{attrs:{src:s(255),alt:"background"}})]),t._v(" "),e("p",[t._v("Some devices also require the GPS to be turned on while scanning for new sensors.")]),t._v(" "),e("div",{staticClass:"custom-block tip"},[e("p",{staticClass:"custom-block-title"},[t._v("Note")]),t._v(" "),e("p",[t._v("Android has drastically reduced the capabilities of apps to run in the background, as a consequence the Update interval set into the app may not be followed by the operating system.\nYou may also have more regular results when the device is charging.")])])])}),[],!1,null,null,null);e.default=i.exports}}]);