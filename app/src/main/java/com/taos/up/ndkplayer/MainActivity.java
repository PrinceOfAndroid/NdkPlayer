package com.taos.up.ndkplayer;

import android.os.Looper;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    private DNPlayer dnPlayer;
    private SurfaceView surfaceView;
    private Button btnStart;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = findViewById(R.id.surfaceView);
        btnStart = findViewById(R.id.btn_start);

        // Example of a call to a native method

//        Toast.makeText(MainActivity.this,stringFromJNI(),Toast.LENGTH_SHORT).show();

        dnPlayer = new DNPlayer();
        dnPlayer.setSurfaceView(surfaceView);
        dnPlayer.setDataSource("rtmp://live.hkstv.hk.lxdns.com/live/hks");
//        dnPlayer.prepare();
//        testThread();\
        dnPlayer.setOnPreparedListener(new DNPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this,"准备成功",Toast.LENGTH_SHORT).show();
                    }
                });
                dnPlayer.start();
            }
        });

        btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                dnPlayer.prepare();
            }
        });

    }

    public void updateUI() {
        Log.e("sss", "执行了？？");
        if (Looper.myLooper() == Looper.getMainLooper()) {
            Toast.makeText(this, "更新UI", Toast.LENGTH_LONG).show();
        } else {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(MainActivity.this, "更新UI", Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void testThread();
}
