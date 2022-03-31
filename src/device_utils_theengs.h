/*
    Theengs - Decode things and devices
    Copyright: (c) Florian ROBERT

    This file is part of Theengs.

    Theengs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Theengs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEVICE_UTILS_THEENGS_H
#define DEVICE_UTILS_THEENGS_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QQmlContext>
#include <QQmlApplicationEngine>

/* ************************************************************************** */

class DeviceUtilsTheengs: public QObject
{
    Q_OBJECT

public:
    static void registerQML()
    {
        qRegisterMetaType<DeviceUtilsTheengs::DeviceBeacons>("DeviceUtilsTheengs::DeviceBeacons");
        qRegisterMetaType<DeviceUtilsTheengs::DeviceScales>("DeviceUtilsTheengs::DeviceScales");
        qRegisterMetaType<DeviceUtilsTheengs::DeviceSensorsAdditionals>("DeviceUtilsTheengs::DeviceSensorsAdditionals");

        qmlRegisterType<DeviceUtilsTheengs>("DeviceUtilsTheengs", 1, 0, "DeviceUtilsTheengs");
    }

    enum DeviceBeacons {

        SENSOR_ACCL_X               = (1 <<  0), //!< Accelerometer
        SENSOR_ACCL_Y               = (1 <<  1),
        SENSOR_ACCL_Z               = (1 <<  2),

        SENSOR_GYRO_X               = (1 <<  3), //!< Gyroscope
        SENSOR_GYRO_Y               = (1 <<  4),
        SENSOR_GYRO_Z               = (1 <<  5),

        SENSOR_STEPS                = (1 <<  6), //!< Pedometer
    };

    enum DeviceScales {

        SENSOR_WEIGHT_UNIT          = (1 << 0),
        SENSOR_WEIGHT_MODE          = (1 << 1),
        SENSOR_WEIGHT               = (1 << 2),

        SENSOR_IMPEDANCE            = (1 << 15),
    };

    enum DeviceSensorsAdditionals {

        // environmental data (additionals)
        //SENSOR_TEMPERATURE_1        = (1 <<  0), //!< Additional temperature sensors
        SENSOR_TEMPERATURE_2        = (1 <<  0), //!< Additional temperature sensors
        SENSOR_TEMPERATURE_3        = (1 <<  1),
        SENSOR_TEMPERATURE_4        = (1 <<  2),
        SENSOR_TEMPERATURE_5        = (1 <<  3),
        SENSOR_TEMPERATURE_6        = (1 <<  4),

        //SENSOR_PRESSURE_1           = (1 <<  5),
        SENSOR_PRESSURE_2           = (1 <<  5),
        SENSOR_PRESSURE_3           = (1 <<  6),
        SENSOR_PRESSURE_4           = (1 <<  7),

        // (bool sensors)
        SENSOR_PRESENCE             = (1 << 16),
        SENSOR_OPEN                 = (1 << 17),
        SENSOR_MOVEMENT             = (1 << 18),
        SENSOR_ALARM                = (1 << 19),
    };
    Q_ENUM(DeviceSensorsAdditionals)
};

/* ************************************************************************** */
#endif // DEVICE_UTILS_THEENGS_H
