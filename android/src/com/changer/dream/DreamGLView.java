package com.changer.dream;

import android.annotation.SuppressLint;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;



/**  DreamGLView */
class DreamGLView extends GLSurfaceView {
    private DreamTest mActivity;
    private DreamEvent lastEvent;

    public DreamGLView(DreamTest activity) {
        super(activity);
        mActivity = activity;
		setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(2);
        mRenderer = new DreamRenderer(activity);
        setRenderer(mRenderer);
        lastEvent = new DreamEvent();
    }

    @SuppressLint("ClickableViewAccessibility")
	public boolean onTouchEvent(final MotionEvent event) {
        lastEvent.mAction = event.getAction();
        lastEvent.mX = event.getX();
        lastEvent.mY = event.getY();
        if (mRenderer!=null) {
            mActivity.nativeSendEvent(lastEvent);
        }
        return true;
    }

    DreamRenderer mRenderer;

}

class DreamRenderer implements GLSurfaceView.Renderer {
    private DreamTest mActivity;
    private DreamStatus mStatus;

    public DreamRenderer(DreamTest activity) {
        mActivity = activity;
        mStatus = new DreamStatus();
        mStatus.mQuit = false;
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mActivity.nativeInitGL();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mActivity.nativeResize(width, height);
    }

    public void onDrawFrame(GL10 gl) {
        // render a frame
        DreamTest.nativeDrawIteration();
        // get status
        mActivity.nativeGetStatus(mStatus);
        if (mStatus.mQuit) {
            Log.e("MESSAGE", "QUIT");
            mActivity.finish();
        }
    }

}

class DreamEvent {
    public int mAction;
    public float mX;
    public float mY;
}

class DreamStatus {
    public boolean mQuit;
}
