package com.mvcoder.opengldemo;

import android.view.Surface;

public class PicViewer {

    private long nativeContext = -1L;

    public PicViewer(){
        nativeContext =  nativeSetup();
    }


    public void showPicture(String imagePath){
        if(nativeContext != -1L){
            showPicture(nativeContext, imagePath);
        }
    }

    public void onSurfaceCreate(Surface surface, int width, int height){
        if(nativeContext!= -1L){
            onSurfaceCreate(nativeContext, surface, width, height);
        }
    }

    public void onSurfaceDestroy(){
        if(nativeContext!= -1L){
            onSurfaceDestroy(nativeContext);
        }
    }

    private native long nativeSetup();

    private native void showPicture(long nativeContext, String imagePath);

    private native void onSurfaceCreate(long nativeContext, Surface surface, int width, int height);

    private native void onSurfaceDestroy(long nativeContext);


}
