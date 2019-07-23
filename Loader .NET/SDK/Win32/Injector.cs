using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Loader.NET.SDK.Win32
{
    internal class Injector
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Auto)]
        private delegate bool Inject(Int32 pId, string szDll, bool deleteAfterInject);


        internal static bool ManualMapInject(Int32 pId, string szDll, bool deleteAfterInject)
        {
            string tmpFile = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
            File.WriteAllBytes(tmpFile, Properties.Resources._mmi_);

            IntPtr _pDll = NativeMethods.LoadLibrary(tmpFile);
            IntPtr pAddr = NativeMethods.GetProcAddress(_pDll, "_Inject@12");
            Inject inject = (Inject) Marshal.GetDelegateForFunctionPointer(pAddr, typeof(Inject));

            bool result = inject(pId, szDll, deleteAfterInject);

            NativeMethods.FreeLibrary(_pDll);
            File.Delete(tmpFile);

            if (deleteAfterInject && File.Exists(szDll))
                File.Delete(szDll);

            return result;
        }
    }
}