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

#ifndef DEVICE_THEENGS_SCALE_H
#define DEVICE_THEENGS_SCALE_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Xiaomi scales:
 * - XMTZC01HM/XMTZC04HM
 * - XMTZC02HM/XMTZC05HM
 */
class DeviceTheengsScales: public DeviceTheengs
{
    Q_OBJECT

    Q_PROPERTY(float weightMin READ getWeightMin NOTIFY minmaxUpdated)
    Q_PROPERTY(float weightMax READ getWeightMax NOTIFY minmaxUpdated)
    Q_PROPERTY(int impedanceMin READ getImpedanceMin NOTIFY minmaxUpdated)
    Q_PROPERTY(int impedanceMax READ getImpedanceMax NOTIFY minmaxUpdated)

    int m_weightMin = 999999;
    int m_weightMax = -99;
    int m_impedanceMin = 999999;
    int m_impedanceMax = -99;

    int getWeightMin() const { return m_weightMin; }
    int getWeightMax() const { return m_weightMax; }
    int getImpedanceMin() const { return m_impedanceMin; }
    int getImpedanceMax() const { return m_impedanceMax; }

public:
    DeviceTheengsScales(const QString &deviceAddr, const QString &deviceName,
                        const QString &deviceModel, const QString &devicePropsJson,
                        QObject *parent = nullptr);
    DeviceTheengsScales(const QBluetoothDeviceInfo &d,
                        const QString &deviceModel, const QString &devicePropsJson,
                        QObject *parent = nullptr);
    ~DeviceTheengsScales();

    // theengs decoder
    void parseTheengsProps(const QString &json);
    void parseTheengsAdvertisement(const QString &json);

    // Chart scale AIO
    Q_INVOKABLE void getChartData_scaleAIO(int maxDays, QDateTimeAxis *axis,
                                           QLineSeries *weight, QLineSeries *impedance);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_SCALE_H
