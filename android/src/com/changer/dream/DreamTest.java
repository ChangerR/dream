/*
 *
 */
package com.changer.dream;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import java.io.IOException;
import com.changer.dream.DreamGLView;
/** */
public class DreamTest extends Activity {

    private DreamGLView mGLView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        try {
            Utils.unpackOnSdCard(getAssets());
        } catch (IOException e) {
            Log.i("Dream", "Error in unpack");
        }
        nativeEnvJ2C(Environment.getExternalStorageDirectory().getAbsolutePath()+"/dream/");
        mGLView = new DreamGLView(this);
        setContentView(mGLView);
        nativeOnCreate();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLView.onPause();
        nativeOnPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
        nativeOnResume();
    }

    @Override    
    public void onDestroy() {
        nativeOnDestroy();
        super.onDestroy();
    }

    /** load Dream.so */
    static {
        System.loadLibrary("dream");
    }

    public native void nativeOnCreate();
    public native void nativeOnPause();
    public native void nativeOnResume();
    public native void nativeOnDestroy();

    public native void nativeInitGL();
    public native void nativeResize(int w, int h);

    public native void nativeGetStatus(DreamStatus status);
    public native void nativeSendEvent(DreamEvent event);
    public native void nativeEnvJ2C(String sdcardPath);

    public static native void nativeDrawIteration();
    
}

