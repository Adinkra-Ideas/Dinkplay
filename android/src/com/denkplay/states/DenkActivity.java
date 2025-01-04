package com.denkplay.states;

import org.qtproject.qt.android.bindings.QtActivity;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.content.Intent;
import android.content.Context;
import android.widget.Toast;
import android.media.AudioManager;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;

import android.content.ComponentName;
import android.content.ServiceConnection;
import com.denkplay.states.DenkService;

public class DenkActivity extends QtActivity {
    private Context context;
    private int myState_;

/* For holding the binded service BEGINS */
    private DenkService theService_;
    private boolean serviceBinded_ = false;
/* For holding the binded service ENDS */

/* For holding vars required to manage audio interrupts BEGINS */
    // AudioAttributes describes to android the use case for your audio.
    // The system looks at them when an app gains and loses audio focus.
    private AudioAttributes audioAttributes_ = new AudioAttributes.Builder()
        .setUsage(AudioAttributes.USAGE_MEDIA)
        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
        .build();
    //
    private AudioManager.OnAudioFocusChangeListener audioFocusChangeListener_ = new AudioManager.OnAudioFocusChangeListener() {
        @Override
        public void onAudioFocusChange(int focusChange) {
            Log.d("Rachit", "focuschanged received. value = " + focusChange);
            switch (focusChange) {
                case AudioManager.AUDIOFOCUS_LOSS:
                    // Permanent loss of audio focus. Pause playback indefinitely
                    theService_.controlCPPMediaControl("com.denkplay.states.action.pause");
                    break; // CONTINUE WITH IMPLEMENTING THE SUSPEND AND UNSUSPEND FOR THE REMAINING PART BEFORE MOVING TO HEADSET MANAGEMENT
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
                    // Pause playback
                    Log.d("Rachit", "Lost audio focus temporarily. media must now suspend");
                    break;
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                    // Lower the volume, keep playing
                    Log.d("Rachit", "Lost audio focus temporarily. But the new audio capturer tells us that we can lower our audi rather than pause");
                    break;
                case AudioManager.AUDIOFOCUS_GAIN:
                    // Your app has been re-granted audio focus again.
                    // This is called after the capturer who captured the audio
                    // session with either "AUDIOFOCUS_LOSS_TRANSIENT" or
                    // "AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK" finally releases the
                    // audio session. Now you can unsuspend your play.
                    Log.d("Rachit", "gained audio focus back. media can now unsuspend");
                    break;
            }
        }
    };
    // AudioFocusRequest is used to pass the AudioAttributes we built above to
    // Android, and also tell Android how we intend to seize the audiosession's focus.
    // This info enables android to notify the other app (if any) currently using audio session.
    // A param of "AudioManager.AUDIOFOCUS_GAIN" means we want full audio focus permanently.
    // "AUDIOFOCUS_GAIN_TRANSIENT" means we want full audio focus but for less than 45 seconds.
    // "AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK" but for less than 45 seconds, and in the meantime, the other app currently holding audio session may decide to duck(lower their volume) rather than pause.
    private AudioFocusRequest focusRequest_ = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
        .setAudioAttributes(audioAttributes_)
        .setAcceptsDelayedFocusGain(true)
        .setOnAudioFocusChangeListener(audioFocusChangeListener_)
        .build();
/* For holding vars required to manage audio interrupts ENDS */


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

    ////////////////////////////////////////////////////////////////
    // ******* METHODS FOR STARTING AND BINDING SERVICE BEGINS *****
    ////////////////////////////////////////////////////////////////

    @Override
    public ComponentName startForegroundService(Intent intent) {
        ComponentName componentName = super.startForegroundService(intent);

        // Bind to DenkService AKA the started foreground service
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

    ////////////////////////////////////////////////////////
    // **** METHODS FOR MANAGING AUDIO INTERRUPTS BEGINS ***
    ////////////////////////////////////////////////////////



    // media player is handled according to the
          // change in the focus which Android system grants for
    // audioFocusChangeListener_ = new AudioManager.OnAudioFocusChangeListener() {
    //         @Override
            // public void onAudioFocusChange(int focusChange) {
    //             if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
    //                 Log.d("Rachit", "gained audio focus. media can now play");
    //             } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT) {
    //                 Log.d("Rachit", "Lost audio focus temporarily. media must now suspend");
    //             } else if (focusChange == AudioManager.AUDIOFOCUS_LOSS) {
    //                 Log.d("Rachit", "Lost audio focus Permanently. media must now pause");
    //             }
        //     }
        // };


    //////////////////////////////////////////////////////
    // **** METHODS DIRECTLY CALLED FROM CPP BEGINS ******
    //////////////////////////////////////////////////////

    /*
    * We want to access the method inside service from
    * Cpp. This method is just to enable CPP reach the
    * setPlayPauseIconInService() method inside the
    * binded service instance theService_
    */
    public void setPlayPauseIconInActivity(boolean playing) {
        theService_.setPlayPauseIconInService(playing);
    }

    // Checked before playing. if failed, it returns false
    // to cpp. which will cause the play() in cpp to not exec
    public boolean seizeControlOfAudioManager() {
    /* NOW SINCE WE HAVE BUILT OUR LISTENERS AND STORED THE FINAL TO focusRequest_,
    // WE NOW NEED TO RUN THE BELOW EACH TIME WE NEED TO START .
    // WHAT THE BELOW CODE DOES IN THIS CASE IS THAT IT REQUEST ANDROID's audiosession
    // MANAGER TO GRANT US CONTROL OF AUDIO SESSION AND IT PASSES OUR focusRequest_
    // OBJECT(WHICH CONTAINS A DESCRIPTION OF HOW/WHY WE NEED THE audiosession) TO ANDROID.
    // AND SINCE OUR focusRequest_ OBJECT ALSO CONTAINS THE LISTENER FOR INTERRUPTIONS,
    // ANDROID WILL EXECUTE THAT LISTENER IF AN INTERRUPT COMES IN WHILE WE HAPPEN TO BE
    // IN CONTROL OF audiosession.
    */
        // retrieve the running instance of audiosession manager from android
        AudioManager audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        // request the audio focus from android and store response in the int variable to know android's response
        int result = audioManager.requestAudioFocus(focusRequest_);
        if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
            return true;
        } else if (result == AudioManager.AUDIOFOCUS_REQUEST_FAILED) {
            return false;
        }
        return false;
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



// ALL CALLS TO new NEEDS TO BE RELEASED ON EXIT
