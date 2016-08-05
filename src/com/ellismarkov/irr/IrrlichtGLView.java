package com.ellismarkov.irr;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;



/**  IrrlichtGLView */
class IrrlichtGLView extends GLSurfaceView {
    private IrrlichtTest mActivity;

    public IrrlichtGLView(IrrlichtTest activity) {
        super(activity);
        mActivity = activity;
        mRenderer = new IrrlichtRenderer(activity);
        setRenderer(mRenderer);
        
    }

    public boolean onTouchEvent(final MotionEvent event) {
    	IrrlichtEvent e=  new IrrlichtEvent();
    	e.mAction = event.getAction();
        e.mX = event.getX();
        e.mY = event.getY();
        //MotionEvent.ACTION_MOVE  
        
        if (mRenderer!=null) {
        	mActivity.PushEvent(e);
           // mActivity.nativeSendEvent(lastEvent);//不要直接发送。。会多线程导致崩溃。
        }
        return true;
    }

    IrrlichtRenderer mRenderer;




}

class IrrlichtRenderer implements GLSurfaceView.Renderer {
    private IrrlichtTest mActivity;
    private int mWindowWidth;
    private int mWindowHeight;
    private IrrlichtStatus mStatus;

    public IrrlichtRenderer(IrrlichtTest activity) {
        mActivity = activity;
        mStatus = new IrrlichtStatus();
        mStatus.mQuit = false;
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mActivity.nativeInitGL();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mWindowWidth = width;
        mWindowHeight = height;
        mActivity.nativeResize(width, height);
    }

    public void onDrawFrame(GL10 gl) {
    	
    	try{
    		mActivity.FlushEvent();
        	
            // render a frame
            mActivity.nativeDrawIteration();
            // get status
            mActivity.nativeGetStatus(mStatus);
            if (mStatus.mQuit) {
                Log.e("MESSAGE", "QUIT");
                mActivity.finish();
            }
    	}catch(Exception e){
    		e.printStackTrace();
    	}
    	 
    }

}

class IrrlichtEvent {
    public int mAction;
    public float mX;
    public float mY;
}

class IrrlichtStatus {
    public boolean mQuit;
}
