package com.tcn.demo;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import androidx.annotation.Nullable;

public class AntiDebugService extends Service {

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        startAntiDebuggingPeriodically();
        return START_STICKY;
    }

    private void startAntiDebuggingPeriodically() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                AntiDebugManager antiDebugManager = new AntiDebugManager();
                antiDebugManager.antiDebug();
                antiDebugManager.X();
            }
        }).start();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}

