using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Loader.NET.SDK.Win32
{
    internal class ManualMapInjector
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Auto)]
        private delegate bool Inject(byte[] rawData, int pId);

        internal static bool InjectDll(byte[] dllData, int pId)
        {
            string tmpFile = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
            File.WriteAllBytes(tmpFile, Properties.Resources._mmi_);

            IntPtr pDll = NativeMethods.LoadLibrary(tmpFile);
            IntPtr pAddr = NativeMethods.GetProcAddress(pDll, "_Inject@8");
            Inject inject = (Inject) Marshal.GetDelegateForFunctionPointer(pAddr, typeof(Inject));

            bool result = inject(dllData, pId);

            NativeMethods.FreeLibrary(pDll);

            File.Delete(tmpFile);

            return result;
        }
    }
}