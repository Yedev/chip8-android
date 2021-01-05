package com.ych.chip8;

public class ChipJNI {
    static {
        System.loadLibrary("vmchip8-lib");
    }

    public native static long jcreateChip8();

    public native static boolean jload(long ptr, String path);

    public native static int jloop(long ptr, byte[] pixs);

    public native static void jdestory(long ptr);

    /**
     * 按键事件
     *
     * @param index   0x1 - 0xF
     * @param pressed 1 down 0 up
     */
    public native static void jkeyEvent(long ptr, int index, int pressed);

    /**
     * call from native , play game's sound
     */
    public static void beep() {
        BitBGM.get().beep();
    }

}
