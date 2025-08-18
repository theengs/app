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

#include "DeviceManager.h"

#include "devices/device_theengs_gateway.h"

#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>

#include <QDebug>

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceManager::selectGateway_fromwifi(const QString &addr)
{
    qDebug() << "DeviceManager::selectGateway_fromwifi(" << addr << ")";

    QString ble_mac = DeviceGateway::bleMac_from_wifiMac(addr);

    selectGateway(ble_mac);
}

void DeviceManager::selectGateway(const QString &addr)
{
    qDebug() << "DeviceManager::selectGateway(" << addr << ")";

    if (addr.isEmpty())
    {
        deselectGateway();
        return;
    }

    if (m_gateway_selected)
    {
        if (addr == m_gateway_selected->getAddressMAC()) return; // already selected
        if (addr == m_gateway_selected->getAddress()) return; // already selected
    }

    for (auto d: std::as_const(m_gateways_model->m_devices))
    {
        DeviceGateway *dd = qobject_cast<DeviceGateway *>(d);

        if (dd->getAddressMAC() == addr || dd->getAddress() == addr)
        {
            qDebug() << ">>>> selecting gateway [" << addr << "]";

            m_gateway_selected = dd;
            Q_EMIT gatewaySelectedUpdated();
            return;
        }
    }
}

void DeviceManager::deselectGateway()
{
    if (m_gateway_selected)
    {
        m_gateway_selected->deviceDisconnect();
        m_gateway_selected = nullptr;
        Q_EMIT gatewaySelectedUpdated();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */
