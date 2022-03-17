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

package com.theengs.app;

import java.lang.String;
import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;

import org.qtproject.qt.android.bindings.QtService;

public class TheengsAndroidService extends QtService {

    private static final String TAG = "TheengsAndroidService";

    @Override
    public void onCreate() {
        super.onCreate();
    }

    public void onResume() {
        //
    }

    public void onPause() {
        //
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int ret = super.onStartCommand(intent, flags, startId);

        return START_STICKY;
    }

    ////////////////////////////////////////////////////////////////////////////

    public static void serviceStart(android.content.Context context) {
        android.content.Intent pQtAndroidService = new android.content.Intent(context, TheengsAndroidService.class);
        pQtAndroidService.addFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startService(pQtAndroidService);
    }

    public static void serviceStop(android.content.Context context) {
        android.content.Intent pQtAndroidService = new android.content.Intent(context, TheengsAndroidService.class);
        context.stopService(pQtAndroidService);
    }
}
