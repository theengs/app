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

#include "device_environmental.h"

#include <QObject>
#include <QString>
#include <QByteArray>

#include <QSqlQuery>
#include <QSqlError>

/* ************************************************************************** */

DeviceEnvironmental::DeviceEnvironmental(const QString &deviceAddr, const QString &deviceName, QObject *parent):
    DeviceSensor(deviceAddr, deviceName, parent)
{
    // Load device infos, bias, limits and initial data
    if (m_dbInternal || m_dbExternal)
    {
        getSqlDeviceInfos();

        //getSqlSensorBias(); // not for Theengs
        //getSqlSensorLimits();

        // Load initial data into the GUI (if they are no more than 12h old)
        getSqlSensorData(12*60);
    }
}

DeviceEnvironmental::DeviceEnvironmental(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceSensor(d, parent)
{
    // Load device infos, bias, limits and initial data
    if (m_dbInternal || m_dbExternal)
    {
        getSqlDeviceInfos();

        //getSqlSensorBias(); // not for Theengs
        //getSqlSensorLimits();

        // Load initial data into the GUI (if they are no more than 12h old)
        getSqlSensorData(12*60);
    }
}

DeviceEnvironmental::~DeviceEnvironmental()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceEnvironmental::updateChartData_environmentalVoc(int maxDays)
{
    if (maxDays <= 0) return;
    int maxMonths = 2;

    qDeleteAll(m_chartData_env);
    m_chartData_env.clear();
    ChartDataVoc *previousdata = nullptr;

    if (m_dbInternal || m_dbExternal)
    {
        QString strftime_mid = "strftime('%Y-%m-%d', timestamp)"; // sqlite
        if (m_dbExternal) strftime_mid = "DATE_FORMAT(timestamp, '%Y-%m-%d')"; // mysql

        QString datetime_months = "datetime('now','-" + QString::number(maxMonths) + " month')"; // sqlite
        if (m_dbExternal) datetime_months = "DATE_SUB(NOW(), INTERVAL -" + QString::number(maxMonths) + " MONTH)"; // mysql

        QSqlQuery graphData;
        graphData.prepare("SELECT " + strftime_mid + "," \
                            "min(voc), avg(voc), max(voc)," \
                            "min(hcho), avg(hcho), max(hcho)," \
                            "min(co2), avg(co2), max(co2) " \
                          "FROM sensorData " \
                          "WHERE deviceAddr = :deviceAddr AND timestamp >= " + datetime_months + " " \
                          "GROUP BY " + strftime_mid + " " \
                          "ORDER BY timestamp DESC "
                          "LIMIT :maxDays;");
        graphData.bindValue(":deviceAddr", getAddress());
        graphData.bindValue(":maxDays", maxDays);

        if (graphData.exec() == false)
        {
            qWarning() << "> graphData.exec() ERROR"
                       << graphData.lastError().type() << ":" << graphData.lastError().text();
            return;
        }

        while (graphData.next())
        {
            if (m_chartData_env.size() < maxDays)
            {
                // missing day(s)?
                if (previousdata)
                {
                    QDateTime datefromsql = graphData.value(0).toDateTime();
                    int diff = datefromsql.daysTo(previousdata->getDateTime());
                    for (int i = diff; i > 1; i--)
                    {
                        if (m_chartData_env.size() < (maxDays-1))
                        {
                            QDateTime fakedate(datefromsql.addDays(i-1));
                            m_chartData_env.push_front(new ChartDataVoc(fakedate, -99, -99, -99, -99, -99, -99, -99, -99, -99, this));
                        }
                    }
                }

                // data
                ChartDataVoc *d = new ChartDataVoc(graphData.value(0).toDateTime(),
                                                   graphData.value(1).toFloat(), graphData.value(2).toFloat(), graphData.value(3).toFloat(),
                                                   graphData.value(4).toFloat(), graphData.value(5).toFloat(), graphData.value(6).toFloat(),
                                                   graphData.value(7).toFloat(), graphData.value(8).toFloat(), graphData.value(9).toFloat(),
                                                   this);
                m_chartData_env.push_front(d);
                previousdata = d;
            }
        }

        // missing day(s)?
        {
            // after
            QDateTime today = QDateTime::currentDateTime();
            int missing = maxDays;
            if (previousdata) missing = static_cast<ChartDataVoc *>(m_chartData_env.last())->getDateTime().daysTo(today);
            for (int i = missing - 1; i >= 0; i--)
            {
                QDateTime fakedate(today.addDays(-i));
                m_chartData_env.push_back(new ChartDataVoc(fakedate, -99, -99, -99, -99, -99, -99, -99, -99, -99, this));
            }

            // before
            today = QDateTime::currentDateTime();
            for (int i = m_chartData_env.size(); i < maxDays; i++)
            {
                QDateTime fakedate(today.addDays(-i));
                m_chartData_env.push_front(new ChartDataVoc(fakedate, -99, -99, -99, -99, -99, -99, -99, -99, -99, this));
            }
        }
/*
        // first vs last (for months less than 31 days long)
        if (m_chartData_env.size() > 1)
        {
            while (!m_chartData_env.isEmpty() &&
                   static_cast<ChartDataVoc *>(m_chartData_env.first())->getDay() ==
                   static_cast<ChartDataVoc *>(m_chartData_env.last())->getDay())
            {
                m_chartData_env.pop_front();
            }
        }
*/
        Q_EMIT chartDataEnvUpdated();
    }
}

/* ************************************************************************** */
