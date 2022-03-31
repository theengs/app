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

#ifndef DEVICE_THEENGS_H
#define DEVICE_THEENGS_H
/* ************************************************************************** */

#include "device_sensor.h"
#include "device_utils_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Theengs generic
 */
class DeviceTheengs: public DeviceSensor
{
    Q_OBJECT

    // probe data
    Q_PROPERTY(float temperature1 READ getTemp1 NOTIFY dataUpdated)
    Q_PROPERTY(float temperature2 READ getTemp2 NOTIFY dataUpdated)
    Q_PROPERTY(float temperature3 READ getTemp3 NOTIFY dataUpdated)
    Q_PROPERTY(float temperature4 READ getTemp4 NOTIFY dataUpdated)
    Q_PROPERTY(float temperature5 READ getTemp5 NOTIFY dataUpdated)
    Q_PROPERTY(float temperature6 READ getTemp6 NOTIFY dataUpdated)
    Q_PROPERTY(int pressure1 READ getPressure1 NOTIFY dataUpdated)
    Q_PROPERTY(int pressure2 READ getPressure2 NOTIFY dataUpdated)
    Q_PROPERTY(int pressure3 READ getPressure3 NOTIFY dataUpdated)
    Q_PROPERTY(int pressure4 READ getPressure4 NOTIFY dataUpdated)

protected:
    // probe data
    float m_temperature1 = -99.f;
    float m_temperature2 = -99.f;
    float m_temperature3 = -99.f;
    float m_temperature4 = -99.f;
    float m_temperature5 = -99.f;
    float m_temperature6 = -99.f;
    float m_pressure1 = -99.f;
    float m_pressure2 = -99.f;
    float m_pressure3 = -99.f;
    float m_pressure4 = -99.f;

public:
    DeviceTheengs(QString &deviceAddr, QString &deviceName, QObject *parent = nullptr);
    DeviceTheengs(const QBluetoothDeviceInfo &d, QObject *parent = nullptr);
    ~DeviceTheengs();

    void parseAdvertisementData(const QByteArray &value);

    // probe data
    float getTemp1() const;
    float getTemp1C() const { return m_temperature1; }
    float getTemp1F() const { return (m_temperature1 * 9.f/5.f + 32.f); }
    float getTemp2() const;
    float getTemp2C() const { return m_temperature2; }
    float getTemp2F() const { return (m_temperature2 * 9.f/5.f + 32.f); }
    float getTemp3() const;
    float getTemp3C() const { return m_temperature3; }
    float getTemp3F() const { return (m_temperature3 * 9.f/5.f + 32.f); }
    float getTemp4() const;
    float getTemp4C() const { return m_temperature4; }
    float getTemp4F() const { return (m_temperature4 * 9.f/5.f + 32.f); }
    float getTemp5() const;
    float getTemp5C() const { return m_temperature5; }
    float getTemp5F() const { return (m_temperature5 * 9.f/5.f + 32.f); }
    float getTemp6() const;
    float getTemp6C() const { return m_temperature6; }
    float getTemp6F() const { return (m_temperature6 * 9.f/5.f + 32.f); }
    int getPressure1() const { return m_pressure1; }
    int getPressure2() const { return m_pressure2; }
    int getPressure3() const { return m_pressure3; }
    int getPressure4() const { return m_pressure4; }

private:
    // QLowEnergyController related
    void serviceScanDone();
    void addLowEnergyService(const QBluetoothUuid &uuid);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_H
