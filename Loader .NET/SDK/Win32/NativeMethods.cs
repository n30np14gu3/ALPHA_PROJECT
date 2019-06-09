using System;
using System.Runtime.InteropServices;

namespace Loader.NET.SDK.Win32
{
    internal class NativeMethods
    {
        [DllImport("kernel32.dll")]
        internal static extern IntPtr LoadLibrary(string dllToLoad);

        [DllImport("kernel32.dll")]
        internal static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

        [DllImport("kernel32.dll")]
        internal static extern bool FreeLibrary(IntPtr hModule);
    }
}