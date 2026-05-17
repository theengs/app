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

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ServiceInfo;
import android.os.Build;
import android.util.Log;

import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;

import org.qtproject.qt.android.bindings.QtService;

public class TheengsAndroidService extends QtService {

    private static final String TAG = "TheengsAndroidService";
    // Foreground-service notification id + channel. Fixed per-process so
    // startForeground/stopForeground/notify all target the same notification.
    private static final int NOTIFICATION_ID = 1001;
    private static final String CHANNEL_ID = "TheengsForegroundService";
    // Package-scoped action so the Stop button only ever wakes our own
    // receiver. Registered dynamically in onCreate() (RECEIVER_NOT_EXPORTED
    // on API 33+) so we don't need a manifest <receiver> entry.
    private static final String ACTION_STOP_FGS = "com.theengs.app.action.STOP_FGS";

    // Live notification content fed from C++ via updateNotification().
    // volatile because the static setter is called from the Qt main thread of
    // the service process while buildNotification() runs on this service's
    // main thread.
    private static volatile String sTitle;
    private static volatile String sBody;
    private static volatile String sBigText;
    private static volatile TheengsAndroidService sInstance;

    private final BroadcastReceiver mStopReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "Stop action received — stopping service");
            stopSelf();
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        sInstance = this;
        IntentFilter filter = new IntentFilter(ACTION_STOP_FGS);
        try {
            // RECEIVER_NOT_EXPORTED is required on API 34+ for unprotected
            // dynamically-registered receivers; harmless flag on 33.
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                registerReceiver(mStopReceiver, filter, Context.RECEIVER_NOT_EXPORTED);
            } else {
                registerReceiver(mStopReceiver, filter);
            }
        } catch (Exception e) {
            Log.e(TAG, "registerReceiver failed", e);
        }
    }

    public void onResume() {
        //
    }

    public void onPause() {
        //
    }

    @Override
    public void onDestroy() {
        sInstance = null;
        try {
            unregisterReceiver(mStopReceiver);
        } catch (Exception e) {
            // Swallow: receiver may not have registered if onCreate threw.
        }
        // Clear the foreground notification on stopService() / stopSelf() so
        // it doesn't linger in the tray after the user toggles off background
        // scanning or presses the Stop action.
        try {
            ServiceCompat.stopForeground(this, ServiceCompat.STOP_FOREGROUND_REMOVE);
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
        super.onStartCommand(intent, flags, startId);
        return START_STICKY;
    }

    private void startInForeground() {
        try {
            ensureChannel();
            Notification n = buildNotification();
            // androidx.core 1.6.1 ships ServiceCompat.startForeground only in
            // its 3-arg form; the typed 4-arg overload landed in 1.12.0. We're
            // pinned to 1.6.1, so branch manually on API 34 instead of bumping
            // the dependency (the policy comment above still applies).
            if (Build.VERSION.SDK_INT >= 34) {
                startForeground(NOTIFICATION_ID, n,
                    ServiceInfo.FOREGROUND_SERVICE_TYPE_CONNECTED_DEVICE);
            } else {
                startForeground(NOTIFICATION_ID, n);
            }
        } catch (Exception e) {
            Log.e(TAG, "startForeground failed", e);
        }
    }

    private void ensureChannel() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) return;
        NotificationManager nm =
            (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (nm == null) return;
        NotificationChannel ch = new NotificationChannel(
            CHANNEL_ID,
            getString(R.string.fgs_channel_name),
            NotificationManager.IMPORTANCE_LOW
        );
        ch.setDescription(getString(R.string.fgs_channel_description));
        ch.setShowBadge(false);
        nm.createNotificationChannel(ch);
    }

    private PendingIntent makeContentIntent() {
        // FLAG_ACTIVITY_CLEAR_TOP + FLAG_ACTIVITY_SINGLE_TOP: bring the existing
        // QtActivity instance to the foreground (delivering onNewIntent) instead
        // of letting Android spin up a fresh ActivityRecord every time the user
        // taps the notification. Combined with launchMode="singleTask" on the
        // activity, this prevents the "black screen + activity pause/destroy
        // timeout" failure mode where rapid relaunches on a slow device
        // (observed on LG V30 / Android 9) deadlock the Qt main thread and
        // SurfaceFlinger never receives a BufferLayer for the new task.
        Intent launch = getPackageManager().getLaunchIntentForPackage(getPackageName());
        if (launch == null) {
            launch = new Intent(Intent.ACTION_MAIN).setPackage(getPackageName());
        }
        launch.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        return PendingIntent.getActivity(
            this, 0, launch,
            PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE
        );
    }

    private PendingIntent makeStopIntent() {
        Intent stop = new Intent(ACTION_STOP_FGS).setPackage(getPackageName());
        return PendingIntent.getBroadcast(
            this, 0, stop,
            PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE
        );
    }

    private Notification buildNotification() {
        String title = sTitle != null ? sTitle : getString(R.string.fgs_title_default);
        String body  = sBody  != null ? sBody  : getString(R.string.fgs_body_default);
        String big   = sBigText != null && !sBigText.isEmpty() ? sBigText : body;

        NotificationCompat.Builder b = new NotificationCompat.Builder(this, CHANNEL_ID)
            .setSmallIcon(R.drawable.ic_stat_logo)
            .setContentTitle(title)
            .setContentText(body)
            .setStyle(new NotificationCompat.BigTextStyle().bigText(big))
            .setContentIntent(makeContentIntent())
            .setOngoing(true)
            .setOnlyAlertOnce(true)
            .setShowWhen(false)
            .setCategory(NotificationCompat.CATEGORY_SERVICE)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .setForegroundServiceBehavior(NotificationCompat.FOREGROUND_SERVICE_IMMEDIATE)
            .addAction(0, getString(R.string.fgs_action_stop), makeStopIntent());
        return b.build();
    }

    /**
     * JNI entry point called from C++ ForegroundNotifier on the service
     * process's Qt main thread. Re-renders the FGS notification with the
     * supplied content. No-op if the service has been torn down between
     * the C++ side queuing an update and us reaching this method.
     */
    public static void updateNotification(String title, String body, String bigText) {
        sTitle = title;
        sBody = body;
        sBigText = bigText;
        TheengsAndroidService self = sInstance;
        if (self == null) return;
        try {
            NotificationManager nm =
                (NotificationManager) self.getSystemService(Context.NOTIFICATION_SERVICE);
            if (nm != null) nm.notify(NOTIFICATION_ID, self.buildNotification());
        } catch (Exception e) {
            Log.e(TAG, "updateNotification failed", e);
        }
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
