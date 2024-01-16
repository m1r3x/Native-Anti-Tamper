package com.tcn.demo;

public class AntiDebugManager {
    static {
        System.loadLibrary("anti_debug"); // Load your native library
    }

    // Native method declaration to initiate anti-debugging checks
    public native static void X();
    public native static void antiDebug();

}


