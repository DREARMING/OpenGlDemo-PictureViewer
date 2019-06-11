package com.mvcoder.opengldemo;

import android.Manifest;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import java.io.File;
import java.nio.ByteBuffer;
import java.util.List;

import pub.devrel.easypermissions.EasyPermissions;

public class MainActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final String TAG = MainActivity.class.getSimpleName();

    private Button btLoad;
    private Button btWater;
    private SurfaceView surfaceView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        requestPermission();
    }

    private PicViewer picViewer;

    private void initView() {
        surfaceView = findViewById(R.id.surfaceview);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                if(picViewer == null){
                    picViewer = new PicViewer();
                }
                picViewer.onSurfaceCreate(holder.getSurface(), surfaceView.getWidth(), surfaceView.getHeight());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if(picViewer != null){
                    picViewer.onSurfaceDestroy();
                    picViewer = null;
                }
            }
        });
        btLoad = findViewById(R.id.btLoad);
        btLoad.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               String imagePath =  Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath() + "/img_test.png";
               Log.d(TAG,"filepath : " + imagePath);
                File imageFile = new File(imagePath);
                if(imageFile.exists()){
                    Log.d(TAG, "imageFile exist");
                   // converJpgToRGBTexture(imageFile.getAbsolutePath());
                    if(picViewer != null){
                        picViewer.showPicture(imagePath);
                    }
                }else{
                    Log.d(TAG, "image file not exist");
                }
            }
        });


        btWater = findViewById(R.id.btWaterMap);
        btWater.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //创建一张文字图片
                int width = surfaceView.getWidth();
                int height = surfaceView.getHeight();
                Bitmap fontBitmap = Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(fontBitmap);
                //抗锯齿
                Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
                //颜色
                paint.setColor(Color.YELLOW);
                paint.setTextSize(16);
                // text shadow
                paint.setShadowLayer(1f, 0f, 1f, Color.WHITE);

                String gText = "mvcoder";
                // draw text to the Canvas center
                Rect bounds = new Rect();
                //	    paint.setTextAlign(Align.CENTER);

                paint.getTextBounds(gText, 0, gText.length(), bounds);
                int x = (fontBitmap.getWidth() - bounds.width())/2;
                int y = (fontBitmap.getHeight() + bounds.height())/2;

                canvas.drawText(gText,x,y,paint);

                int capacity = width * height * 4;
                ByteBuffer byteBuffer = ByteBuffer.allocate(capacity);
                fontBitmap.copyPixelsToBuffer(byteBuffer);
                byteBuffer.position(0);

                byte[] buffer = new byte[capacity];
                byteBuffer.get(buffer,0, capacity);
                //取背景图
                String imagePath =  Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath() + "/img_test.png";
                //文字所在的位置、宽高

                if(picViewer!= null){
                    picViewer.showWaterPicture(imagePath, buffer, width, height);
                }

            }
        });
    }

    private void requestPermission(){
        String[] permissions = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE};
        if(!EasyPermissions.hasPermissions(this, permissions)){
            EasyPermissions.requestPermissions(this, "",1, permissions);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        EasyPermissions.onRequestPermissionsResult(requestCode,permissions,grantResults,this);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void converJpgToRGBTexture(String url);

    @Override
    public void onPermissionsGranted(int requestCode, List<String> perms) {

    }

    @Override
    public void onPermissionsDenied(int requestCode, List<String> perms) {

    }
}
