package com.denkplay.states;

import org.qtproject.qt.android.bindings.QtActivity;
import android.os.Bundle;
import android.util.Log;
import android.content.Intent;
import android.content.Context;
import android.app.PendingIntent;
import androidx.core.app.ActivityCompat;
import android.Manifest;
import android.content.pm.PackageManager;

import android.widget.Toast;

import android.R;
import android.os.Build;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import androidx.core.app.NotificationCompat;
import android.content.ComponentName;

public class DenkActivity extends QtActivity {
    private Context context;
    private int myState_;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = this;
        myState_ = 0;
        Log.d("Rachitttt", "In Stop Method");
    }

    @Override
    protected void onStart() {
        super.onStart();
        myState_ = 1;
    }

    @Override
    public void onResume(){
        super.onResume();
        myState_ = 2;
    }

    @Override
    protected void onPause() {
        super.onPause();
        myState_ = 3;
        // moveTaskToBack (true);
    }

    @Override
    protected void onStop() {
        super.onStop();
        myState_ = 4;
        // Log.d("Rachit", "In Stop Method");
    }


    // PendingIntent.getForegroundService() Just calls the Service's onStartCommand() directly.
    // Maybe its because of the flags we're using.
    // I tested this by overriding the startForegroundService() and startService() implementation
    // from main activity. The lines I printed there only gets printed when the service is started
    // from C++. Never gets printed when the PendingIntent.getService / getForegroundService calls


    // **********************************************

    /**
      * Called from frontend to decide what
      * should happen when user swipes up to
      * kill the app from minimized view, or
      * clicks back button to kill the app.
      * Both sends same onClose signal from
      * QML part of Qt.
      * @returns void
      */
    public void myBackPressManager() {
        if (myState_ == 2) {
            moveTaskToBack (true);
            super.onPause();
        } else {
            finishAffinity();
            System.exit(0);
        }
    }


}
