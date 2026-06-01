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

import android.Manifest;
import android.app.Activity;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.ServiceInfo;
import android.net.Uri;
import android.os.Build;
import android.os.PowerManager;
import android.util.Log;

import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;
import androidx.core.content.ContextCompat;

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
    // Background-work alarm: an (exact) AllowWhileIdle AlarmManager alarm drives
    // gotowork() so the background refresh interval is honoured through deep
    // doze, where the old C++ QTimer (a non-wakeup timerfd) was deferred to doze
    // maintenance windows (device-measured ~1-3 h gaps).
    private static final String ACTION_WORK = "com.theengs.app.action.WORK";
    private static final int REQUEST_CODE_WORK = 1102;

    // Request code for the POST_NOTIFICATIONS runtime-permission dialog
    // (Android 13+). We don't observe the result here — the foreground
    // service simply starts posting once Android records the grant — but
    // the value must be stable so a redelivered onRequestPermissionsResult
    // wouldn't be mistaken for some other in-flight request.
    private static final int REQUEST_CODE_POST_NOTIFICATIONS = 1101;

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

    // Fired by the AllowWhileIdle alarm. Hands control to C++ (gotowork()),
    // which does the scan + reschedules the next alarm via scheduleWork().
    private final BroadcastReceiver mWorkReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "Work alarm fired");
            // Hold a timed partial wakelock so the CPU stays awake through the
            // ~60s background scan window that gotowork() kicks off; otherwise
            // the broadcast wakelock is released the moment onReceive returns
            // and the device re-suspends mid-scan (nothing gets decoded).
            // Auto-releases at the timeout, so no explicit release needed.
            try {
                PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
                if (pm != null) {
                    PowerManager.WakeLock wl = pm.newWakeLock(
                        PowerManager.PARTIAL_WAKE_LOCK, "theengs:work");
                    wl.setReferenceCounted(false);
                    wl.acquire(65_000);
                }
            } catch (Exception e) {
                Log.e(TAG, "work wakelock failed", e);
            }
            try {
                nativeOnWorkAlarm();
            } catch (Throwable t) {
                Log.e(TAG, "nativeOnWorkAlarm failed", t);
            }
        }
    };

    // Implemented in C++ (AndroidService), registered via
    // QJniEnvironment::registerNativeMethods. Posts gotowork() onto the Qt
    // service thread.
    private static native void nativeOnWorkAlarm();

    @Override
    public void onCreate() {
        super.onCreate();
        sInstance = this;
        try {
            // RECEIVER_NOT_EXPORTED is required on API 34+ for unprotected
            // dynamically-registered receivers; harmless flag on 33.
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                registerReceiver(mStopReceiver, new IntentFilter(ACTION_STOP_FGS), Context.RECEIVER_NOT_EXPORTED);
                registerReceiver(mWorkReceiver, new IntentFilter(ACTION_WORK), Context.RECEIVER_NOT_EXPORTED);
            } else {
                registerReceiver(mStopReceiver, new IntentFilter(ACTION_STOP_FGS));
                registerReceiver(mWorkReceiver, new IntentFilter(ACTION_WORK));
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
        cancelWork();
        try {
            unregisterReceiver(mStopReceiver);
            unregisterReceiver(mWorkReceiver);
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

    /**
     * True when POST_NOTIFICATIONS has been granted (or the OS doesn't gate
     * it — pre-Android-13 / API &lt; 33 — in which case the manifest
     * declaration is sufficient and this always returns true).
     *
     * Called from C++ (PermissionManager) via JNI auto-linking.
     */
    public static boolean isPostNotificationsGranted(Context ctx) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) {
            return true;
        }
        return ContextCompat.checkSelfPermission(ctx,
            Manifest.permission.POST_NOTIFICATIONS) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Trigger the system runtime-permission dialog for POST_NOTIFICATIONS.
     * No-op on Android &lt; 13 (API 33), where the manifest declaration is
     * sufficient; also no-op when the permission is already granted.
     *
     * The dialog is asynchronous and we don't observe the result — the
     * foreground service starts posting the moment Android records the
     * grant. Callers that need the result should re-check via
     * {@link #isPostNotificationsGranted(Context)} after the Activity
     * resumes.
     *
     * Must be invoked with an Activity context;
     * {@link ActivityCompat#requestPermissions} requires one.
     */
    public static void requestPostNotifications(Activity activity) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) return;
        if (isPostNotificationsGranted(activity)) return;
        ActivityCompat.requestPermissions(activity,
            new String[]{Manifest.permission.POST_NOTIFICATIONS},
            REQUEST_CODE_POST_NOTIFICATIONS);
    }

    /**
     * True when the app may schedule exact alarms. Always true below API 31
     * (exact alarms are unrestricted there); on 31+ reflects
     * AlarmManager.canScheduleExactAlarms() — auto-granted on 31-33, denied by
     * default on 34+. Called from C++ (PermissionManager) via JNI.
     */
    public static boolean canScheduleExactAlarms(Context ctx) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S) return true;
        AlarmManager am = (AlarmManager) ctx.getSystemService(Context.ALARM_SERVICE);
        return am != null && am.canScheduleExactAlarms();
    }

    /**
     * Open Settings → Alarms & reminders for this app so the user can grant
     * SCHEDULE_EXACT_ALARM (needed for precise interval timing on API 34+).
     * No-op below API 31 or when already granted. Asynchronous — re-check via
     * {@link #canScheduleExactAlarms(Context)} when the activity resumes.
     */
    public static void requestScheduleExactAlarms(Context ctx) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S) return;
        if (canScheduleExactAlarms(ctx)) return;
        try {
            // Literal value of Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM
            // (API 31) — used directly so it compiles regardless of the build's
            // android.jar; gated by SDK_INT >= S above so it only runs on 31+.
            Intent i = new Intent("android.settings.REQUEST_SCHEDULE_EXACT_ALARM")
                .setData(Uri.parse("package:" + ctx.getPackageName()))
                .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            ctx.startActivity(i);
        } catch (Exception e) {
            Log.e(TAG, "requestScheduleExactAlarms failed", e);
        }
    }

    /**
     * True when the app is exempt from battery optimizations, so Doze /
     * app-standby won't defer its background-work alarms. Always true below
     * API 23. Called from C++ (PermissionManager) via JNI.
     */
    public static boolean isIgnoringBatteryOptimizations(Context ctx) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return true;
        PowerManager pm = (PowerManager) ctx.getSystemService(Context.POWER_SERVICE);
        return pm != null && pm.isIgnoringBatteryOptimizations(ctx.getPackageName());
    }

    /**
     * Show the system "ignore battery optimizations" dialog for this app.
     * No-op below API 23 or when already exempt. Requires the
     * REQUEST_IGNORE_BATTERY_OPTIMIZATIONS manifest permission. Asynchronous —
     * re-check via {@link #isIgnoringBatteryOptimizations(Context)} on resume.
     */
    public static void requestIgnoreBatteryOptimizations(Context ctx) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return;
        if (isIgnoringBatteryOptimizations(ctx)) return;
        try {
            // Literal value of Settings.ACTION_REQUEST_IGNORE_BATTERY_OPTIMIZATIONS.
            Intent i = new Intent("android.settings.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS")
                .setData(Uri.parse("package:" + ctx.getPackageName()))
                .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            ctx.startActivity(i);
        } catch (Exception e) {
            Log.e(TAG, "requestIgnoreBatteryOptimizations failed", e);
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    private static PendingIntent workPendingIntent(Context ctx) {
        Intent i = new Intent(ACTION_WORK).setPackage(ctx.getPackageName());
        return PendingIntent.getBroadcast(
            ctx, REQUEST_CODE_WORK, i,
            PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
    }

    /**
     * Schedule the next background-work tick via an AllowWhileIdle alarm.
     * Called from C++ (AndroidService::scheduleNextWork) at startup and at the
     * end of every gotowork().
     *
     * Prefers {@code setExactAndAllowWhileIdle}: it fires at the requested time
     * even in deep doze, so the user's "Update interval" is honoured precisely.
     * That needs SCHEDULE_EXACT_ALARM — auto-granted on API 31-32, user-grantable
     * on 33+. When it isn't granted (canScheduleExactAlarms() == false, or a
     * race throws SecurityException) we degrade to {@code setAndAllowWhileIdle}:
     * still fires through doze, but Android batches it so the effective interval
     * runs longer (measured ~+50%). Either way the alarm pierces doze, unlike a
     * non-wakeup timer which is deferred to maintenance windows.
     */
    public static void scheduleWork(int delayMillis) {
        TheengsAndroidService self = sInstance;
        if (self == null) return;
        AlarmManager am = (AlarmManager) self.getSystemService(Context.ALARM_SERVICE);
        if (am == null) return;
        long at = System.currentTimeMillis() + Math.max(0, delayMillis);
        PendingIntent pi = workPendingIntent(self);
        // canScheduleExactAlarms() is API 31+; below that exact alarms are
        // always permitted (minSdk is 23, so setExactAndAllowWhileIdle exists).
        boolean canExact = Build.VERSION.SDK_INT < Build.VERSION_CODES.S
                           || am.canScheduleExactAlarms();
        try {
            if (canExact) {
                am.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, at, pi);
                Log.i(TAG, "scheduled EXACT work alarm in " + delayMillis + "ms");
            } else {
                am.setAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, at, pi);
                Log.i(TAG, "scheduled inexact work alarm in " + delayMillis
                        + "ms (SCHEDULE_EXACT_ALARM not granted)");
            }
        } catch (SecurityException se) {
            // Exact permission revoked between the check and the call.
            try {
                am.setAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, at, pi);
                Log.w(TAG, "exact alarm denied, fell back to inexact", se);
            } catch (Exception e) {
                Log.e(TAG, "scheduleWork fallback failed", e);
            }
        } catch (Exception e) {
            Log.e(TAG, "scheduleWork failed", e);
        }
    }

    public static void cancelWork() {
        TheengsAndroidService self = sInstance;
        if (self == null) return;
        AlarmManager am = (AlarmManager) self.getSystemService(Context.ALARM_SERVICE);
        if (am != null) {
            try { am.cancel(workPendingIntent(self)); } catch (Exception e) { /* ignore */ }
        }
    }

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
