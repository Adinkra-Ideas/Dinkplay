package com.denkplay.states;

import android.content.Context;
import android.content.Intent;
import android.app.PendingIntent;
import android.util.Log;
import android.os.IBinder;
import android.os.Bundle;
import android.widget.Toast;
import android.app.ActivityManager;
import android.content.ServiceConnection;
import android.content.ComponentName;

import android.R;
import android.os.Build;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import androidx.core.app.NotificationCompat;

import org.qtproject.qt.android.bindings.QtService;

public class DenkService extends QtService
{
    private static native void sendToQt(String message);

    private Context context;
    private int ret_;

    DenkService denkService;

    @Override
    public void onCreate() {
        super.onCreate();
        context = this;

        // Create the notif channel for our Notification way ahead of time
        final String ChannelID = "DinkPlay";
        NotificationChannel channel = new NotificationChannel(
                ChannelID,
                ChannelID,
                NotificationManager.IMPORTANCE_LOW
            );
        getSystemService(NotificationManager.class)
            .createNotificationChannel(channel);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        // Turn off the background service
        stopSelf();
    }


     // we will move the lines to a declarative function
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // Toast.makeText(context, "Starting Dinkplay..", Toast.LENGTH_SHORT).show();

        // Filter the intent and inform CPP if it was
        // a click from prev, play or pause
        String actionRcvd = intent.getAction();
        if (actionRcvd != null) {
            if (actionRcvd.equals("com.denkplay.states.action.prev")) {
                sendToQt("((((Prev))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.play")) {
                sendToQt("((((Play))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.next")) {
                sendToQt("((((Next))))");
            }
        }


        // Call the Activity AKA ThisApp when this foreground service is clicked
        // PendingIntent.getActivity simply pulls the activity AKA App into the
        // front of phone
        Intent activityIntent = new Intent(this, DenkActivity.class);
        PendingIntent pendingActIntent = PendingIntent.getActivity(context, 0, activityIntent, PendingIntent.FLAG_IMMUTABLE);

        // PendingIntent.getService Calls the Service.onStartCommand directly.

        // When the user
        Intent previousIntent = new Intent(this, DenkService.class);
        previousIntent.setAction("com.denkplay.states.action.prev");
        PendingIntent ppreviousIntent = PendingIntent.getForegroundService(this, 0, previousIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        Intent playIntent = new Intent(this, DenkService.class);
        playIntent.setAction("com.denkplay.states.action.play");
        PendingIntent pplayIntent = PendingIntent.getForegroundService(this, 0, playIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        Intent nextIntent = new Intent(this, DenkService.class);
        nextIntent.setAction("com.denkplay.states.action.next");
        PendingIntent pnextIntent = PendingIntent.getForegroundService(this, 0, nextIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        // Build the notification
        final String ChannelID = "DinkPlay";
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, ChannelID)
                .setSmallIcon(getApplicationInfo().icon)
                .setContentTitle("")
                .setContentText("")
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                .setContentIntent(pendingActIntent) // when I removed this, the app stopped dying on multi clicks
                .setOngoing(true)
                .addAction(new NotificationCompat.Action(R.drawable.ic_media_previous, "Previous", ppreviousIntent))
                .addAction(new NotificationCompat.Action(R.drawable.ic_media_play, "Toggle", pplayIntent))
                .addAction(new NotificationCompat.Action(R.drawable.ic_media_next, "Next", pnextIntent))
                ;

            startForeground(1002, builder.build(), 2); // 2 == FOREGROUND_SERVICE_TYPE_MEDIA_PLAYBACK

            int ret = super.onStartCommand(intent, flags, startId);
            return ret;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }


    // private boolean foregroundServiceIsRunning() {
    //     ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
    //     for (ActivityManager.RunningServiceInfo rService: activityManager.getRunningServices(Integer.MAX_VALUE)) {
    //         if (DenkService.class.getName().equals(rService.service.getClassName())) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

}


// LOGIC
// bool playPause = false; // everything is paused by default ('play' will be shown on screen)
// all of the other buttons besides pause button, that gets clicked sets it to true. ('pause' will be shown while true)
// only pause button sets it to false ('play' will be shown on screen)
