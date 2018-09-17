package com.taos.up.ndkplayer;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class DNPlayer implements SurfaceHolder.Callback {
    private String dataSource;
    private OnPreparedListener listener;

    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 让使用者 设置播放文件或者直播地址
     */
    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    /**
     * 设置播放显示的画布
     *
     * @param surfaceView
     */
    public void setSurfaceView(SurfaceView surfaceView) {
        SurfaceHolder holder = surfaceView.getHolder();
        /**
         * 监听画布的准备情况
         */
        holder.addCallback(this);
        Surface surface = holder.getSurface();
    }

    /**
     * 准备视频
     * 进行视频解封装
     */
    public void prepare() {
        native_prepare(dataSource);
    }

    public void onPrepare(){
        if (listener!=null){
            listener.onPrepared();
        }
    }

    public void start() {
        native_start();
    }

    public void stop() {

    }

    public void release() {

    }


    /**
     * 画布创建成功
     *
     * @param surfaceHolder
     */
    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    /**
     * 画布改变（横竖屏切换）
     *
     * @param surfaceHolder
     * @param i
     * @param i1
     * @param i2
     */
    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        native_setSurface(surfaceHolder.getSurface());
    }

    /**
     * 画布销毁 （按home 退出当前页面）
     *
     * @param surfaceHolder
     */
    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    public void onError(int errorCode) {

    }

    native void native_prepare(String dataSource);

    native void native_start();

    native void native_setSurface(Surface surface);

    interface OnPreparedListener {
        void onPrepared();
    }

    public void setOnPreparedListener(OnPreparedListener listener) {
        this.listener = listener;
    }
}
