using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Reflection;

namespace Loader.NET
{
    public class Program
    {
        static Dictionary<string, Assembly> assembliesDictionary = new Dictionary<string, Assembly>();

        [STAThread]
        public static void Main()
        {
            AppDomain.CurrentDomain.AssemblyResolve += OnResolveAssembly;
            App.Main();
        }

        private static Assembly OnResolveAssembly(object sender, ResolveEventArgs args)
        {
            AssemblyName assemblyName = new AssemblyName(args.Name);
            Assembly executingAssembly = Assembly.GetExecutingAssembly();
            string path = string.Format("{0}.dll", assemblyName.Name);

            if (assemblyName.CultureInfo.Equals(CultureInfo.InvariantCulture) == false)
            {
                path = String.Format(@"{0}\{1}", assemblyName.CultureInfo, path);
            }

            if (!assembliesDictionary.ContainsKey(path))
            {
                using (Stream assemblyStream = executingAssembly.GetManifestResourceStream(path))
                {
                    if (assemblyStream != null)
                    {
                        var assemblyRawBytes = new byte[assemblyStream.Length];
                        assemblyStream.Read(assemblyRawBytes, 0, assemblyRawBytes.Length);
                        using (var pdbStream = executingAssembly.GetManifestResourceStream(Path.ChangeExtension(path, "pdb")))
                        {
                            if (pdbStream != null)
                            {
                                var pdbData = new Byte[pdbStream.Length];
                                pdbStream.Read(pdbData, 0, pdbData.Length);
                                var assembly = Assembly.Load(assemblyRawBytes, pdbData);
                                assembliesDictionary.Add(path, assembly);
                                return assembly;
                            }
                        }
                        assembliesDictionary.Add(path, Assembly.Load(assemblyRawBytes));
                    }
                    else
                    {
                        assembliesDictionary.Add(path, null);
                    }
                }
            }
            return assembliesDictionary[path];
        }
    }
}