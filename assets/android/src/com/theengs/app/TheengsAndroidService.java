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
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.os.Build;
import android.util.Log;

import org.qtproject.qt.android.bindings.QtService;

public class TheengsAndroidService extends QtService {

    private static final String TAG = "TheengsAndroidService";
    // Foreground-service notification id + channel. Fixed per-process so
    // startForeground/stopForeground refer to the same notification.
    private static final int NOTIFICATION_ID = 1001;
    private static final String CHANNEL_ID = "TheengsForegroundService";

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
        // Clear the foreground notification on stopService() so it doesn't
        // linger in the tray after the user toggles off background scanning.
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                stopForeground(STOP_FOREGROUND_REMOVE);
            } else {
                stopForeground(true);
            }
        } catch (Exception e) {
            Log.e(TAG, "stopForeground failed", e);
        }
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // Promote to foreground BEFORE delegating to QtService. Android
        // ANRs / kills the service if startForeground() isn't called
        // within 5 s of startForegroundService(). Typed as
        // FOREGROUND_SERVICE_TYPE_CONNECTED_DEVICE because the service
        // exists to keep BLE scan callbacks alive while the screen is off.
        startInForeground();
        int ret = super.onStartCommand(intent, flags, startId);
        return START_STICKY;
    }

    private void startInForeground() {
        try {
            Notification notification = buildNotification();
            if (Build.VERSION.SDK_INT >= 34) {
                startForeground(
                    NOTIFICATION_ID,
                    notification,
                    ServiceInfo.FOREGROUND_SERVICE_TYPE_CONNECTED_DEVICE
                );
            } else {
                startForeground(NOTIFICATION_ID, notification);
            }
        } catch (Exception e) {
            Log.e(TAG, "startForeground failed", e);
        }
    }

    private Notification buildNotification() {
        NotificationManager nm =
            (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel ch = new NotificationChannel(
                CHANNEL_ID,
                "Theengs background service",
                NotificationManager.IMPORTANCE_LOW
            );
            ch.setDescription("Keeps the BLE scan running while the app is in the background");
            nm.createNotificationChannel(ch);
        }
        Intent launch =
            getPackageManager().getLaunchIntentForPackage(getPackageName());
        // FLAG_ACTIVITY_CLEAR_TOP + FLAG_ACTIVITY_SINGLE_TOP: bring the existing
        // QtActivity instance to the foreground (delivering onNewIntent) instead
        // of letting Android spin up a fresh ActivityRecord every time the user
        // taps the notification. Combined with launchMode="singleTask" on the
        // activity, this prevents the "black screen + activity pause/destroy
        // timeout" failure mode where rapid relaunches on a slow device
        // (observed on LG V30 / Android 9) deadlock the Qt main thread and
        // SurfaceFlinger never receives a BufferLayer for the new task.
        launch.addFlags(
            Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP
        );
        PendingIntent pi = PendingIntent.getActivity(
            this, 0, launch,
            PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE
        );
        Notification.Builder b;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            b = new Notification.Builder(this, CHANNEL_ID);
        } else {
            b = new Notification.Builder(this);
        }
        b.setSmallIcon(R.drawable.ic_stat_logo)
         .setContentTitle("Theengs")
         .setContentText("Scanning for sensors")
         .setContentIntent(pi)
         .setOngoing(true)
         .setOnlyAlertOnce(true);
        return b.build();
    }

    ////////////////////////////////////////////////////////////////////////////

    public static void serviceStart(android.content.Context context) {
        android.content.Intent pQtAndroidService = new android.content.Intent(context, TheengsAndroidService.class);
        pQtAndroidService.addFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK);
        // startForegroundService() is required since API 26 for the manifest's
        // foregroundServiceType to take effect. onStartCommand() above promotes
        // the service to foreground within the 5 s window Android allows.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(pQtAndroidService);
        } else {
            context.startService(pQtAndroidService);
        }
    }

    public static void serviceStop(android.content.Context context) {
        android.content.Intent pQtAndroidService = new android.content.Intent(context, TheengsAndroidService.class);
        context.stopService(pQtAndroidService);
    }
}
