package com.denkplay.states;

import org.qtproject.qt.android.bindings.QtActivity;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.content.Intent;
import android.content.Context;
import android.widget.Toast;

import android.content.ComponentName;
import android.content.ServiceConnection;
import com.denkplay.states.DenkService;

public class DenkActivity extends QtActivity {
    private Context context;
    private int myState_;

    /* For holding the binded service */
    private DenkService theService_;
    private boolean serviceBinded_ = false;
    /* For holding the binded service */

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = this;
        myState_ = 0;

        Toast.makeText(this, "Starting Dinkplay..", Toast.LENGTH_SHORT).show();
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
        // Log.d("Rachit", Boolean.toString(serviceBinded_));
    }

    @Override
    public void onDestroy() {
        unbindService(connection);
        serviceBinded_ = false;

        // This is a clean way to quickly exit the app onDestroy.
        // if you depend on super.onDestroy, exiting can take over
        // 20 seconds after app closes. Also, startedServices wont
        // get their ondestroy called at all, orphaning them in the
        // process. an
        // Worse is if you attempt to relaunch the app, it will hang
        // and crash.
        // The only way to avoid all these problem is to cleanly exit
        // using the below two calls.
        finishAffinity();
        System.exit(0);

        super.onDestroy();
    }

    @Override
    public ComponentName startForegroundService(Intent intent) {
        ComponentName componentName = super.startForegroundService(intent);

        // Bind to DenkService.
        bindService(intent, connection, 0);

        return componentName;
    }

    /** connection is simply 2 callbacks. one used by bindService(), the other used by unbindService() */
    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to DenkService, cast the IBinder and get DenkService instance.
            DenkService.MyBinder binder = (DenkService.MyBinder) service;
            theService_ = binder.getService();
            serviceBinded_ = true;

        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            serviceBinded_ = false;
        }
    };

    /*
    * We want to access the method inside service from
    * Cpp. This method is just to enable CPP reach the
    * setPlayPauseIconInService() method inside the
    * binded service instance theService_
    */
    public void setPlayPauseIconInActivity(boolean playing) {
        theService_.setPlayPauseIconInService(playing);
    }

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
            // Instant and clean exit. All running services onDestroy()
            // will be called as well to ensure startedServices also have
            // the opportunity to close themselves gracefully.
            // finishAffinity() kills app and frees memory.
            // System.exit(0); forces android to take back freed memory.
            finishAffinity();
            System.exit(0);
        }
    }

}
